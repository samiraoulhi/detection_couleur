#include "lcd.h"
#include "i2c.h"
#include "delay.h"

/* Bits du PCF8574 : D7..D4 = donnees LCD, puis BL, EN, RW, RS */
#define LCD_BACKLIGHT  0x08
#define LCD_EN         0x04
#define LCD_RS         0x01

/* EN=1 puis EN=0 dans la meme transaction I2C */
static void LCD_WriteNibble(uint8_t nibble, uint8_t rs)
{
  uint8_t d = (nibble & 0xF0) | LCD_BACKLIGHT | rs;

  I2C1_Start();
  I2C1_Write_Address(LCD_I2C_ADDR, I2C_Direction_Transmitter);
  I2C1_Write_Data(d | LCD_EN);
  I2C1_Write_Data(d);
  I2C1_Stop();
}

static void LCD_WriteByte(uint8_t value, uint8_t rs)
{
  LCD_WriteNibble(value & 0xF0, rs);
  LCD_WriteNibble((value << 4) & 0xF0, rs);
}

static void LCD_Cmd(uint8_t cmd)
{
  LCD_WriteByte(cmd, 0);
}

void LCD_Init(void)
{
  Delay_ms(50);                  /* mise sous tension du LCD */
  LCD_WriteNibble(0x30, 0); Delay_ms(5);
  LCD_WriteNibble(0x30, 0); Delay_us(150);
  LCD_WriteNibble(0x30, 0); Delay_us(150);
  LCD_WriteNibble(0x20, 0); Delay_us(150);   /* passage en mode 4 bits */

  LCD_Cmd(0x28);                 /* 4 bits, 2 lignes, 5x8 */
  LCD_Cmd(0x0C);                 /* affichage ON, curseur OFF */
  LCD_Cmd(0x06);                 /* incrementation automatique */
  LCD_Clear();
}

void LCD_Clear(void)
{
  LCD_Cmd(0x01);
  Delay_ms(2);
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
  LCD_Cmd(0x80 | (col + (row ? 0x40 : 0x00)));
}

void LCD_Print(const char *s)
{
  while (*s) LCD_WriteByte((uint8_t)*s++, LCD_RS);
}

void LCD_PrintLine(uint8_t row, const char *s)
{
  uint8_t n = 0;

  LCD_SetCursor(0, row);
  while (*s && n < LCD_COLS) { LCD_WriteByte((uint8_t)*s++, LCD_RS); n++; }
  while (n < LCD_COLS)       { LCD_WriteByte(' ', LCD_RS); n++; }
}
