#include "i2c.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx.h"

#define Timed(x) while (x)

#if I2C1_SDA_ON_PB9
  #define I2C1_SDA_PIN  GPIO_Pin_9
  #define I2C1_SDA_SRC  GPIO_PinSource9
#else
  #define I2C1_SDA_PIN  GPIO_Pin_7
  #define I2C1_SDA_SRC  GPIO_PinSource7
#endif

void I2C1_Start(void)
{
	Timed(I2C_GetFlagStatus(I2C1 , I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1 , ENABLE);
	Timed (! I2C_CheckEvent(I2C1 , I2C_EVENT_MASTER_MODE_SELECT));
}

/* address : adresse 7 bits (decalee ici) */
void I2C1_Write_Address(uint8_t address, uint8_t direction)
{
	I2C_Send7bitAddress(I2C1 , address<<1 , direction);
	if(direction == I2C_Direction_Transmitter)
		Timed (! I2C_CheckEvent(I2C1 ,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	else
		Timed (! I2C_GetFlagStatus(I2C1 ,I2C_FLAG_ADDR));
}

void I2C1_Write_Data(uint8_t data)
{
	I2C_SendData(I2C1 , data);
	Timed (! I2C_GetFlagStatus(I2C1 , I2C_FLAG_BTF));
}

void I2C1_Stop(void)
{
	I2C_GenerateSTOP(I2C1 , ENABLE);
	Timed(I2C_GetFlagStatus(I2C1 , I2C_FLAG_STOPF));
}

void I2C1_Restart(void)
{
	I2C_GenerateSTART(I2C1 , ENABLE);
	Timed (! I2C_CheckEvent(I2C1 , I2C_EVENT_MASTER_MODE_SELECT));
}

/* Lecture d'un seul octet (sans parametre NACK) */
uint8_t I2C1_Read(void)
{
	I2C_AcknowledgeConfig(I2C1 , DISABLE);
	(void) I2C1 ->SR2;
	Timed (! I2C_GetFlagStatus(I2C1 , I2C_FLAG_RXNE));
	return I2C_ReceiveData(I2C1);
}

/* Lecture de nbyte octets (nbyte >= 1) apres I2C1_Write_Address(..., Receiver).
   Genere le STOP et remet l'ACK a la fin. Sequence d'apres AN2824. */
void I2C1_Read_Buffer(uint8_t *buf, uint8_t nbyte)
{
	if (nbyte == 0) return;

	if (nbyte == 1)
	{
		I2C_AcknowledgeConfig(I2C1 , DISABLE);
		__disable_irq();
		(void) I2C1 ->SR2;                       // efface ADDR
		I2C_GenerateSTOP(I2C1 , ENABLE);
		__enable_irq();
		Timed (! I2C_GetFlagStatus(I2C1 , I2C_FLAG_RXNE));
		*buf = I2C_ReceiveData(I2C1);
	}
	else if (nbyte == 2)
	{
		I2C_PECPositionConfig(I2C1 , I2C_NACKPosition_Next);
		__disable_irq();
		(void) I2C1 ->SR2;                       // efface ADDR
		I2C_AcknowledgeConfig(I2C1 , DISABLE);
		__enable_irq();
		Timed (! I2C_GetFlagStatus(I2C1 , I2C_FLAG_BTF));
		__disable_irq();
		I2C_GenerateSTOP(I2C1 , ENABLE);
		*buf++ = I2C_ReceiveData(I2C1);
		__enable_irq();
		*buf = I2C_ReceiveData(I2C1);
		I2C_PECPositionConfig(I2C1 , I2C_NACKPosition_Current);
	}
	else
	{
		(void) I2C1 ->SR2;                       // efface ADDR

		while (nbyte-- != 3)
		{
			Timed (! I2C_GetFlagStatus(I2C1 , I2C_FLAG_BTF));
			*buf++ = I2C_ReceiveData(I2C1);
		}

		Timed (! I2C_GetFlagStatus(I2C1 , I2C_FLAG_BTF));
		I2C_AcknowledgeConfig(I2C1 , DISABLE);
		__disable_irq();
		*buf++ = I2C_ReceiveData(I2C1);          // octet N-2
		I2C_GenerateSTOP(I2C1 , ENABLE);
		__enable_irq();
		*buf++ = I2C_ReceiveData(I2C1);          // octet N-1
		Timed (! I2C_GetFlagStatus(I2C1 , I2C_FLAG_RXNE));
		*buf = I2C_ReceiveData(I2C1);            // octet N
	}

	I2C_AcknowledgeConfig(I2C1 , ENABLE);
}

/* Version STM32F4 : GPIOB sur AHB1, fonction alternative AF4 */
void I2C1_LowLevel_Init(int clockSpeed , int OwnAddress)
{
	I2C_TypeDef* I2Cx = I2C1;
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 , ENABLE);

	GPIO_PinAFConfig(GPIOB , GPIO_PinSource6 , GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB , I2C1_SDA_SRC , GPIO_AF_I2C1);

	/* I2C1 SCL (PB6) et SDA : open-drain obligatoire */
	GPIO_StructInit (& GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | I2C1_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB , &GPIO_InitStructure);

	/* Reset I2C1 */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1 , ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1 , DISABLE);

	I2C_StructInit (& I2C_InitStructure);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = OwnAddress;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = clockSpeed;
	I2C_Init(I2Cx , &I2C_InitStructure);
	I2C_Cmd(I2Cx , ENABLE);
}
