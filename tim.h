/*
 * tim.h
 *
 *  Created on: May 25, 2025
 *      Author: wataoxp
 */

#ifndef TIM_H_
#define TIM_H_

#include "periph.h"

namespace TimerCodes{
	typedef enum{
		success,
		failed,
		NotTimer,
		NotChannel
	}RetCode;
}

typedef struct{
	uint32_t Presclaer;
	uint32_t Reload;

	/* TIM1~3,14,16,17 */
	uint32_t Division;
	uint32_t Source;

	/* TIM1~3 */
	uint32_t CountMode;
}TIM_InitTypedef;

class TIM{
private:
	TIM_TypeDef *TIMx;
	uint32_t TimPort;
	uint32_t CheckTimerPeriph(void);
	void CheckPulsePin(uint32_t Channel);
public:
	TIM(TIM_TypeDef *TIMPORT);
	uint32_t ConfigTimer(TIM_InitTypedef *Config);
	uint32_t ConfigPWM(uint32_t Channel,uint32_t mode);
	uint32_t ConfigDMA(uint32_t Channel,uint32_t ReqSource);

	uint32_t ConfigInput(uint32_t Channel,uint32_t Polarity);
	void ConfigCombinedCh(uint32_t ch1Pol,uint32_t ch2Pol);

	uint32_t* GetCCxRegAddr(uint32_t Channel);

	inline void UpdateTimer(void)
	{
		LL_TIM_GenerateEvent_UPDATE(TIMx);			//更新イベントを発生させて分周等を更新
		while(LL_TIM_IsActiveFlag_UPDATE(TIMx) == 0);
		LL_TIM_ClearFlag_UPDATE(TIMx);				//更新イベントフラグをクリア
	}

	inline void EnableTimer(void)
	{
		LL_TIM_EnableCounter(TIMx);
	}
	inline void DisableTimer(void)
	{
		LL_TIM_DisableCounter(TIMx);
	}

	inline void SetAutoReload(uint32_t Reload)
	{
		LL_TIM_SetAutoReload(TIMx, Reload);
	}
	inline void SetCH1CompareValue(uint32_t value)
	{
		LL_TIM_OC_SetCompareCH1(TIMx, value);
	}
	inline void SetCH2CompareValue(uint32_t value)
	{
		LL_TIM_OC_SetCompareCH2(TIMx, value);
	}
	inline void SetCH3CompareValue(uint32_t value)
	{
		LL_TIM_OC_SetCompareCH3(TIMx, value);
	}
	inline void SetCH4CompareValue(uint32_t value)
	{
		LL_TIM_OC_SetCompareCH4(TIMx, value);
	}

	inline void EnablePulse(uint32_t Channel)
	{
		LL_TIM_CC_EnableChannel(TIMx, Channel);
	}
	inline void DisablePulse(uint32_t Channel)
	{
		LL_TIM_CC_DisableChannel(TIMx, Channel);
	}

};

#if 0
inline void TIM2outputPin(uint32_t channel);
void PulsePinConfig(GPIO_TypeDef *GPIOx,uint32_t PinPos,uint32_t Alternate);
#endif



#endif /* TIM_H_ */
