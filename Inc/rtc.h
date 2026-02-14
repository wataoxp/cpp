/*
 * rtc.h
 *
 *  Created on: Jan 21, 2026
 *      Author: wataoxp
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "periph.h"

namespace TimeParameter{
	typedef enum{
		Success,
		Initialized,
		Error_Initialization,
		Error_NotSynchro,
		Clock_NotSet,
		Alarm_NotModule,
	}ConfigStatus;

	constexpr uint32_t SynchroTime = 1000;
	constexpr uint32_t AsynchDefault = 0x7F;
	constexpr uint32_t SynchDefault = 0xFF;

	enum class AlarmValue :uint32_t{
		WeekDay_Disable,
		WeekDay_Enable,
	};

	enum class Options :uint8_t{
		Alarm_Enable,
		Alarm_Disable,
		ALMA,
		ALMB,
		ALMA_ALMB,
		NoneAlarm,
		WakeUp_Enable,
		WakeUp_Disable,
	};

	typedef struct{
		AlarmValue WeekDaySel;
		uint8_t Day;

		uint32_t Mask;

		uint8_t Hours;
		uint8_t Minutes;
		uint8_t Seconds;
	}AlarmStatus;

	typedef struct{
		Options Alarm;
		Options SelectAlarm;
		Options WakeUp;

		uint8_t Hours;
		uint8_t Minutes;
		uint8_t Seconds;
		uint8_t WeekDay;
		uint8_t Month;
		uint8_t Day;
		uint8_t Year;

		AlarmStatus ALMA;
		AlarmStatus ALMB;

	}ConfigParameters;
}


class TimeClock{
private:
	RTC_TypeDef *RTCx;
	uint32_t CheckInitMode(void);
	void CheckDBP(void);
	uint32_t WaitForSynchro(void);
	void SetALMA(TimeParameter::AlarmStatus *alma);
	void SetALMB(TimeParameter::AlarmStatus *almb);
public:
	TimeClock(RTC_TypeDef *rtc);
	void ClockConfig(void);
	uint32_t Config(uint32_t HourFormat,uint32_t AsynchPrescaler,uint32_t SynchPrescaler);
	uint32_t SetTime(uint32_t Format,uint32_t Hours,uint32_t Minutes,uint32_t Seconds);
	uint32_t SetDate(uint32_t WeekDay,uint32_t Month,uint32_t Day,uint32_t Year);
	uint32_t SetAlarm(TimeParameter::ConfigParameters *init);
	uint32_t EnableAlarm(TimeParameter::Options AlarmSelect);
	void SetWakeUpTimer(uint32_t Count);
};




#endif /* INC_RTC_H_ */
