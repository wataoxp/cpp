/*
 * exti.h
 *
 *  Created on: Jan 28, 2025
 *      Author: wataoxp
 */

#ifndef INC_EXTI_H_
#define INC_EXTI_H_

#include "gpio.h"
#include "periph.h"

#define IOPORT_SPACE 0x00000400UL

typedef struct{
	IRQn_Type IRQn;
	uint32_t PinPos;
	uint8_t Port;
	GPIO_TypeDef *GPIOx;
	uint32_t ExtiLine;
	uint32_t Pull;
	uint8_t Mode;
	uint8_t Trigger;
	NVIC_Prioritys prime;
}Exti_ConfigTypedef;

typedef enum{
	Exti_Succses,
	Exti_Noport,
}ExtiCode;

class EXTIR{
private:
	Exti_ConfigTypedef *Exti;
	static constexpr uint32_t EXTICR_Msk = 0x07;
	void StructInit(uint32_t pin);
	void SetSource(uint8_t port,uint32_t pin);
	void ExtiMode(uint8_t Mode,uint32_t Line);
	void ExtiTrigger(uint8_t Trigger,uint32_t Line);
	inline IRQn_Type CheckExtiLine(uint32_t pin);
	inline GPIO_Port CheckPort(GPIO_TypeDef *GPIOx);
public:
	EXTIR(Exti_ConfigTypedef *Exti);
	void Init(void);
};

#endif /* INC_EXTI_H_ */
