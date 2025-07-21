/*
 * i2c.h
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "periph.h"

//I2Cスレーブメモリアドレス幅(byte)
#define I2C_MEMADD_SIZE_8BIT UINT8_C(1)
#define I2C_MEMADD_SIZE_16BIT UINT8_C(2)

//I2C_OAR1レジスタ設定用。7ビットアドレスの場合最下位ビット(OA１)は無視されるので１ビットシフトする
#define I2C_OA1_7BIT_Pos 1
#define I2C_CLOCK_400 UINT32_C(0x00C12166)

namespace Wires{
	typedef enum{
		Success,
		Failed,
		TxOver,
		RxOver,
	}WireStatus;

	typedef enum{
		MemAddSize8 = 1,
		MemAddSize16 = 2,
	}MemAdd;
}

//typedef struct{
//	GPIO_TypeDef *PortSCL;
//	GPIO_TypeDef *PortSDA;
//	uint32_t PinSCL;
//	uint32_t PinSDA;
//	uint32_t AlternateSCL;
//	uint32_t AlternateSDA;
//}WirePinStruct;

class I2C{
private:
	I2C_TypeDef *I2Cx;
	uint32_t TimeOut;
	inline uint8_t GetMemAddLowByte(uint16_t address);
	inline uint8_t GetMemAddHighByte(uint16_t address);
public:
	I2C(I2C_TypeDef *I2CPORT);
	void ConfigMaster(void);
	void ConfigSlave(uint8_t OwnAddr);

	inline void Enable(void)
	{
		LL_I2C_Enable(I2Cx);
	}
	inline void Disable(void)
	{
		LL_I2C_Disable(I2Cx);
		/* P935、PEクリア後は3APBクロック分0で維持する。APB分周無と仮定したウェイト */
		__NOP();
		__NOP();
		__NOP();
	}

	uint32_t IsActiveDevice(uint8_t addr,uint32_t TimeOut);

	uint32_t Transmit(uint8_t addr,uint8_t *TxBuf,uint8_t length);
    uint32_t MemWrite(uint8_t addr,uint16_t Reg,Wires::MemAdd MemAddSize,uint8_t *TxBuf,uint8_t length);
    uint32_t MemRead(uint8_t addr,uint16_t Reg,Wires::MemAdd MemAddSize,uint8_t *RxBuf,uint8_t length);

    uint32_t Receive(uint8_t addr,uint8_t *RxBuf,uint8_t length);
	void Write(uint8_t addr,uint8_t Reg);
	void Write(uint8_t addr,uint8_t Reg,uint8_t Data);
};

/**
 * I2C_CR2 Register Config
 * @param I2Cx...I2C Instance
 * @param address...SlaveAddress
 * @param direction...I2C Transmission Direction
 * @arg LL_I2C_REQUEST_WRITE
 * @arg LL_I2C_REQUEST_READ
 * @param length...Transmission Size
 * @param EndMode...REPEAT STRATorSTOPFLAG
 * @arg LL_I2C_MODE_SOFTEND...ReStart
 * @arg LL_I2C_MODE_AUTOEND...STOPF
 **/
inline void CR2SetUP(I2C_TypeDef *I2Cx,uint8_t address,uint16_t direction,uint8_t length,uint32_t EndMode)
{
	uint32_t tmp = 0;
	tmp = (address & I2C_CR2_SADD)  | (direction & I2C_CR2_RD_WRN) | I2C_CR2_START | (length << I2C_CR2_NBYTES_Pos) | (EndMode & I2C_CR2_AUTOEND);

	MODIFY_REG(I2Cx->CR2,I2C_CR2_SADD|I2C_CR2_RD_WRN|I2C_CR2_NBYTES|I2C_CR2_START|I2C_CR2_AUTOEND,tmp);
}
/* Wire */
/****************************************************************
#define I2C_BUFFER_MAX 255
#define I2C_BUFFER_HALF 128
typedef enum{
	SoftEnd,
	AutoEnd,
}WireEndMode;
class I2C{
private:
	I2C_TypeDef *I2Cx;
	uint8_t address;
	uint8_t index;
	uint8_t Buffer[I2C_BUFFER_HALF];
	void endTransmission(WireEndMode mode);
	uint32_t requestFrom(uint8_t addr,uint8_t length);
public:
	I2C(I2C_TypeDef *I2CPORT);
	void ConfigMaster(void);
	void ConfigSlave(uint8_t OwnAddr);
	void beginTransmission(uint8_t addr);
	uint32_t Write(uint8_t val);
	uint32_t Write(uint8_t *data,uint8_t length);
	void endTransmission(void);
	uint32_t requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t Reg);
	uint32_t requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t HighByte,uint8_t LowByte);
	uint8_t Read(void);
	uint32_t Read(uint8_t *buf,uint8_t length);
};
*********************************************************************/
#endif /* INC_I2C_H_ */
