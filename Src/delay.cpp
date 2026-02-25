/*
 * delay.c
 *
 *  Created on: Jan 6, 2025
 *      Author: wataoxp
 */
#include "delay.h"

CoreClock tDelay::ClockSource = HSICLOCK;
TIM* TimPolicy::pTim = nullptr;

uint32_t tDelay::Init(CoreClock source)
{
	if(!source)
	{
		return 1;
	}
	ClockSource = source;

	InitMillTick();
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

	return 0;
}

void tDelay::Delay(uint32_t nTime)
{
	__IO uint32_t tmp = SysTick->CTRL;
	__IO uint32_t mDelay = nTime;
	((void)tmp);

	while(mDelay)
	{
		if (LL_SYSTICK_IsActiveCounterFlag() != 0)
		{
			mDelay--;
		}
	}
}


