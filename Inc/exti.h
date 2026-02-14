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

namespace ExtiCommands{
	constexpr uint32_t EXTICR_Msk = 0x07;
	constexpr uint32_t IoPortSpace = 0x00000400UL;
}

class EXTIR{
private:
	GPIO_TypeDef *GPIOx;
	uint32_t PinPos;

	inline IRQn_Type CheckExtiLine(void);
	inline GPIO_Port CheckPort(void);

	void SetSource(GPIO_Port port);
	void ExtiMode(uint8_t Mode,uint32_t Line);
	void ExtiTrigger(uint8_t Trigger,uint32_t Line);
public:
	EXTIR(GPIO_TypeDef *GPIOPORT,uint32_t pin);
	uint32_t Config(void);
	void ConfigMode_Trigger(uint8_t Mode,uint8_t Trigger);
};

#endif /* INC_EXTI_H_ */
