/*
 * tim.cpp
 *
 *  Created on: May 25, 2025
 *      Author: wataoxp
 */
#include "tim.h"

TIM::TIM(TIM_TypeDef *TIMPORT) :TIMx(TIMPORT)
{
	;
}

uint32_t TIM::CheckTimerPeriph(void)
{
	uint32_t TimNumber = (uint32_t)TIMx;
	uint32_t ret = 0;

	switch(TimNumber)
	{
	case TIM2_BASE:
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
		break;
	case TIM3_BASE:
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
		break;
	case TIM1_BASE:
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
		break;
	case TIM14_BASE:
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
		break;
	case TIM16_BASE:
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
		break;
	case TIM17_BASE:
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
		break;
	default:
		ret = 1;
	}
	return ret;
}

//uint32_t TIM::TimerConfig(uint32_t Presclaer,uint32_t Reload)
//{
//	if(CheckTimerPeriph() != 0)
//	{
//		return 1;
//	}
//
//	if(IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx))
//	{
//		LL_TIM_SetClockDivision(TIMx, LL_TIM_CLOCKDIVISION_DIV1);
//	}
//
//	LL_TIM_SetClockSource(TIMx, LL_TIM_CLOCKSOURCE_INTERNAL);
//	LL_TIM_SetPrescaler(TIMx, (Presclaer-1));
//	LL_TIM_SetAutoReload(TIMx, (Reload-1));
//
//	if(IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx))
//	{
//		LL_TIM_SetCounterMode(TIMx, LL_TIM_COUNTERMODE_UP);
//	}
//
//	LL_TIM_GenerateEvent_UPDATE(TIMx);			//更新イベントを発生させておく
//
//	LL_TIM_ClearFlag_UPDATE(TIMx);				//更新イベントフラグをクリア
//	LL_TIM_EnableARRPreload(TIMx);
//
//	CLEAR_REG(TIMx->CCER);
//	CLEAR_REG(TIMx->CR2);
//	CLEAR_REG(TIMx->CCMR1);
//	CLEAR_REG(TIMx->CCMR2);
//
//	return 0;
//}

uint32_t TIM::TimerConfig(TIM_InitTypedef *Config)
{
	if(CheckTimerPeriph() != 0)
	{
		return 1;
	}

	if(IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx))
	{
		LL_TIM_SetClockDivision(TIMx, Config->Division);
	}

	LL_TIM_SetClockSource(TIMx, Config->Source);
	LL_TIM_SetPrescaler(TIMx, Config->Presclaer);
	LL_TIM_SetAutoReload(TIMx, Config->Reload);

	if(IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx))
	{
		LL_TIM_SetCounterMode(TIMx, Config->CountMode);
	}

	LL_TIM_GenerateEvent_UPDATE(TIMx);			//更新イベントを発生させて分周等を更新

	LL_TIM_ClearFlag_UPDATE(TIMx);				//更新イベントフラグをクリア
	LL_TIM_EnableARRPreload(TIMx);

	CLEAR_REG(TIMx->CCER);
	CLEAR_REG(TIMx->CR2);
	CLEAR_REG(TIMx->CCMR1);
	CLEAR_REG(TIMx->CCMR2);

	return 0;
}

void TIM::PWMConfig(uint32_t Channel,uint32_t mode)
{
	uint32_t tmpccEr = 0;
	uint32_t tmpccMr = 0;
	uint32_t *CCMRx_Reg;

	tmpccEr |= Channel;				//CCER,CCxE
//	tmpccEr |= Channel << 1;		//CCER,CCxP,ActiveLow

	//CCMRx,OCxM。LL_TIM_OCMODE_PWM1がすでに4ビット左シフトされた値なのでビットシフトはその分調節
	switch(Channel)
	{
	case LL_TIM_CHANNEL_CH1:
		tmpccMr = mode | TIM_CCMR1_OC1PE;
		CCMRx_Reg = (uint32_t*)&TIMx->CCMR1;
		break;
	case LL_TIM_CHANNEL_CH2:
		tmpccMr = mode << (TIM_CCMR1_OC2M_Pos - 4) | TIM_CCMR1_OC2PE;
		CCMRx_Reg = (uint32_t*)&TIMx->CCMR1;
		break;
	case LL_TIM_CHANNEL_CH3:		//CH3、4の挙動が怪しいかも
		tmpccMr = mode | TIM_CCMR2_OC3PE;
		CCMRx_Reg = (uint32_t*)&TIMx->CCMR2;
		break;
	case LL_TIM_CHANNEL_CH4:
		tmpccMr = mode << (TIM_CCMR2_OC4M_Pos - 4) | TIM_CCMR2_OC4PE;
		CCMRx_Reg = (uint32_t*)&TIMx->CCMR2;
		break;
	default:
		CCMRx_Reg = nullptr;
		break;
	}

	if(CCMRx_Reg != nullptr)
	{
		TIMx->CCER |= tmpccEr;
		*CCMRx_Reg |= tmpccMr;
	}
}

uint32_t* TIM::ConfigDMA(uint32_t Channel)
{
	uint32_t *CCRxReg = nullptr;

	LL_TIM_CC_SetDMAReqTrigger(TIMx, LL_DMAMUX_REQ_TIM2_UP);	//カウンタ更新時にDMAリクエスト、0であるときCCｘイベントでリクエスト送信
	LL_TIM_EnableDMAReq_UPDATE(TIMx);							//カウンタ更新時DMA要求を許可

	switch(Channel)
	{
	case LL_TIM_CHANNEL_CH1:
		CCRxReg = (uint32_t*)&TIMx->CCR1;
		break;
	case LL_TIM_CHANNEL_CH2:
		CCRxReg = (uint32_t*)&TIMx->CCR2;
		break;
	case LL_TIM_CHANNEL_CH3:
		CCRxReg= (uint32_t*)&TIMx->CCR3;
		break;
	case LL_TIM_CHANNEL_CH4:
		CCRxReg = (uint32_t*)&TIMx->CCR4;
		break;
	default:
		break;
	}
	return CCRxReg;
}

/****** GPIO設定も担当していた頃のやつ *****/
/***
 * オルタネートは各機能毎にある程度統一されている。
 * ハードウェア側データシート(G031ではP40）を参照してほしいが、タイマーの場合はAF1またはAF2になる。
 * なおI2CがすべてAF6である。
 ***/
#if 0
inline void TIM::TIM2outputPin(uint32_t Channel)
{
	GPIO_TypeDef *GPIOx = GPIOA;
	uint32_t PinPos;
	uint32_t Alternate = LL_GPIO_AF_2;

	switch(Channel)
	{
	case LL_TIM_CHANNEL_CH1:
		PinPos = Pin0;
		break;
	case LL_TIM_CHANNEL_CH2:
		PinPos = Pin1;
		break;
	case LL_TIM_CHANNEL_CH3:
		PinPos = Pin2;
		break;
	case LL_TIM_CHANNEL_CH4:
		PinPos = Pin3;
		break;
	}
//	PulsePinConfig(GPIOx, PinPos, Alternate);
}

void TIM::PulsePinConfig(GPIO_TypeDef *GPIOx,uint32_t PinPos,uint32_t Alternate)
{
	GPIO Pulse(GPIOx,PinPos);

	Pulse.Begin();
	Pulse.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
	Pulse.AlternateInit(Alternate);
}
#endif

