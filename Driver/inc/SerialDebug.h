/*
 * SerialDebug.h
 *
 *  Created on: May 2, 2026
 *      Author: wataoxp
 */

#ifndef INC_SERIALDEBUG_H_
#define INC_SERIALDEBUG_H_

#include "uart.h"
#include "i2c.h"
#include "rda.h"

namespace SerDebug{
	constexpr char Initial[]	= "NotCommand\r\nw,Reg,Addr/r,Reg\r\n";
	constexpr char Enter[] 		= "\r\n";
	constexpr char BufferOver[] = "BufferOver,Limit50Byte\r\n";

	enum ReturnCode{
		Complete,
		EndOfText,
		Over,
		ErrorCmd,
	};

	typedef struct{
		char Command;
		uint32_t Addr;
		uint32_t Data;
	}SplitData;
}

class SerialDebug{
private:
	UART& Serial;
	I2C& wire;
	char string[50];
	uint8_t DeviceAddress;

	void ExtSetRegister(uint8_t Reg, uint16_t value);
	uint16_t ExtGetRegister(uint8_t Reg);

	void WriteSerial(char* str,uint8_t length);
	uint8_t ReadSerial();
	SerDebug::ReturnCode GetTerminalString();
	SerDebug::ReturnCode StringAnalyze();
public:
	SerialDebug(UART& uart,I2C& i2c,uint8_t Address);
	void SerialControl();
};






#endif /* INC_SERIALDEBUG_H_ */
