#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f4xx.h"

/* Delais reels bases sur SysTick (polling, sans interruption).
   Delay_us : valeur max ~ 99000 us a 168 MHz. */
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);

#endif
