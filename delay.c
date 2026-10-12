#include "delay.h"

void Delay_us(uint32_t us)
{
  uint32_t ticks = us * (SystemCoreClock / 1000000UL);

  SysTick->LOAD = ticks - 1;
  SysTick->VAL  = 0;
  SysTick->CTRL = 0x05;                    /* horloge coeur, timer ON, pas d'IT */
  while (!(SysTick->CTRL & 0x00010000));   /* attend COUNTFLAG */
  SysTick->CTRL = 0;
}

void Delay_ms(uint32_t ms)
{
  while (ms--) Delay_us(1000);
}
