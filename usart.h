#ifndef USART_H_
#define USART_H_

#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

void USART1_init(void);
void UART_Write_Line (char* data);

#endif
