/*
 * delay.c
 *
 *  Created on: Jan 6, 2025
 *      Author: wataoxp
 */
#include "delay.h"

void Delay(uint32_t nTime)
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

void MicroDelay(uint32_t nTime)
{
	__IO uint32_t tmp = SysTick->CTRL;
	__IO uint32_t mDelay = nTime;
	((void)tmp);

	SysTick->LOAD  = (uint32_t)(MICRO_LOAD - 1UL);
	SysTick->VAL = 0;

	while(mDelay)
	{
		if (LL_SYSTICK_IsActiveCounterFlag() != 0U)
		{
			mDelay--;
		}
	}

	SysTick->LOAD  = (uint32_t)(MILL_LOAD - 1UL);
}
