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

// nopループを使う方
#if 0
uint32_t tDelay::SetClockSource(CoreClock source)
{
	if(!source)
	{
		return 1;
	}
	ClockSource = source;

	return 0;
}

/* 64MHzの場合、1秒間に64Mサイクル(1us辺り64サイクル)
 * 1ループを4サイクル(命令)と仮定して、1us辺り16サイクル*ループ数とする
 * 64MHz、-Ofast最適化で10usを設定すると20usになった。遅い分には問題ないと見ている
*/
void tDelay::uDelay(uint32_t nTime)
{
	__IO uint32_t loop = nTime * (ClockSource / 4);

	while(loop--)
	{
		__NOP();
	}
}
#endif


