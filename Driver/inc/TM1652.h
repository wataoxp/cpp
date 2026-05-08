/*
 * TM1652.h
 *
 *  Created on: Feb 6, 2026
 *      Author: wataoxp
 */

#ifndef INC_TM1652_H_
#define INC_TM1652_H_

#include "uart.h"
#include <stdint.h>

namespace Seg7{

	// 0~9の点灯パターン
	constexpr uint8_t DigNum[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

	// すべてのコマンドは下位4ビットを0b1000とする
	typedef enum :uint8_t{
		CommandBase = 1 << 3,
		DispCommand = 1 << 4,
	}Commands;

	// xxxx0000->Duty比
	// 0000xxx0->セグメント駆動電流
	// 0000000x->セグ数の選択
	typedef enum :uint8_t{
		DutyMask = 0xF0,
		CurrentMask = 0x0E,
		ModeMask = 0x01,
	}DispCommands;

	typedef enum :uint8_t{
		MinCurrent = 0,
		MaxCurrent = 7,
		MinDuty = 0,
		MaxDuty = 15,
	}DriveStatus;

	typedef enum :uint8_t{
		Success,
		Failed,
		UnderFlow,
		OverFlow,
	}Seg7RetrunCode;

	typedef enum :uint8_t{
		Seg8Dig5 = 0,
		Seg7Dig6 = 1,
	}SegmentMode;
}

class TM1652{
private:
	UART& uart;
	Seg7::SegmentMode SelectSeg;
	void tm_Delay(uint32_t timeout);
	void SendData(uint8_t *buf,uint8_t size);
	uint8_t ReverseByte(uint8_t b);
public:
	TM1652(UART& serial);
	void Init(uint8_t Current,uint8_t Duty);
	void Clear(void);
	uint32_t SetBrightness(uint8_t Current,uint8_t Duty);
	void WriteDig(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4);
	void WriteDig(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4,uint8_t dot);
};





#endif /* INC_TM1652_H_ */
