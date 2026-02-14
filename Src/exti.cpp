/*
 * exti.c
 *
 *  Created on: Jan 28, 2025
 *      Author: wataoxp
 */
#include "exti.h"

EXTIR::EXTIR(GPIO_TypeDef *GPIOPORT,uint32_t pin) :GPIOx(GPIOPORT),PinPos(pin)
{
	;
}

/*** Private ***/
inline GPIO_Port EXTIR::CheckPort(void)
{
	GPIO_Port ret = (GPIO_Port)UINT8_MAX;

	if(GPIOx == GPIOA)
	{
		ret = PORTA;
	}
	else if(GPIOx == GPIOB)
	{
		ret = PORTB;
	}
	else if(GPIOx == GPIOC)
	{
		ret = PORTC;
	}
#ifdef STM32G0xx
	else if(GPIOx == GPIOD)
	{
		ret = PORTD;
	}
#endif
	else if(GPIOx == GPIOF)
	{
		ret = PORTF;
	}
	else
	{
		;
	}
	return ret;
}

void EXTIR::SetSource(GPIO_Port port)
{
	uint32_t Reg = PinPos / 4;
	uint32_t Bit = (PinPos % 4) * 8;

	MODIFY_REG(EXTI->EXTICR[Reg],ExtiCommands::EXTICR_Msk << Bit, port << Bit);
}

void EXTIR::ExtiMode(uint8_t Mode,uint32_t Line)
{
	switch (Mode)
	{
	case LL_EXTI_MODE_IT:
		LL_EXTI_DisableEvent_0_31(Line);
		LL_EXTI_EnableIT_0_31(Line);
		break;
	case LL_EXTI_MODE_EVENT:
		LL_EXTI_DisableIT_0_31(Line);
		LL_EXTI_EnableEvent_0_31(Line);
		break;
	case LL_EXTI_MODE_IT_EVENT:
		LL_EXTI_EnableIT_0_31(Line);
		LL_EXTI_EnableEvent_0_31(Line);
		break;
	default:
		break;
	}
}

void EXTIR::ExtiTrigger(uint8_t Trigger,uint32_t Line)
{
	switch(Trigger)
	{
	case LL_EXTI_TRIGGER_RISING:
		LL_EXTI_DisableFallingTrig_0_31(Line);
		LL_EXTI_EnableRisingTrig_0_31(Line);
		break;
	case LL_EXTI_TRIGGER_FALLING:
		LL_EXTI_DisableRisingTrig_0_31(Line);
		LL_EXTI_EnableFallingTrig_0_31(Line);
		break;
	case LL_EXTI_TRIGGER_RISING_FALLING:
		LL_EXTI_EnableRisingTrig_0_31(Line);
		LL_EXTI_EnableFallingTrig_0_31(Line);
		break;
	default:
		break;
	}
}

/*** Public ***/
uint32_t EXTIR::Config(void)
{
	GPIO_Port Port;
	uint32_t ErrCode = 0;

	Port = CheckPort();

	ErrCode = (uint32_t)Port;
	SetSource(Port);

	return ErrCode;
}

void EXTIR::ConfigMode_Trigger(uint8_t Mode,uint8_t Trigger)
{
	uint32_t ExtiLine = 1 << PinPos;

	ExtiMode(Mode, ExtiLine);
	ExtiTrigger(Trigger, ExtiLine);
}

#if 0
//NVICの設定は分離する方針(2025-09-30)
inline IRQn_Type EXTIR::CheckExtiLine(void)
{
	IRQn_Type ret = (IRQn_Type)UINT8_MAX;

	if(PinPos <= Pin1)
	{
		 ret = EXTI0_1_IRQn;
	}
	else if(PinPos <= Pin3)
	{
		 ret = EXTI2_3_IRQn;
	}
	else if(PinPos <= Pin15)
	{
		 ret = EXTI4_15_IRQn;
	}
	else
	{
		;
	}
	return ret;
}
#endif
