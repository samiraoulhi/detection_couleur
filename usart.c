#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "usart.h"

static void UART_Write(const char *data)
{
	size_t len = strlen(data);
	size_t k;

	for (k = 0; k < len; k++) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, (uint16_t)data[k]);
	}
}

/* Envoie le texte puis un retour a la ligne */
void UART_Write_Line (char* data)
{
	UART_Write(data);
	UART_Write("\r\n");
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/* USART1 : TX = PA9 (AF7), 115200 bauds, emission seule */
void USART1_init(void)
{
	GPIO_InitTypeDef initStruct;
	USART_InitTypeDef initStructUSART;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

	GPIO_StructInit(&initStruct);
	initStruct.GPIO_Pin   = GPIO_Pin_9;   // Tx
	initStruct.GPIO_Mode  = GPIO_Mode_AF;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_OType = GPIO_OType_PP;
	initStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &initStruct);

	USART_StructInit(&initStructUSART);
	initStructUSART.USART_BaudRate = 115200;
	initStructUSART.USART_WordLength = USART_WordLength_8b;
	initStructUSART.USART_StopBits = USART_StopBits_1;
	initStructUSART.USART_Parity = USART_Parity_No;
	initStructUSART.USART_Mode = USART_Mode_Tx;
	initStructUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &initStructUSART);
	USART_Cmd(USART1, ENABLE);
}
