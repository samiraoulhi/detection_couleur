#include <string.h>
#include "stm32f4xx_it.h"
#include "stm32f4xx_conf.h"

char state = 0;

/* Exceptions du coeur Cortex-M4 */
void NMI_Handler(void) { }

void HardFault_Handler(void)   { while (1) { } }
void MemManage_Handler(void)   { while (1) { } }
void BusFault_Handler(void)    { while (1) { } }
void UsageFault_Handler(void)  { while (1) { } }

void SVC_Handler(void)      { }
void DebugMon_Handler(void) { }
void PendSV_Handler(void)   { }

/* SysTick est utilise en polling par delay.c : pas d'interruption */
void SysTick_Handler(void)  { }

/* Interruptions peripheriques (vides, comme dans le projet d'origine) */
void TIM2_IRQHandler(void)   { }
void EXTI0_IRQHandler(void)  { }
void USART1_IRQHandler(void) { }

uint8_t isDigit(uint8_t digit) {
    if (digit >= 48 && digit <= 57) return 1;
    else return 0;
}
