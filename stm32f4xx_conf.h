#ifndef __STM32F4xx_CONF_H
#define __STM32F4xx_CONF_H

/* Peripheriques utilises par le projet */
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

/* #define USE_FULL_ASSERT    1 */

#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif

#endif /* __STM32F4xx_CONF_H */
