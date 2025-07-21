/*
 * tim.cpp
 *
 *  Created on: May 25, 2025
 *      Author: wataoxp
 */
#include "tim.h"

using namespace TimerCodes;

TIM::TIM(TIM_TypeDef *TIMPORT) :TIMx(TIMPORT)
{
	;
}

uint32_t TIM::CheckTimerPeriph(void)
{
	uint32_t TimNumber = (uint32_t)TIMx;
	uint32_t ret = success;

	switch(TimNumber)
	{
#ifdef STM32G0x1
	case TIM2_BASE:
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
		break;
#endif
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
		ret = NotTimer;
	}
	return ret;
}

uint32_t TIM::ConfigTimer(TIM_InitTypedef *Config)
{
	if(CheckTimerPeriph() != success)
	{
		return NotTimer;
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
	UpdateTimer();
	LL_TIM_EnableARRPreload(TIMx);

	CLEAR_REG(TIMx->CCER);
	CLEAR_REG(TIMx->CR2);
	CLEAR_REG(TIMx->CCMR1);
	CLEAR_REG(TIMx->CCMR2);

	return 0;
}

uint32_t TIM::ConfigPWM(uint32_t Channel,uint32_t mode)
{
	if(Channel > LL_TIM_CHANNEL_CH6)
	{
		return failed;
	}

	LL_TIM_OC_EnablePreload(TIMx, Channel);		//OCxPE、プリロード有効。更新イベント時にCCRxを更新
	LL_TIM_OC_SetMode(TIMx, Channel, mode);		//OCxM、出力モード（PWM等）
	LL_TIM_OC_SetPolarity(TIMx, Channel, LL_TIM_OCPOLARITY_HIGH);	//CCER,CCxP 極性の選択

	if(IS_TIM_BREAK_INSTANCE(TIMx))
	{
		LL_TIM_OC_SetIdleState(TIMx, Channel,LL_TIM_OCIDLESTATE_LOW);	//OISビットがあるTIMの場合
	}

	if(IS_TIM_BREAK_INSTANCE(TIMx))
	{
		LL_TIM_EnableAllOutputs(TIMx);								//BDTR->MOE
	}
	return success;
}
uint32_t TIM::ConfigDMA(uint32_t Channel,uint32_t ReqSource)
{
	uint32_t Ret = success;

	LL_TIM_CC_SetDMAReqTrigger(TIMx, ReqSource);

	if(ReqSource == LL_TIM_CCDMAREQUEST_CC)
	{
		switch(Channel)
		{
		case LL_TIM_CHANNEL_CH1:
			LL_TIM_EnableDMAReq_CC1(TIMx);
			break;
		case LL_TIM_CHANNEL_CH2:
			LL_TIM_EnableDMAReq_CC2(TIMx);
			break;
		case LL_TIM_CHANNEL_CH3:
			LL_TIM_EnableDMAReq_CC3(TIMx);
			break;
		case LL_TIM_CHANNEL_CH4:
			LL_TIM_EnableDMAReq_CC4(TIMx);
			break;
		default:
			Ret = NotChannel;
			break;
		}
	}
	else
	{
		LL_TIM_EnableDMAReq_UPDATE(TIMx);
	}
	return Ret;
}


uint32_t* TIM::GetCCxRegAddr(uint32_t Channel)
{
	uint32_t *CCRxReg = nullptr;								//DMA、ペリフェラルアドレスを返す

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

uint32_t TIM::ConfigInput(uint32_t Channel,uint32_t Polarity)
{
	if(Channel > LL_TIM_CHANNEL_CH6)
	{
		return failed;
	}

	if(LL_TIM_CC_IsEnabledChannel(TIMx, Channel) != 0)
	{
		LL_TIM_CC_DisableChannel(TIMx, Channel);
	}

	LL_TIM_IC_SetActiveInput(TIMx, Channel, LL_TIM_ACTIVEINPUT_DIRECTTI);		//CC1をT1入力に
	LL_TIM_IC_SetPrescaler(TIMx, Channel, LL_TIM_ICPSC_DIV1);					//1エッジでキャプチャ開始
	LL_TIM_IC_SetFilter(TIMx, Channel, LL_TIM_IC_FILTER_FDIV16_N8);				//8回フィルタ
//	LL_TIM_IC_SetFilter(TIMx, Channel, LL_TIM_IC_FILTER_FDIV1);					//フィルター無し
	LL_TIM_IC_SetPolarity(TIMx, Channel, Polarity);								//指定した極性のエッジでキャプチャ

	//スレーブモード
	LL_TIM_SetTriggerInput(TIMx, LL_TIM_TS_TI1FP1);								//TI1FP1をトリガ設定
	LL_TIM_SetSlaveMode(TIMx, LL_TIM_SLAVEMODE_RESET);							//TI1FP1の立ち上がりでCNTリセット

	LL_TIM_CC_EnableChannel(TIMx, Channel);

	return success;
}

void TIM::ConfigCombinedCh(uint32_t ch1Pol,uint32_t ch2Pol)
{
	//PWM入力モードはCH1とCH2のみ(P602)

	LL_TIM_CC_DisableChannel(TIMx, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_DisableChannel(TIMx, LL_TIM_CHANNEL_CH2);

	//Rising Edge CCR
	LL_TIM_IC_SetActiveInput(TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);	//CC1SをTI1に配置
	LL_TIM_IC_SetPrescaler(TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);				//入力プリスケーラ
	LL_TIM_IC_SetFilter(TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1_N8);			//フィルター無し
	LL_TIM_IC_SetPolarity(TIMx, LL_TIM_CHANNEL_CH1, ch1Pol);							//極性の選択

	//Falling Edge CCR
	LL_TIM_IC_SetActiveInput(TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_INDIRECTTI);	//CC2SをTI1に配置
	LL_TIM_IC_SetPrescaler(TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
	LL_TIM_IC_SetFilter(TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1_N8);
	LL_TIM_IC_SetPolarity(TIMx, LL_TIM_CHANNEL_CH2,ch2Pol);

	//スレーブモード
	LL_TIM_SetTriggerInput(TIMx, LL_TIM_TS_TI1FP1);								//TI1FP1をトリガ設定
	LL_TIM_SetSlaveMode(TIMx, LL_TIM_SLAVEMODE_RESET);							//TI1FP1の立ち上がりでCNTリセット

	LL_TIM_CC_EnableChannel(TIMx, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIMx, LL_TIM_CHANNEL_CH2);
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
uint32_t TIM::TimerConfig(uint32_t Presclaer,uint32_t Reload)
{
	if(CheckTimerPeriph() != 0)
	{
		return 1;
	}

	if(IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx))
	{
		LL_TIM_SetClockDivision(TIMx, LL_TIM_CLOCKDIVISION_DIV1);
	}

	LL_TIM_SetClockSource(TIMx, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetPrescaler(TIMx, (Presclaer-1));
	LL_TIM_SetAutoReload(TIMx, (Reload-1));

	if(IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx))
	{
		LL_TIM_SetCounterMode(TIMx, LL_TIM_COUNTERMODE_UP);
	}

	LL_TIM_GenerateEvent_UPDATE(TIMx);			//更新イベントを発生させておく

	LL_TIM_ClearFlag_UPDATE(TIMx);				//更新イベントフラグをクリア
	LL_TIM_EnableARRPreload(TIMx);

	CLEAR_REG(TIMx->CCER);
	CLEAR_REG(TIMx->CR2);
	CLEAR_REG(TIMx->CCMR1);
	CLEAR_REG(TIMx->CCMR2);

	return 0;
}

//特に問題はないけどLLライブラリを積極活用する方針に沿って一時休眠
uint32_t TIM::PWMConfig(uint32_t Channel,uint32_t mode)
{
	uint32_t ret = success;
	uint32_t tmpccEr = 0;
	uint32_t tmpccMr = 0;

	uint32_t *CCMRx_Reg;			//チャネルに応じたCCMRxレジスタのアドレスを格納

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
	case LL_TIM_CHANNEL_CH3:
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
	else
	{
		ret = failed;
	}

	return ret;
}
#endif

