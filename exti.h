/*
 * exti.h
 *
 *  Created on: Jan 28, 2025
 *      Author: wataoxp
 */

#ifndef INC_EXTI_H_
#define INC_EXTI_H_

#include <gpio.h>
#include "periph.h"

#define MOD(n,m) (n & (m-1))
#define DIV(n,m) (n >> m)

typedef struct{
	IRQn_Type IRQn;
	uint32_t PinPos;
	GPIO_Port Port;
	GPIO_TypeDef *GPIOx;
	uint32_t ExtiLine;
	uint32_t Pull;
	uint8_t Mode;
	uint8_t Trigger;
}Exti_ConfigTypedef;

void EXTI_Init(Exti_ConfigTypedef *InitStruct);
void EXTI_SetSource(uint32_t port,uint32_t pin);

#endif /* INC_EXTI_H_ */
