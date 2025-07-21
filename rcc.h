/*
 * rcc.h
 *
 *  Created on: Jan 22, 2025
 *      Author: wataoxp
 */

#ifndef INC_RCC_H_
#define INC_RCC_H_

#include "periph.h"

typedef struct{
	uint32_t Latency;		//FLASH->ACR,FlashAccessWait
	uint32_t PLLSrc;
	uint32_t PLLM;			//RCC->PLLCFGR,PLL,InputClockRatio
	uint32_t PLLN;			//RCC->PLLCFGR,ClockMultiplication
	uint32_t PLLR;			//RCC->PLLCFGR,PLL,OutputClockRatio
	uint32_t HSIdiv;
	uint32_t AHBdiv;
	uint32_t APBdiv;
	uint32_t SysClkSrc;
	uint32_t clock;
}RCC_InitTypedef;

void RCC_InitG0(RCC_InitTypedef *rcc);
void RCC_InitC0(RCC_InitTypedef *rcc);
uint32_t SysTick_Init(uint32_t Ticks);

/*
 * RCC_APBENR1レジスタについて
 * 同レジスタにあるDBGENビットは基本セットされている。
 * IDEのデバッグの構成->低電力モードのデバッグ(雑な和訳)が有効になっているので、これを無効にするとDBGENビットはクリアされたままになる
 */


#endif /* INC_RCC_H_ */
