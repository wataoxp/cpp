/*
 * rtc.cpp
 *
 *  Created on: Jan 21, 2026
 *      Author: wataoxp
 */

#include "rtc.h"

using namespace TimeParameter;

TimeClock::TimeClock(RTC_TypeDef *rtc) :RTCx(rtc)
{
	;
}
/* Private Function */

// RTCが初期化モードかチェック。あるいは有効化
uint32_t TimeClock::CheckInitMode(void)
{
	uint32_t tmp;
	__IO uint32_t timeout = SynchroTime;

	if(LL_RTC_IsActiveFlag_INIT(RTCx) != 0)
	{
		return Initialized;
	}
	LL_RTC_EnableInitMode(RTCx);		// ICSRレジスタは0書き込みでクリアされるビットを含む

	tmp = LL_RTC_IsActiveFlag_INIT(RTCx);
	while ((timeout != 0) && (tmp != 1))		// INITFビットが1になるまでポーリング
	{
		if (LL_SYSTICK_IsActiveCounterFlag() == 1)
		{
			timeout--;
		}
		tmp = LL_RTC_IsActiveFlag_INIT(RTCx);

		if(timeout == 0) return Error_Initialization;
	}

	return Success;
}

// RTCドメインへのアクセス権のチェック。保護中(DBP=0)なら解除
// DBPによる保護が有効なとき、割り込みフラグのクリアも出来なくなるので注意
void TimeClock::CheckDBP(void)
{
	// PWRドメインへのクロックチェック
	if(LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_PWR) != 1)
	{
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
		// ダミーリード
		LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_PWR);
	}
	// DBPビットをセットしてRTCドメインへの書き込みを有効化
	if(LL_PWR_IsEnabledBkUpAccess() != 1)
	{
		LL_PWR_EnableBkUpAccess();
		while(LL_PWR_IsEnabledBkUpAccess() == 0);
	}
}

// G0ではなくHALやL4ライブラリの仕様に準拠
uint32_t TimeClock::WaitForSynchro(void)
{
	uint32_t tmp;
	__IO uint32_t timeout = SynchroTime;

	LL_RTC_ClearFlag_RS(RTCx);
	tmp = LL_RTC_IsActiveFlag_RS(RTCx);

	while((timeout != 0) && (tmp != 1))
	{
		if (LL_SYSTICK_IsActiveCounterFlag() == 1)
		{
			timeout--;
		}
		tmp = LL_RTC_IsActiveFlag_RS(RTCx);

		if(timeout == 0) return Error_NotSynchro;
	}

	return Success;
}

void TimeClock::SetALMA(AlarmStatus *alma)
{
	uint32_t _Hours = __LL_RTC_CONVERT_BIN2BCD(alma->Hours);
	uint32_t _Minutes = __LL_RTC_CONVERT_BIN2BCD(alma->Minutes);
	uint32_t _Seconds = __LL_RTC_CONVERT_BIN2BCD(alma->Seconds);

	LL_RTC_ALMA_Disable(RTCx);

	// 日付をセットする
	if(alma->WeekDaySel == AlarmValue::WeekDay_Disable)
	{
		LL_RTC_ALMA_DisableWeekday(RTCx);
		LL_RTC_ALMA_SetDay(RTCx, __LL_RTC_CONVERT_BIN2BCD(alma->Day));
	}
	// 曜日をセットする
	else
	{
		LL_RTC_ALMA_EnableWeekday(RTCx);
		// 月曜を1、日曜を7として扱う
		LL_RTC_ALMA_SetWeekDay(RTCx, __LL_RTC_CONVERT_BIN2BCD(alma->Day));
	}

	// 24時間表記
	LL_RTC_ALMA_ConfigTime(RTCx, LL_RTC_ALMA_TIME_FORMAT_AM, _Hours, _Minutes, _Seconds);

	LL_RTC_ALMA_SetMask(RTCx, alma->Mask);
}

