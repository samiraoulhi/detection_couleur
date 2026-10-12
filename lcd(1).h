#ifndef LCD_H
#define LCD_H

#include "stm32f4xx.h"

/* Adresse 7 bits du backpack PCF8574 : 0x27 (PCF8574) ou 0x3F (PCF8574A). */
#define LCD_I2C_ADDR  0x27
#define LCD_COLS      16

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(const char *s);
void LCD_PrintLine(uint8_t row, const char *s);  /* ecrit s puis complete avec des espaces */

#endif
