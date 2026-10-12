#include "tcs34725.h"
#include "i2c.h"
#include "delay.h"

#define TCS_CMD           0x80   // bit de commande (acces simple)
#define TCS_CMD_AUTOINC   0xA0   // commande + auto-increment (lecture de plusieurs registres)

#define TCS_REG_ENABLE    0x00
#define TCS_REG_ATIME     0x01
#define TCS_REG_CONTROL   0x0F
#define TCS_REG_ID        0x12
#define TCS_REG_STATUS    0x13
#define TCS_REG_CDATAL    0x14   // puis R (0x16), G (0x18), B (0x1A)

#define TCS_ENABLE_PON    0x01
#define TCS_ENABLE_AEN    0x02

#define TCS_GAIN_1X       0x00
#define TCS_GAIN_4X       0x01
#define TCS_GAIN_16X      0x02
#define TCS_GAIN_60X      0x03

#define TCS_ATIME_VALUE   0xEB   // (256-0xEB) x 2,4 ms = ~50 ms d'integration
#define TCS_GAIN_VALUE    TCS_GAIN_4X

// Ecrit une valeur dans un registre du TCS34725
static void TCS_Write(uint8_t reg, uint8_t value)
{
  I2C1_Start();
  I2C1_Write_Address(TCS34725_ADDRESS, I2C_Direction_Transmitter);
  I2C1_Write_Data(TCS_CMD | reg);
  I2C1_Write_Data(value);
  I2C1_Stop();
}

// Lit n octets consecutifs a partir du registre reg (auto-increment)
static void TCS_Read(uint8_t reg, uint8_t *buf, uint8_t n)
{
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  I2C_PECPositionConfig(I2C1, I2C_NACKPosition_Current);

  I2C1_Start();
  I2C1_Write_Address(TCS34725_ADDRESS, I2C_Direction_Transmitter);
  I2C1_Write_Data(TCS_CMD_AUTOINC | reg);
  I2C1_Restart();
  I2C1_Write_Address(TCS34725_ADDRESS, I2C_Direction_Receiver);
  I2C1_Read_Buffer(buf, n);
}

uint8_t TCS34725_ReadID(void)
{
  uint8_t id = 0;
  TCS_Read(TCS_REG_ID, &id, 1);
  return id;
}

void TCS34725_Init(void)
{
  TCS_Write(TCS_REG_ATIME, TCS_ATIME_VALUE);
  TCS_Write(TCS_REG_CONTROL, TCS_GAIN_VALUE);
  TCS_Write(TCS_REG_ENABLE, TCS_ENABLE_PON);
  Delay_ms(3);                                            // >= 2,4 ms apres PON
  TCS_Write(TCS_REG_ENABLE, TCS_ENABLE_PON | TCS_ENABLE_AEN);
  Delay_ms(TCS34725_INTEG_MS + 3);                        // 1re integration
}

void TCS34725_ReadRGBC(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b)
{
  uint8_t d[8];

  TCS_Read(TCS_REG_CDATAL, d, 8);
  *c = (uint16_t)(d[1] << 8) | d[0];
  *r = (uint16_t)(d[3] << 8) | d[2];
  *g = (uint16_t)(d[5] << 8) | d[4];
  *b = (uint16_t)(d[7] << 8) | d[6];
}