void TimeClock::SetALMB(AlarmStatus *almb)
{
	uint32_t _Hours = __LL_RTC_CONVERT_BIN2BCD(almb->Hours);
	uint32_t _Minutes = __LL_RTC_CONVERT_BIN2BCD(almb->Minutes);
	uint32_t _Seconds = __LL_RTC_CONVERT_BIN2BCD(almb->Seconds);

	LL_RTC_ALMB_Disable(RTCx);

	if(almb->WeekDaySel == AlarmValue::WeekDay_Disable)
	{
		LL_RTC_ALMA_DisableWeekday(RTCx);
		LL_RTC_ALMA_SetDay(RTCx, __LL_RTC_CONVERT_BIN2BCD(almb->Day));
	}
	else
	{
		LL_RTC_ALMA_EnableWeekday(RTCx);
		LL_RTC_ALMA_SetWeekDay(RTCx, __LL_RTC_CONVERT_BIN2BCD(almb->Day));
	}

	LL_RTC_ALMB_ConfigTime(RTCx, LL_RTC_ALMB_TIME_FORMAT_AM, _Hours, _Minutes, _Seconds);

	LL_RTC_ALMB_SetMask(RTCx, almb->Mask);
}


/* Public Function */

void TimeClock::ClockConfig(void)
{
	// RCC_BDCRレジスタも保護対象
	CheckDBP();

	LL_RCC_DisableRTC();

	// RTCのリセット。すべての設定を初期化
	LL_RCC_ForceBackupDomainReset();
	// 手動でクリアが必要
	LL_RCC_ReleaseBackupDomainReset();

	// LSE駆動能力の設定
	LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
	LL_RCC_LSE_Enable();
	while(LL_RCC_LSE_IsReady() != 1);

	// RTCクロックソースの設定
	LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
}

uint32_t TimeClock::Config(uint32_t HourFormat,uint32_t AsynchPrescaler,uint32_t SynchPrescaler)
{
	uint32_t ret;

	CheckDBP();

	LL_RTC_DisableWriteProtection(RTCx);	// RTCレジスタの書き込み保護を解除

	ret = CheckInitMode();

	if(ret != Error_Initialization)
	{
		LL_RTC_SetHourFormat(RTCx, HourFormat);
		LL_RTC_SetAsynchPrescaler(RTCx, AsynchPrescaler);
		LL_RTC_SetSynchPrescaler(RTCx, SynchPrescaler);

		LL_RTC_DisableInitMode(RTCx);	// 初期化モードの終了
	}

	LL_RTC_DisableShadowRegBypass(RTCx);

	LL_RTC_EnableWriteProtection(RTCx);	// 書き込み保護の有効化

	return ret;
}

uint32_t TimeClock::SetTime(uint32_t Format,uint32_t Hours,uint32_t Minutes,uint32_t Seconds)
{
	uint32_t ret;
	uint32_t _Hours = __LL_RTC_CONVERT_BIN2BCD(Hours);
	uint32_t _Minutes = __LL_RTC_CONVERT_BIN2BCD(Minutes);
	uint32_t _Seconds = __LL_RTC_CONVERT_BIN2BCD(Seconds);

	CheckDBP();

	LL_RTC_DisableWriteProtection(RTCx);

	ret = CheckInitMode();

	if(ret != Error_Initialization)
	{
		LL_RTC_TIME_Config(RTCx, Format, _Hours, _Minutes, _Seconds);

		LL_RTC_DisableInitMode(RTCx);
	}

	ret += WaitForSynchro();

	LL_RTC_EnableWriteProtection(RTCx);

	return ret;
}

uint32_t TimeClock::SetDate(uint32_t WeekDay,uint32_t Month,uint32_t Day,uint32_t Year)
{
	uint32_t ret;
	uint32_t _Month = __LL_RTC_CONVERT_BIN2BCD(Month);
	uint32_t _Day = __LL_RTC_CONVERT_BIN2BCD(Day);
	uint32_t _Year = __LL_RTC_CONVERT_BIN2BCD(Year);

	CheckDBP();

	LL_RTC_DisableWriteProtection(RTCx);

	ret = CheckInitMode();

	if(ret != Error_Initialization)
	{
		LL_RTC_DATE_Config(RTCx, WeekDay, _Day, _Month, _Year);

		LL_RTC_DisableInitMode(RTCx);
	}

	ret += WaitForSynchro();

	LL_RTC_EnableWriteProtection(RTCx);

	return ret;
}

