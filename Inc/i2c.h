/*
 * i2c.h
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

//#include "delay.h"
#include "periph.h"

namespace Wires{
	typedef enum{
		success,
		Failed,
		TxOver,
		RxOver,
	}WireStatus;

	typedef enum{
		MemAddSize8 = 1,
		MemAddSize16 = 2,
	}MemAdd;

	constexpr uint8_t DirectionWrite = 0;
	constexpr uint8_t DirectionRead = 1;

	// I2C_OAR1レジスタ設定用。7ビットアドレスの場合最下位ビット(OA１)は無視されるので１ビットシフトする
	constexpr uint8_t OAR1_BitPos = 1;

	constexpr uint32_t SCL64MHz = 0x00C12166;
	constexpr uint32_t SCL16MHz = 0x00300617;

	constexpr uint32_t IsActiveMaxLoop = 10;
}

class I2C{
private:
	I2C_TypeDef *I2Cx;
	uint8_t GetMemAddLowByte(uint16_t address);
	uint8_t GetMemAddHighByte(uint16_t address);
	uint32_t GetWireBus(void);
public:
	I2C(I2C_TypeDef *I2CPORT);
	uint32_t ConfigMaster(CoreClock clock);
	void ConfigSlave(uint8_t OwnAddr);
	template <typename wait>
	bool IsActiveDevice(uint8_t addr);

	uint32_t Transmit(uint8_t addr,uint8_t *TxBuf,uint8_t length);
    uint32_t MemWrite(uint8_t addr,uint16_t Reg,Wires::MemAdd MemAddSize,uint8_t *TxBuf,uint8_t length);
    uint32_t MemRead(uint8_t addr,uint16_t Reg,Wires::MemAdd MemAddSize,uint8_t *RxBuf,uint8_t length);

    uint32_t Receive(uint8_t addr,uint8_t *RxBuf,uint8_t length);
	void Write(uint8_t addr,uint8_t Reg);
	void Write(uint8_t addr,uint8_t Reg,uint8_t Data);

	void Enable(void);
	void Disable(void);
};

inline void I2C::Enable(void)
{
	LL_I2C_Enable(I2Cx);
}
inline void I2C::Disable(void)
{
	LL_I2C_Disable(I2Cx);
	/* P935、PEクリア後は3APBクロック分0で維持する。APB分周無と仮定したウェイト */
	__NOP();
	__NOP();
	__NOP();
}



#endif /* INC_I2C_H_ */
