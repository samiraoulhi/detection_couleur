#ifndef TCS34725_H
#define TCS34725_H

#include "stm32f4xx.h"

#define TCS34725_ADDRESS    0x29   // adresse 7 bits
#define TCS34725_INTEG_MS   52     // delai entre deux mesures (>= temps d'integration ~50 ms)

/* Lit le registre ID : 0x44 = TCS34725, 0x4D = TCS34727 */
uint8_t TCS34725_ReadID(void);

/* Configure integration + gain, puis met le capteur sous tension */
void    TCS34725_Init(void);

/* Lit Clear, Red, Green, Blue en une seule transaction I2C (8 octets) */
void    TCS34725_ReadRGBC(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b);

#endif
