#ifndef MAIN_H
#define MAIN_H

/* ############################## INCLUDES ########################## */
#include "tcs34725.h"
#include "lcd16x2_i2c.h"
#include "stm32f1xx_hal.h" // Adapter selon votre microcontrôleur

/* ############################## VARIABLES ########################## */
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim3;

extern uint8_t buttoncalibrate;
extern unsigned int Color_Sensor_Address;
extern uint8_t rojo, verde, azul, morado, total;
extern uint16_t Clear_value, Red_value, Green_value, Blue_value;
extern uint32_t color, ColorsThreshold[4], NR, NG, NB, c;
extern char red_str[10];

/* ############################## PROTOTYPES ########################## */
void Test_cts34725(void);
void Read_cts34725(void);
void affich(void);
void LCD(void);
void DefineColour(uint32_t colour);
void rest_cal(void);
void resetvar(void);

#endif // MAIN_H