uint32_t TimeClock::SetAlarm(ConfigParameters *init)
{
	uint32_t ret = Alarm_NotModule;

	CheckDBP();

	LL_RTC_DisableWriteProtection(RTCx);

	if((init->SelectAlarm == Options::ALMA) || (init->SelectAlarm == Options::ALMA_ALMB))
	{
		SetALMA(&(init->ALMA));
		ret = Success;
	}
	if((init->SelectAlarm == Options::ALMB) || (init->SelectAlarm == Options::ALMA_ALMB))
	{
		SetALMB(&(init->ALMB));
		ret = Success;
	}

	LL_RTC_DisableAlarmPullUp(RTCx);
	LL_RTC_EnableWriteProtection(RTCx);

	return ret;
}

uint32_t TimeClock::EnableAlarm(Options SelectAlarm)
{
	uint32_t ret = Alarm_NotModule;
	CheckDBP();

	LL_RTC_DisableWriteProtection(RTCx);

	if((SelectAlarm == Options::ALMA) || (SelectAlarm == Options::ALMA_ALMB))
	{
		LL_RTC_ALMA_Disable(RTCx);
		while(LL_RTC_IsActiveFlag_ALRAW(RTCx) == 0);
		LL_RTC_ClearFlag_ALRA(RTCx);
		LL_RTC_EnableIT_ALRA(RTCx);
		LL_RTC_ALMA_Enable(RTCx);

		ret = Success;
	}
	if((SelectAlarm == Options::ALMB) || (SelectAlarm == Options::ALMA_ALMB))
	{
		LL_RTC_ALMB_Disable(RTCx);
		while(LL_RTC_IsActiveFlag_ALRBW(RTCx) == 0);
		LL_RTC_ClearFlag_ALRB(RTCx);
		LL_RTC_EnableIT_ALRB(RTCx);
		LL_RTC_ALMB_Enable(RTCx);

		ret = Success;
	}

	LL_RTC_EnableWriteProtection(RTCx);

	return ret;
}
void TimeClock::SetWakeUpTimer(uint32_t Count)
{
	CheckDBP();

	LL_RTC_DisableWriteProtection(RTCx);

	// ウェイクアップタイマの初期化
	LL_RTC_WAKEUP_Disable(RTCx);
	// 更新が可能になるまで待つ
	while(LL_RTC_IsActiveFlag_WUTW(RTCx) == 0);

	LL_RTC_ClearFlag_WUT(RTCx);
	LL_RTC_EnableIT_WUT(RTCx);
	LL_RTC_WAKEUP_SetClock(RTCx, LL_RTC_WAKEUPCLOCK_CKSPRE);
	LL_RTC_WAKEUP_SetAutoReload(RTCx, Count);

	// WUTEビットのセット前に確認をする
	while(LL_RTC_IsActiveFlag_WUTW(RTCx) == 0);
	LL_RTC_WAKEUP_Enable(RTCx);
	while(LL_RTC_WAKEUP_IsEnabled(RTCx) != 1);

	LL_RTC_EnableWriteProtection(RTCx);
}

#if 0
void TimeClock::CheckDBP(void)
{
	// PWRドメインへのクロックチェック
	if(LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_PWR) != 1)
	{
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
		__NOP();
	}
	// DBPビットをセットしてRTCドメインへの書き込みを有効化
	if(LL_PWR_IsEnabledBkUpAccess() != 1)
	{
		LL_PWR_EnableBkUpAccess();
		while(LL_PWR_IsEnabledBkUpAccess() == 0);
	}
	// PWRドメインへのクロック停止
	if(LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_PWR) == 1)
	{
		LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
		__NOP();
	}
}
#endif

