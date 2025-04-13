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
	uint32_t Periphs;
	GPIO_TypeDef *GPIOPORT;
	uint32_t PINx;
public:
	GPIO(GPIO_TypeDef *GPIOx,uint32_t pin);
	static inline void SetPinSpeed(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t speed);
	static inline void SetPinPull(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t pull);
	static inline void SetPinMode(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t mode);
	static inline void SetAlternate0_7(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t alternate);
	static inline void SetAlternate8_15(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t alternate);
	static inline void SetOutputPinType(GPIO_TypeDef *GPIOx,uint32_t pinmask,uint32_t outputType);
	uint32_t GetPortNumber(void);
	GPIO_Code OutputInit(GPIO_InitTypedef *InitStruct);
	static GPIO_Code OutputInit(GPIO_TypeDef *GPIOx,GPIO_InitTypedef *InitStruct,uint32_t pinpos);
	static GPIO_Code InputInit(GPIO_TypeDef *GPIOx,GPIO_Port PORTx,uint32_t pinpos,uint32_t Mode,uint32_t Pull);
};

inline void GPIO::SetPinSpeed(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t speed)
{
	MODIFY_REG(GPIOx->OSPEEDR,GPIO_OSPEEDR_OSPEED0 << (pinpos * 2),(speed << (pinpos * 2)));
}
inline void GPIO::SetPinPull(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t pull)
{
	MODIFY_REG(GPIOx->PUPDR,GPIO_PUPDR_PUPD0 << (pinpos * 2),(pull << (pinpos * 2)));
}
inline void GPIO::SetPinMode(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t mode)
{
	MODIFY_REG(GPIOx->MODER,GPIO_MODER_MODE0 << (pinpos * 2),(mode << (pinpos * 2)));
}
inline void GPIO::SetAlternate0_7(GPIO_TypeDef* GPIOx,uint32_t pinpos,uint32_t alternate)
{
	MODIFY_REG(GPIOx->AFR[0],GPIO_AFRL_AFSEL0 << (pinpos * 4),alternate << (pinpos * 4));
}
inline void GPIO::SetAlternate8_15(GPIO_TypeDef* GPIOx,uint32_t pinpos,uint32_t alternate)
{
	pinpos -= 8;
	MODIFY_REG(GPIOx->AFR[1],GPIO_AFRH_AFSEL8 << (pinpos * 4),alternate << (pinpos * 4));
}
inline void GPIO::SetOutputPinType(GPIO_TypeDef *GPIOx,uint32_t pinmask,uint32_t outputType)
{
	MODIFY_REG(GPIOx->OTYPER,pinmask,(pinmask * outputType));
}


#endif /* GPIO_H_ */
