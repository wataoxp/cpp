/*
 * periph.h
 *
 *  Created on: Feb 17, 2025
 *      Author: wataoxp
 */

#ifndef SRC_PERIPH_H_
#define SRC_PERIPH_H_

//#define STM32G0xx
///***
// * GPIOD
// * I2C2
// * SPI2
// */
//#define STM32G0x1	//TIM2

#define STM32C0xx

#ifdef STM32G0xx
#include "stm32g0xx.h"

/* System Headers */
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"

/* Add Peripherals */
#include "./stm32g0xx/stm32g0xx_ll_tim.h"
#include "./stm32g0xx/stm32g0xx_ll_i2c.h"
#include "./stm32g0xx/stm32g0xx_ll_spi.h"
#include "./stm32g0xx/stm32g0xx_ll_adc.h"

#else
#include "stm32c0xx.h"

/* System Headers */
#include "stm32c0xx_ll_rcc.h"
#include "stm32c0xx_ll_bus.h"
#include "stm32c0xx_ll_system.h"
#include "stm32c0xx_ll_exti.h"
#include "stm32c0xx_ll_cortex.h"
#include "stm32c0xx_ll_utils.h"
#include "stm32c0xx_ll_pwr.h"
#include "stm32c0xx_ll_dma.h"
#include "stm32c0xx_ll_gpio.h"

/* Add Peripherals */
#include "./stm32c0xx/stm32c0xx_ll_tim.h"
#include "./stm32c0xx/stm32c0xx_ll_i2c.h"
#include "./stm32c0xx/stm32c0xx_ll_spi.h"
#include "./stm32c0xx/stm32c0xx_ll_adc.h"

#endif

typedef enum{
	Pin0,
	Pin1,
	Pin2,
	Pin3,
	Pin4,
	Pin5,
	Pin6,
	Pin7,
	Pin8,
	Pin9,
	Pin10,
	Pin11,
	Pin12,
	Pin13,
	Pin14,
	Pin15,
}GPIO_Pin;

typedef enum{
	PORTA,
	PORTB,
	PORTC,
	PORTD,
	PORTF = 5,
	NoPort,
}GPIO_Port;

typedef enum{
	HighPriority = 0,
	MidPriority,
	LowPriority,
}NVIC_Prioritys;

#define GPIO_WRITE(GPIOx,Pin) (GPIOx->BSRR = 1 << Pin)
#define GPIO_CLEAR(GPIOx,Pin) (GPIOx->BRR = 1 << Pin)
#define GPIO_READ(GPIOx,Pin) ((GPIOx->IDR & 1 << Pin) >> Pin)

#endif /* SRC_PERIPH_H_ */
