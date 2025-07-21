/*
 * exti.c
 *
 *  Created on: Jan 28, 2025
 *      Author: wataoxp
 */
#include "exti.h"

EXTIR::EXTIR(Exti_ConfigTypedef *InitStruct) : Exti(InitStruct)
{
	;
}
void EXTIR::Init(void)
{
	//Exti->Port = ((uint32_t) (Exti->GPIOx) - IOPORT_BASE) / IOPORT_SPACE;
	Exti->Port = CheckPort(Exti->GPIOx);

	StructInit(Exti->PinPos);
	SetSource(Exti->Port, Exti->PinPos);
	ExtiMode(Exti->Mode, Exti->ExtiLine);
	ExtiTrigger(Exti->Trigger, Exti->ExtiLine);
}
void EXTIR::StructInit(uint32_t pin)
{
	Exti->IRQx = CheckExtiLine(pin);
	Exti->ExtiLine = 1 << pin;

	__NVIC_SetPriority(Exti->IRQx, Exti->prime);
	__NVIC_EnableIRQ(Exti->IRQx);
}
void EXTIR::SetSource(uint8_t port,uint32_t pin)
{
	uint32_t Reg = pin / 4;
	uint32_t Bit = (pin % 4) * 8;

	MODIFY_REG(EXTI->EXTICR[Reg],EXTICR_Msk << Bit, port << Bit);
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
inline IRQn_Type EXTIR::CheckExtiLine(uint32_t pin)
{

	if(pin <= Pin1)
	{
		 return EXTI0_1_IRQn;
	}
	else if(pin <= Pin3)
	{
		 return EXTI2_3_IRQn;
	}
	else
	{
		 return EXTI4_15_IRQn;
	}
}
inline GPIO_Port EXTIR::CheckPort(GPIO_TypeDef *GPIOx)
{
	GPIO_Port ret = NoPort;
//	uint32_t PORT = (uint32_t)(GPIOx);

//	switch(PORT)
//	{
//	case GPIOA_BASE:
//		ret = PORTA;
//		break;
//	}


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
