/*
 * splc792a.cpp
 *
 *  Created on: Apr 10, 2026
 *      Author: wataoxp
 */

#include "splc792a.h"

using namespace LcdUnits;

SPLC792A::SPLC792A(I2C& i2c,DelayPoicy& pDelay) : wire(i2c),delay(pDelay)
{
	;
}

void SPLC792A::SoftReset(GPIO_TypeDef* RstPort,uint32_t RstPin)
{
	GPIO_CLEAR(RstPort,RstPin);
	delay.mDelay(1);
	GPIO_WRITE(RstPort,RstPin);
	delay.mDelay(40);
}

void SPLC792A::Init()
{
	uint8_t init[] = {
			FunctionSetOff,FunctionSetOn,IconContrastCmd,ContrastCmd,FunctionSetOff,
			DispCmdFull,ClearDisplay,
	};

	for(uint32_t i = 0;i < sizeof(init);i++)
	{
		wire.MemWrite(DeviceAddress, CmdCtrl, Wires::MemAddSize8, init, sizeof(init));
	}
}

void SPLC792A::PointClear()
{
	char ClearBuf[16];

	for(uint32_t i = 0;i < sizeof(ClearBuf);i++)
	{
		ClearBuf[i] = ' ';		// 半角スペース
	}

	CMDSend(ReturnHome);
	delay.mDelay(5);
	StringLCD(ClearBuf, sizeof(ClearBuf));
	CMDSend(HomeCursor);
	delay.mDelay(5);
}

