/*
 * debouce.h
 *
 *  Created on: Mar 13, 2026
 *      Author: wataoxp
 */

#ifndef INC_DEBOUNCE_H_
#define INC_DEBOUNCE_H_

#include "periph.h"

/* 割り込みハンドラから呼び出すので静的関数としておく */

namespace DebouceUnits{
	constexpr uint32_t CheckIntervalMask = (16-1);

	typedef struct{
		uint32_t LowCnt;
		uint32_t MiddleCnt;
		uint32_t HighCnt;
	}VerticalCounter;
}

class Debounce{
private:
	static DebouceUnits::VerticalCounter count;
	static uint32_t PinMask;
	static GPIO_TypeDef* GPIOx;
	static uint32_t State;
public:
	static void FilterSetUp(GPIO_TypeDef* pGPIO,uint32_t mask);
	static void ButtonFilter(uint32_t& command);
};



#endif /* INC_DEBOUNCE_H_ */
