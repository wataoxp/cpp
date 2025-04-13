/*
 * gpio.c
 *
 *  Created on: Jan 12, 2025
 *      Author: wataoxp
 */

#include "gpio.h"

GPIO::GPIO(GPIO_TypeDef *GPIOx,uint32_t pin)
{
	GPIOPORT = GPIOx;
	PINx = pin;
}

uint32_t GPIO::GetPortNumber(void)
{
	if(GPIOPORT == GPIOA)
	{
		Periphs = 1 << PORTA;
	}
	else if(GPIOPORT == GPIOB)
	{
		Periphs = 1 << PORTB;
	}
	else if(GPIOPORT == GPIOC)
	{
		Periphs = 1 << PORTC;
	}
	else if(GPIOPORT == GPIOD)
	{
		Periphs = 1 << PORTD;
	}
	else if(GPIOPORT == GPIOF)
	{
		Periphs = 1 << PORTF;
	}
	else
	{
		Periphs = 0;
	}

	return Periphs;
}
GPIO_Code GPIO::OutputInit(GPIO_InitTypedef *InitStruct)
{
	//GPIO::OutputInit(GPIOPORT, InitStruct);
	return this->OutputInit(GPIOPORT, InitStruct,PINx);
}
GPIO_Code GPIO::OutputInit(GPIO_TypeDef *GPIOx,GPIO_InitTypedef *InitStruct,uint32_t pinpos)
{
	uint32_t pin = pinpos;

	if((pin == Pin13) || (pin == Pin14))
	{
		if(GPIOx == GPIOA)
		{
			return SWDPin;
		}
	}
	GPIO_CLEAR(GPIOx,pin);
	GPIO::SetPinSpeed(GPIOx, pin, InitStruct->Speed);
	GPIO::SetOutputPinType(GPIOx, (1 << pin), InitStruct->OutputType);


	if(InitStruct->Mode == LL_GPIO_MODE_ALTERNATE)
	{
		if(pinpos < Pin8)
		{
			GPIO::SetAlternate0_7(GPIOx,pin,InitStruct->Alternate);
		}
		else
		{
			GPIO::SetAlternate8_15(GPIOx,pin,InitStruct->Alternate);
		}
	}
	GPIO::SetPinPull(GPIOx,pin,InitStruct->Pull);
	GPIO::SetPinMode(GPIOx,pin,InitStruct->Mode);

	return GPIO_Success;
}

GPIO_Code GPIO::InputInit(GPIO_TypeDef *GPIOx,GPIO_Port PORTx,uint32_t pinpos,uint32_t Mode,uint32_t Pull)
{
	if((pinpos == Pin13) || (pinpos == Pin14))
	{
		if(GPIOx == GPIOA)
		{
			return SWDPin;
		}
	}

	if(LL_IOP_GRP1_IsEnabledClock(1 << PORTx) == 0)
	{
		LL_IOP_GRP1_EnableClock(1 << PORTx);
	}
	GPIO::SetPinPull(GPIOx,pinpos,Pull);
	GPIO::SetPinMode(GPIOx,pinpos,Mode);

	return GPIO_Success;
}
