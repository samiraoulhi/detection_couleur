#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "i2c.h"
#include "usart.h"
#include "lcd.h"
#include "delay.h"
#include "tcs34725.h"

/* ---------------- Reglages ---------------- */
#define N_SAMPLES         4      // nombre de mesures moyennees
#define DEBUG_HUE         0      // 1 : envoie clear + teinte sur l'UART (pour regler les seuils)

// Color flags
#define PURPLE_FLAG 1
#define BLUE_FLAG   2
#define CYAN_FLAG   3
#define GREEN_FLAG  4
#define PINK_FLAG   5
#define RED_FLAG    6
#define ORANGE_FLAG 7
#define YELLOW_FLAG 8

// Find maximal floating point value
#define MAX_FLOAT(a, b) (((a) > (b)) ? (a) : (b))

// Find minimal floating point value
#define MIN_FLOAT(a, b) (((a) < (b)) ? (a) : (b))

// Variable declaration
char i, color_detected, color_flag;
uint16_t Clear, Red, Green, Blue;
float color_value, color_value_sum;
float Red_Ratio, Green_Ratio, Blue_Ratio;

// Convert RGB values to HSL values
float RGB_To_HSL(float red, float green, float blue) {
  float volatile fmax, fmin, hue, saturation, luminance;
 
  fmax = MAX_FLOAT(MAX_FLOAT(red, green), blue);
  fmin = MIN_FLOAT(MIN_FLOAT(red, green), blue);
 
  luminance = fmax;
  if (fmax > 0)
    saturation = (fmax - fmin) / fmax;
  else
    saturation = 0;
 
  if (saturation == 0)
    hue = 0;
  else {
    if (fmax == red)
      hue = (green - blue) / (fmax - fmin);
    else if (fmax == green)
      hue = 2 + (blue - red) / (fmax - fmin);
    else
      hue = 4 + (red - green) / (fmax - fmin);
    hue = hue / 6;
  
    if (hue < 0)
      hue += 1;
  }
  return hue;
}

// Affiche la couleur detectee sur l'UART et sur la 2e ligne du LCD
void Show_Color(const char *name) {
  UART_Write_Line((char *)name);
  LCD_PrintLine(1, name);
}

int main(void) {
  uint8_t id = 0;
  uint8_t n;
  char text[24];

  // Initialize variables
  color_value_sum = 0;
  color_detected = 0;
  color_flag = 0;

  // Bus I2C1 partage par le LCD et le capteur (100 kHz : limite du PCF8574)
  I2C1_LowLevel_Init(100000, OwnAddress1);
  Delay_ms(100);

  // LCD d'abord : si l'affichage reste sur "Init capteur...", le capteur ne repond pas
  LCD_Init();
  LCD_PrintLine(0, "Init capteur...");

  // Initialize UART1 module
  USART1_init();
  UART_Write_Line("TCS34725 Color Test");

  // Verification de la presence du capteur
  id = TCS34725_ReadID();
  sprintf(text, "ID=0x%02X", id);
  LCD_PrintLine(1, text);
  UART_Write_Line(text);
  Delay_ms(1000);

  if (id != 0x44 && id != 0x4D) {
    LCD_PrintLine(0, "Capteur inconnu");
    UART_Write_Line("Capteur inconnu (ID attendu 0x44)");
    while (1);
  }

  TCS34725_Init();
  LCD_PrintLine(0, "Couleur :");
  LCD_PrintLine(1, "...");

  while(1) {
    // Moyenne de N_SAMPLES mesures
    color_value_sum = 0;
    n = 0;

    for(i = 0; i < N_SAMPLES; i++) {
      Delay_ms(TCS34725_INTEG_MS);                       // attendre une nouvelle integration
      TCS34725_ReadRGBC(&Clear, &Red, &Green, &Blue);

      if (Clear == 0) continue;                     // evite la division par zero

      // Divide Red, Green and Blue values with Clear value
      Red_Ratio   = ((float)Red   / (float)Clear);
      Green_Ratio = ((float)Green / (float)Clear);
      Blue_Ratio  = ((float)Blue  / (float)Clear);

      // Convert RGB values to HSL values
      color_value = RGB_To_HSL(Red_Ratio, Green_Ratio, Blue_Ratio);

      // Sum the color values
      color_value_sum = color_value_sum + color_value;
      n++;
    }

    if (n > 0) color_value = color_value_sum / (float)n;
    else       color_value = -1.0f;                 // aucune mesure valide -> hors plage

#if DEBUG_HUE
    sprintf(text, "C=%u hue=%d", Clear, (int)(color_value * 1000));
    UART_Write_Line(text);
#endif

    // Orange color range
    if ((color_value >= 0.992) && (color_value <= 0.999)) {
      color_detected = 1;
      if (color_flag != ORANGE_FLAG){
        color_flag = ORANGE_FLAG;
        Show_Color("Orange");
      }
    }

    // Red color range
    else if ((color_value >= 0.9750) && (color_value <= 0.9919)) {
      color_detected = 1;
      if (color_flag != RED_FLAG){
        color_flag = RED_FLAG;
        Show_Color("Red");
      }
    }

    // Pink color range
    else if ((color_value >= 0.920) && (color_value <= 0.9749)) {
      color_detected = 1;
      if (color_flag != PINK_FLAG){
        color_flag = PINK_FLAG;
        Show_Color("Pink");
      }
    }

    // Purple color range
    else if ((color_value >= 0.6201) && (color_value <= 0.919)) {
      color_detected = 1;
      if (color_flag != PURPLE_FLAG){
        color_flag = PURPLE_FLAG;
        Show_Color("Purple");
      }
    }

    // Blue color range
    else if ((color_value >= 0.521) && (color_value <= 0.6200)) {
      color_detected = 1;
      if (color_flag != BLUE_FLAG){
        color_flag = BLUE_FLAG;
        Show_Color("Blue");
      }
    }

    // Cyan color range
    else if ((color_value >= 0.470) && (color_value < 0.520)) {
      color_detected = 1;
      if (color_flag != CYAN_FLAG){
        color_flag = CYAN_FLAG;
        Show_Color("Cyan");
      }
    }

    // Green color range
    else if ((color_value >= 0.210) && (color_value <= 0.469)) {
      color_detected = 1;
      if (color_flag != GREEN_FLAG){
        color_flag = GREEN_FLAG;
        Show_Color("Green");
      }
    }

    // Yellow color range
    else if ((color_value >= 0.0650) && (color_value <= 0.1800)) {
      color_detected = 1;
      if (color_flag != YELLOW_FLAG){
        color_flag = YELLOW_FLAG;
        Show_Color("Yellow");
      }
    }

    // Color not in range
    else {
      if (color_detected == 0){
        color_flag = 0;
        Delay_ms(500);
        UART_Write_Line("Color not in range.");
        LCD_PrintLine(1, "Not in range");
      }
      else {
        color_detected = 0;
      }
    }
  }
}
