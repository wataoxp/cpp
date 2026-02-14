/*
 * adc.h
 *
 *  Created on: Jun 3, 2025
 *      Author: wataoxp
 */

#ifndef ADC_H_
#define ADC_H_

#include "periph.h"

typedef void (*uDelay)(uint32_t);

typedef struct{
	/* CFGR1 */
	uint32_t Resolution; 		//RES
	uint32_t DataAlignment;		//ALGIN

	uint32_t TriggerSource;		//EXTSEL
	uint32_t TriggerEdge;		//EXTEN
	uint32_t OverRun;			//OVRMOD

	uint32_t Configurability;	//CHSELRMOD

	/* CFGR2 */
	uint32_t Clock;				//CKMODE

	/* SMPR */
	uint32_t SamplingTime;

	/* CHSELR */
	uint32_t SequencerLength;
}ADC_ConfigTypedef;

class AnalogConverter{
private:
	ADC_TypeDef *ADCx;
public:
	AnalogConverter(ADC_TypeDef *ADCPort);

	uint32_t Config(ADC_ConfigTypedef* Config,uDelay delay);
	uint32_t RegConfig(ADC_ConfigTypedef *RegConfig,uint32_t Channel);
	uint32_t SelectChannel(ADC_ConfigTypedef *Config,uint32_t Channel);
	void SetISR(void);

	uint16_t StartSoftConvert(void);
	inline void StartHwTrigger(void)
	{
		LL_ADC_REG_StartConversion(ADCx);
	}

	inline void EnableADC(void)
	{
		LL_ADC_Enable(ADCx);
		while(LL_ADC_IsActiveFlag_ADRDY(ADCx) == 0);
	}

	void DisableADC(void);
};




#endif /* ADC_H_ */
