/*
 * debounce.cpp
 *
 *  Created on: Mar 13, 2026
 *      Author: wataoxp
 */

#include "debounce.h"
#include <cassert>

//#define VerticalCount4Bit

using namespace DebouceUnits;

VerticalCounter Debounce::count = {};
uint32_t Debounce::PinMask = 0;
GPIO_TypeDef* Debounce::GPIOx = nullptr;
uint32_t Debounce::State = 0;

void Debounce::FilterSetUp(GPIO_TypeDef* pGPIO,uint32_t mask)
{
	GPIOx = pGPIO;
	PinMask = mask;
}

#ifdef VerticalCount4Bit
void Debounce::ButtonFilter(uint32_t& command)
{
	static uint32_t state = 0,cnt0 = 0,cnt1 = 0;
	uint32_t delta,toggle;

	uint32_t sample = ~(LL_GPIO_ReadInputPort(GPIOx)) & PinMask;

	delta = sample ^ state;
	cnt1 = (cnt1 ^ cnt0) & delta;
	cnt0 = ~cnt0 & delta;

	toggle = delta & ~(cnt0 | cnt1);
	state ^= toggle;

	command |= toggle & state;
	// 解放時を取得すると長押しの判定も可能に
//	uint32_t Released = toggle & ~state;
}
#else
void Debounce::ButtonFilter(uint32_t& command)
{
	static uint32_t state = 0,cnt0 = 0,cnt1 = 0;
	uint32_t delta,toggle;

	uint32_t sample = ~(LL_GPIO_ReadInputPort(GPIOx)) & PinMask;

	delta = sample ^ state;
	cnt1 = (cnt1 ^ cnt0) & delta;
	cnt0 = ~cnt0 & delta;
	toggle = cnt0 & cnt1;
	state ^= toggle;

	command |= state & toggle;
}
#endif

//void Debounce::ButtonFilter(__IO uint32_t& command)
//{
//	assert(PinMask != 0);
//	assert(GPIOx != nullptr);
//
//	uint32_t RawData = ~(LL_GPIO_ReadInputPort(GPIOx)) & PinMask;
//	// 半加算器ロジック
//	uint32_t Carry = count.LowCnt & RawData;	// ケタ上がりを検出
//	count.LowCnt ^= RawData;					// Sum
//	count.MiddleCnt ^= Carry;					// CarryとのSumを計算
//
//	count.LowCnt &= RawData;					// 入力が0になったビットをリセット
//	count.MiddleCnt &= RawData;
//
//	uint32_t Verify = count.LowCnt & count.MiddleCnt;	// 入力ピンの位置に0/1が立っているかチェック
//	uint32_t Edge = Verify & ~State;			// Verifyが1、Stateが0であれば有効なEdgeとする
//	State |= Edge;								// Stateを更新
//	State &= RawData;
//
//	if(Edge)
//	{
//		command |= Edge;
//	}
//}



