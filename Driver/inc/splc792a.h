/*
 * splc792a.h
 *
 *  Created on: Apr 10, 2026
 *      Author: wataoxp
 */

#ifndef INC_SPLC792A_H_
#define INC_SPLC792A_H_

#include "i2c.h"
#include "delay.h"

namespace LcdUnits
{
    constexpr uint8_t DeviceAddress = (0x3E << 1);
    constexpr uint8_t CmdCtrl = 0x00;
    constexpr uint8_t DataCtrl = 0x40;

    enum Commands : uint8_t{
        ClearDisplay = 0x01,
        ReturnHome = 0x02,
        SetCursor = 0x80
    };

    enum CursorControl : uint8_t{
        EnterCursor = 0x40, // 16*2モード 2行目左端
        HomeCursor = 0x03
    };

    enum DisplaySettings : uint8_t{
        DispOn = 0x04,
        CursorOn = 0x02,
        BlinkCursor = 0x01,
        DispCmdBase = 0x08,
        DispCmdFull = DispCmdBase | DispOn
    };

    enum MemoryAccess : uint8_t{
        CgramAccess = 0x40,
        DdramAccess = 0x80
    };

    enum FunctionSet : uint8_t{
        FunctionSetOn = 0x39, // 8bit, 2line, 5*7dot, instruction table 1
        FunctionSetOff = 0x38  // instruction table 0
    };

    enum ExtensionMode : uint8_t{
        ContrastHigh = 0x03,
        IconContrastCmd = 0x50 | ContrastHigh,
        ContrastLow = 0x0F,
        ContrastCmd = 0x70 | ContrastLow,
        VoltageSet = 0x03,
        VoltageCmd = 0x60 | VoltageSet
    };

    enum PowerSave : uint8_t{
        ContrastHighOff = 0x50,
        ContrastLowOff = 0x70,
        LowVoltageSet = 0x60 | 0x00
    };
}

class SPLC792A{
private:
	I2C& wire;
	DelayPoicy& delay;
public:
	SPLC792A(I2C& i2c,DelayPoicy& pDelay);
	void SoftReset(GPIO_TypeDef* RstPort,uint32_t RstPin);
	void Init();
	void PointClear();

	void StringLCD(const char *str, uint8_t size);
	void SetCusor(uint8_t x, uint8_t y);
	void ClearLCD();
	void CMDSend(uint8_t cmd);
};

inline void SPLC792A::StringLCD(const char *str, uint8_t size)
{
	using namespace LcdUnits;
    wire.MemWrite(DeviceAddress, DataCtrl, Wires::MemAddSize8, (uint8_t*)str, size);
}

inline void SPLC792A::SetCusor(uint8_t x, uint8_t y)
{
    using namespace LcdUnits;
    wire.Write(DeviceAddress, CmdCtrl, DdramAccess | (x + y * EnterCursor));
}

inline void SPLC792A::ClearLCD()
{
    using namespace LcdUnits;
    wire.Write(DeviceAddress, CmdCtrl, ClearDisplay);
}

inline void SPLC792A::CMDSend(uint8_t cmd)
{
	using namespace LcdUnits;
	wire.Write(DeviceAddress, CmdCtrl, cmd);
}

#endif /* INC_SPLC792A_H_ */
