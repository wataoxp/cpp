/*
 * adc.cpp
 *
 *  Created on: Jun 3, 2025
 *      Author: wataoxp
 */

#include "adc.h"

AnalogConverter::AnalogConverter(ADC_TypeDef *ADCPort) :ADCx(ADCPort)
{
	;
}

uint32_t AnalogConverter::SelectChannel(ADC_ConfigTypedef *Config,uint32_t Channel)
{
	LL_ADC_REG_SetSequencerChAdd(ADCx, Channel);	//シングル変換、またはSCANDIRの順番

	if(Config->Configurability != LL_ADC_REG_SEQ_FIXED)
	{
		if(Channel > LL_ADC_CHANNEL_14)		//CHSELRMOD=1の最大値
		{
			return 1;
		}
		LL_ADC_REG_SetSequencerConfigurable(ADCx,LL_ADC_REG_SEQ_CONFIGURABLE);
		LL_ADC_REG_SetSequencerLength(ADCx,Config->SequencerLength);
	}

	while(LL_ADC_IsActiveFlag_CCRDY(ADCx) == 0);		//CHSELRMODおよびCHSELRへのアクセス後はCCRDYを待つ
	LL_ADC_ClearFlag_CCRDY(ADCx);

	return 0;
}

uint32_t AnalogConverter::Config(ADC_ConfigTypedef* Config,uDelay delay)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

	if(LL_ADC_IsEnabled(ADCx) != 0)
	{
		return 1;
	}
	CLEAR_REG(ADCx->CR);
	CLEAR_REG(ADCx->CFGR1);
	CLEAR_REG(ADCx->CFGR2);

	LL_ADC_EnableInternalRegulator(ADCx);
	delay(200);							//レギュレータの安定化待ち。データシート上では20us。MX_ADC‗Initではだいたいこのくらい

	LL_ADC_SetResolution(ADCx, Config->Resolution);			//分解能
	LL_ADC_SetDataAlignment(ADCx, Config->DataAlignment);	//DR、右または左寄せ
	LL_ADC_SetClock(ADCx, Config->Clock);					//ADCクロック。最大で35MHz

	return 0;
}

uint32_t AnalogConverter::RegConfig(ADC_ConfigTypedef *RegConfig,uint32_t Channel)
{
	if(LL_ADC_IsEnabled(ADCx) != 0)
	{
		return 1;
	}

	LL_ADC_REG_SetTriggerSource(ADCx, RegConfig->TriggerSource);			//変換トリガソース
	LL_ADC_REG_SetTriggerEdge(ADCx,RegConfig->TriggerEdge);					//変換トリガ方向
	LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_1, RegConfig->SamplingTime);		//サンプリング時間
	LL_ADC_REG_SetOverrun(ADCx,RegConfig->OverRun);							//オーバーラン時にDRの値を保持or上書き

	return 0;
}

void AnalogConverter::SetISR(void)
{
	LL_ADC_EnableIT_EOC(ADCx);
}

uint16_t AnalogConverter::StartSoftConvert(void)
{
	if(LL_ADC_IsEnabled(ADCx) == 0)
	{
		this->EnableADC();
	}

	LL_ADC_REG_StartConversion(ADCx);
	while(LL_ADC_IsActiveFlag_EOC(ADCx) == 0);
	LL_ADC_ClearFlag_EOC(ADCx);

	return LL_ADC_REG_ReadConversionData8(ADCx);
}

void AnalogConverter::DisableADC(void)
{
	while(LL_ADC_IsDisableOngoing(ADCx) != 0);		//ADDISビットが1であるなら待つ
	LL_ADC_REG_StopConversion(ADCx);

	while(LL_ADC_REG_IsStopConversionOngoing(ADCx) != 0);	//ADSTPビットが0になるまで待つ
	LL_ADC_Disable(ADCx);
}


#if 0
//TRGOがうまくいかなかったやつ
uint32_t AnalogConverter::Config(ADC_ConfigTypedef *Config)
{
	uint32_t tmpCFGR1 = 0;
	uint32_t tmpCFGR2 = 0;

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

	tmpCFGR1 |= Config->CFGR1s->Resolution;
	tmpCFGR1 |= Config->CFGR1s->CHSELRMOD;
	tmpCFGR1 |= Config->CFGR1s->EXTEN;
	tmpCFGR1 |= Config->CFGR1s->EXTSEL;

	tmpCFGR2 |= Config->CFGR2s->ADclock;

	if(LL_ADC_REG_IsConversionOngoing(ADCx) != 0)
	{
		LL_ADC_REG_StopConversion(ADCx);
	}

	WRITE_REG(ADCx->CFGR1,tmpCFGR1);
	WRITE_REG(ADCx->CFGR2,tmpCFGR2);

	LL_ADC_REG_SetSequencerDiscont(ADCx, LL_ADC_REG_SEQ_DISCONT_DISABLE);		//不連続モード無効
	LL_ADC_REG_SetContinuousMode(ADCx, LL_ADC_REG_CONV_SINGLE);					//シングル変換モード

	LL_ADC_SetSamplingTimeCommonChannels(ADCx, LL_ADC_SAMPLINGTIME_COMMON_1,LL_ADC_SAMPLINGTIME_39CYCLES_5);
	LL_ADC_SetChannelSamplingTime(ADCx, Config->ADC_Channel, LL_ADC_SAMPLINGTIME_COMMON_1);

	LL_ADC_REG_SetOverrun(ADCx, LL_ADC_REG_OVR_DATA_OVERWRITTEN);				//OVR時上書き

//	while(LL_ADC_IsActiveFlag_CCRDY(ADCx) == 0);			//CHSELRMODをセットした場合はCCRDYを待つ
//	LL_ADC_ClearFlag_CCRDY(ADCx);

	SelectChannel(Config->ADC_Channel);
	while(LL_ADC_IsActiveFlag_CCRDY(ADCx) == 0);			//CHSELRレジスタへのアクセス後もCCRDYをチェック
	LL_ADC_ClearFlag_CCRDY(ADCx);

	return ConfigCR(Config);
}
inline void AnalogConverter::SelectChannel(uint32_t Channel)
{
	uint32_t value = Channel & ADC_CHSELR_CHSEL_Msk;	//シングルチャネル、またはSCANDIRの順番で変換
 	SET_BIT(ADCx->CHSELR,value);



//CHSELRMOD = 1かつチャネルが8未満であるとき
//	SET_BIT(ADCx->CHSELR,ADC_CHSELR_SQ2);				//任意の順番で変換可能。SQxで変換完了とするとき、SQ(x+1)にFFを書き込む
}
#endif
