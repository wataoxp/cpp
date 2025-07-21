/*
 * delay.h
 *
 *  Created on: Jan 6, 2025
 *      Author: wataoxp
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "periph.h"

#define SYSTEM_CLOCK 48000000		//HCLK
#define MICRO_SECOND 1000000		//1s = 1000000us
#define MILL_SECOND 1000			//1s = 1000ms
#define MICRO_LOAD (SYSTEM_CLOCK/MICRO_SECOND)	// HCLK/MICRO_SECOND = 1usでカウントされる値
#define MILL_LOAD (SYSTEM_CLOCK/MILL_SECOND)	// HCLK/MILL_SECOND = 1msでカウントされる値

#define SYSINT_ENABLE SysTick_CTRL_TICKINT_Msk
#define SYSINT_DISABLE 0

static inline void Init1usTick(void)
{
	SysTick->LOAD  = (uint32_t)(MICRO_LOAD - 1UL);
	SysTick->VAL = 114514;
}
static inline void DeInit1usTick(void)
{
	SysTick->LOAD  = (uint32_t)(MILL_LOAD - 1UL);
}

static inline void SysTick_ConfigISR(uint8_t valid)
{
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= valid;
}



//デフォルトはSystemClock_Config()のLL_Init1msTick()が適用されるので、1msカウントになっている。
void Delay(uint32_t nTime);
void MicroDelay(uint32_t nTime);

#endif /* INC_DELAY_H_ */
