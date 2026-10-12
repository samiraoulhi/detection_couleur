#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_i2c.h"

#define  ClockSpeed            400000

#define I2C_DIRECTION_TX 0
#define I2C_DIRECTION_RX 1

#define OwnAddress1 0x28
#define OwnAddress2 0x30

#define I2C1_DR_Address              0x40005410

/* Choix des broches I2C1 (SCL est toujours PB6) :
     1 -> SDA sur PB9  (F4-Discovery)
     0 -> SDA sur PB7  (autres cartes F407)
   A adapter selon le schema de ta carte. */
#define I2C1_SDA_ON_PB9  1

/* Deja definis dans la bibliotheque F4, gardes par precaution */
#ifndef I2C_NACKPosition_Next
#define I2C_NACKPosition_Next      ((uint16_t)0x0800)
#endif
#ifndef I2C_NACKPosition_Current
#define I2C_NACKPosition_Current   ((uint16_t)0xF7FF)
#endif

void I2C1_Start(void);
void I2C1_Write_Address(uint8_t address, uint8_t direction);
void I2C1_Write_Data(uint8_t data);
void I2C1_Stop(void);
void I2C1_Restart(void);
void I2C1_LowLevel_Init(int clockSpeed , int OwnAddress);
uint8_t I2C1_Read(void);
void I2C1_Read_Buffer(uint8_t *buf, uint8_t nbyte);   /* lecture de 1 a N octets + STOP */

#endif //I2C_H_
