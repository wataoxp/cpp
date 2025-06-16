/*
 * tim.h
 *
 *  Created on: May 25, 2025
 *      Author: wataoxp
 */

#ifndef TIM_H_
#define TIM_H_

#include "periph.h"

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
	uint32_t TimerConfig(TIM_InitTypedef *Config);
	void PWMConfig(uint32_t Channel,uint32_t mode);
	uint32_t* ConfigDMA(uint32_t Channel);

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

	inline void EnableTimer(void)
	{
		LL_TIM_EnableCounter(TIMx);
	}
	inline void DisableTimer(void)
	{
		LL_TIM_DisableCounter(TIMx);
	}
	/*** PWMConfigでセット済なので基本は必要ない ***/
	inline void EnablePulse(uint32_t Channel)
	{
		LL_TIM_CC_EnableChannel(TIMx, Channel);
	}
	inline void DisablePulse(uint32_t Channel)
	{
		LL_TIM_CC_DisableChannel(TIMx, Channel);
	}

	inline void CheckPulse(uint32_t Channel)
	{
		if(LL_TIM_IsEnabledCounter(TIMx) == 0)
		{
			LL_TIM_EnableCounter(TIMx);
		}
		if(LL_TIM_CC_IsEnabledChannel(TIMx, Channel) == 0)
		{
			LL_TIM_CC_EnableChannel(TIMx, Channel);
		}
	}

	inline void TriggerOutput(uint32_t TRGO)		//MOEビットをセットする(一部のタイマー)
	{
		LL_TIM_SetTriggerOutput(TIMx, TRGO);
	}
};

#if 0
inline void TIM2outputPin(uint32_t channel);
void PulsePinConfig(GPIO_TypeDef *GPIOx,uint32_t PinPos,uint32_t Alternate);
#endif



#endif /* TIM_H_ */
