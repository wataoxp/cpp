/*
 * TM1652.cpp
 *
 *  Created on: Feb 6, 2026
 *      Author: wataoxp
 */

#include "TM1652.h"

using namespace Seg7;

TM1652::TM1652(UART& serial) :uart(serial)
{
	// ドットを含むと8セグ
	SelectSeg = Seg8Dig5;
}

inline void TM1652::SendData(uint8_t *buf,uint8_t size)
{
	uart.TransmitData(buf, size);
}

// 1データ送信後は3ms以上のアイドル状態(UARTではHigh)を作る
void TM1652::tm_Delay(uint32_t timeout)
{
	__IO uint32_t tmp = SysTick->CTRL;
	((void)tmp);

	while(timeout)
	{
		if (LL_SYSTICK_IsActiveCounterFlag() != 0)
		{
			timeout--;
		}
	}
}

// LSBtoMSB
uint8_t TM1652::ReverseByte(uint8_t b)
{
	b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
	b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
	b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;

	return b;
}

void TM1652::Init(uint8_t Current,uint8_t Duty)
{
	Clear();
	tm_Delay(15);
	SetBrightness(Current,Duty);
	tm_Delay(15);
}

void TM1652::Clear(void)
{
	uint8_t clear[5] = {0};
	clear[0] = CommandBase;

	SendData(clear, sizeof(clear));
	tm_Delay(5);
}

uint32_t TM1652::SetBrightness(uint8_t Current,uint8_t Duty)
{
	uint8_t data[2];
	uint32_t ret = Success;

	if(Current < MinCurrent || Duty < MinDuty)
	{
		Current = MinDuty;
		Duty = MinDuty;
		ret = UnderFlow;
	}
	else if(Current > MaxCurrent || Duty > MaxDuty)
	{
		Current = MaxCurrent;
		Duty = MaxDuty;
		ret = OverFlow;
	}

	data[0] = CommandBase | DispCommand;
	data[1] = ReverseByte(Duty) & DutyMask;		// ビット反転し下位4ビットを上位へ
	data[1] |= (ReverseByte(Current) >> 4) & CurrentMask;	// 同様に反転、シフトして下位へ
	data[1] |= SelectSeg;
	SendData(data, sizeof(data));

	tm_Delay(5);

	return ret;
}

void TM1652::WriteDig(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4)
{
	uint8_t buffer[5];
	buffer[0] = CommandBase | 0x00;		// 1桁目を指定
	buffer[1] = DigNum[d1];
	buffer[2] = DigNum[d2];
	buffer[3] = DigNum[d3];
	buffer[4] = DigNum[d4];

	SendData(buffer, sizeof(buffer));
	tm_Delay(5);
}

void TM1652::WriteDig(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4,uint8_t dot)
{
	uint8_t buffer[5];
	buffer[0] = CommandBase | 0x00;		// 1桁目を指定
	buffer[1] = DigNum[d1];
	buffer[2] = DigNum[d2];
	buffer[3] = DigNum[d3];
	buffer[4] = DigNum[d4];

	if(dot)
	{
		buffer[2] |= 0x80;
	}

	SendData(buffer, sizeof(buffer));
	tm_Delay(5);
}
