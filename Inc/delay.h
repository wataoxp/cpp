/*
 * delay.h
 *
 *  Created on: Jan 6, 2025
 *      Author: wataoxp
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "periph.h"
#include "tim.h"

// 1MHz(1000*1000)だとして、1000を設定すると1kHz。1000*1000なら1MHzでCOUNTFLAGがセットされる
namespace SysTickTiming{
	constexpr uint32_t MillSecond = 1000;
	constexpr uint32_t MicroSecond = 1000*1000;
}

class tDelay{
private:
	static CoreClock ClockSource;
public:
	static uint32_t SetClockSource(CoreClock source);
	static uint32_t Init(CoreClock source);
	static void Delay(uint32_t nTime);
	static void uDelay(uint32_t nTime);

	static void InitMillTick(void);
	static void InitMicroTick(void);

	static void EnableISR(void);
	static void DisableISR(void);
};

// SysTickではTIMインスタンスは無視
class SysTickPolicy{
public:
	static uint32_t Init(CoreClock source,TIM* tim = nullptr)
	{
		return tDelay::Init(source);
	}

	static void mDelay(uint32_t nTime)
	{
		tDelay::Delay(nTime);
	}

	static void uDelay(uint32_t nTime)
	{
		tDelay::InitMicroTick();
		tDelay::Delay(nTime);
		tDelay::InitMillTick();
	}
};

class TimPolicy{
private:
	// staticメンバは参照として持てないのでポインタ
	static TIM* pTim;
public:
	static uint32_t Init(CoreClock source,TIM* obj)
	{
		if(obj == nullptr) return 1;
		pTim = obj;

		TimerPeripheral::TIM_InitTypedef Config = {0};

		// 1us用の設定
		Config.CountMode = LL_TIM_CLOCKDIVISION_DIV1;
		Config.Source = LL_TIM_CLOCKSOURCE_INTERNAL;
		Config.Prescale = source-1;
		Config.Reload = UINT16_MAX-1;
		Config.CountMode = LL_TIM_COUNTERDIRECTION_UP;
		Config.Division = LL_TIM_CLOCKDIVISION_DIV1;

		if(pTim->ConfigTimer(&Config) != TimerPeripheral::success) return 1;

		pTim->EnableTimer();
		return 0;
	}

	// 16ビットを超えないように1ms待機*nTime
	static void mDelay(uint32_t nTime)
	{
		for(uint32_t i = 0;i < nTime;i++)
		{
			pTim->Delay(1000);
		}
	}

	static void uDelay(uint32_t nTime)
	{
		pTim->Delay(nTime);
	}
};

namespace DelayMode{
	using Standard = SysTickPolicy;
	using RtosMode = TimPolicy;
}

inline void tDelay::InitMillTick(void)
{
	uint32_t load = ClockSource * SysTickTiming::MillSecond;

	SysTick->LOAD = load-1;
	SysTick->VAL = 0;
}


inline void tDelay::InitMicroTick(void)
{
	uint32_t load = ClockSource;

	SysTick->LOAD = load-1;
	SysTick->VAL = 0;
}

inline void tDelay::EnableISR(void)
{
	LL_SYSTICK_EnableIT();
}

inline void tDelay::DisableISR(void)
{
	LL_SYSTICK_DisableIT();
}

// Policyが指定したメンバを使う形に置き換わる
template <typename Policy>
class xDelay{
public:
	static uint32_t Init(CoreClock source,TIM* tim)
	{
		return Policy::Init(source,tim);
	}

	static void mDelay(uint32_t nTime)
	{
		Policy::mDelay(nTime);
	}

	static void uDelay(uint32_t nTime)
	{
		Policy::uDelay(nTime);
	}
};

#if __has_include("FreeRTOSConfig.h")
#include "task.h"
#define USE_RTOS_DELAY
#endif

#ifdef USE_RTOS_DELAY
class RtosPolicy{
public:
	static uint32_t Init(CoreClock source)
	{
		return tDelay::SetClockSource(source);
	}

	static void mDelay(uint32_t nTime)
	{
		vTaskDelay((const TickType_t)nTime);
	}
};
#endif

#endif /* INC_DELAY_H_ */
