#ifndef __TCS34725_H
#define __TCS34725_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "writeFlash.h"
#include "ssd1306.h"
#include "test.h"
#include "stm32f4xx_hal.h"
#include "servosandbuttons.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define UMBRAL 150
void Test_cts34725(void);
void Init_cts34725(void);
void Read_cts34725(void);
void Store_Colors(void);
void Show_console(void);
void CicleColor(void);
void StartThreshold(void);
void mapData(void);
void CalibrateColour(void);
void DefineColour(uint32_t colour);
void resetvar(void);
void affich(void);
#ifdef __cplusplus
}
#endif
#endif /* __TCS34725_H */
