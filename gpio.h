/*
 * gpio.h
 *
 *  Created on: Apr 11, 2025
 *      Author: wataoxp
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "periph.h"

#define GPIO_WRITE(GPIOx,Pin) (GPIOx->BSRR = 1 << Pin)
#define GPIO_CLEAR(GPIOx,Pin) (GPIOx->BRR = 1 << Pin)
#define GPIO_READ(GPIOx,Pin) ((GPIOx->IDR & 1 << Pin) >> Pin)

typedef enum{
	GPIO_Success,
	GPIO_Failed,
	GPIO_NoPort,
	SWDPin,
}GPIO_Code;

typedef struct{
	uint32_t Mode;
	uint32_t Speed;
	uint32_t OutputType;
	uint32_t Pull;
	uint32_t Alternate;
}GPIO_InitTypedef;

class GPIO{
private:
	GPIO_TypeDef *GPIOx;
	GPIO_InitTypedef Config;
	uint32_t PinPos;
	inline void SetPinSpeed(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t speed);
	inline void SetPinPull(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t pull);
	inline void SetPinMode(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t mode);
	inline void SetAlternate0_7(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t alternate);
	inline void SetAlternate8_15(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t alternate);
	inline void SetOutputPinType(GPIO_TypeDef *GPIOx,uint32_t pinmask,uint32_t outputType);
public:
	GPIO(GPIO_TypeDef *GPIOx,uint32_t pin);
	GPIO_Code Begin(void);
	void SetParameter(uint8_t Pull,uint8_t Mode,uint8_t Speed,uint8_t Type);
	uint32_t GetPortNumber(GPIO_TypeDef *GPIOx);
	void OutputInit(void);
	void InputInit(void);
	void AlternateInit(uint32_t Alternate);
};


#endif /* GPIO_H_ */
