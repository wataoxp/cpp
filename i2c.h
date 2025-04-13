/*
 * i2c.h
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include <stdint.h>
#include "stm32g0xx_ll_i2c.h"

//I2Cスレーブメモリアドレス幅(byte)
#define I2C_MEMADD_SIZE_8BIT UINT8_C(1)
#define I2C_MEMADD_SIZE_16BIT UINT8_C(2)

//I2C_OAR1レジスタ設定用。7ビットアドレスの場合最下位ビット(OA１)は無視されるので１ビットシフトする
#define I2C_OA1_7BIT_Pos 1

#define I2C_CLOCK_400 UINT32_C(0x00C12166)
#define I2C_BUFFER_MAX 255
#define I2C_BUFFER_HALF 128

typedef enum{
	TX_ISR,
	TX_Poll,
}I2C_Mode;

typedef struct{
	uint8_t *BufAdd;
	uint8_t BufSize;
}I2C_IsrTypedef;

typedef enum{
	SoftEnd,
	AutoEnd,
}WireEndMode;

typedef enum{
	wire_Succses,
	wire_Over,
	wire_match,
	wire_mismatch,
}WireStatus;

class I2C{
private:
	I2C_TypeDef *I2Cx;
	uint8_t address;
	uint8_t index;
	uint8_t Buffer[I2C_BUFFER_HALF];
	//static inline void CR2SetUP(I2C_TypeDef *I2Cx,uint8_t address,uint16_t direction,uint8_t length,uint32_t EndMode);
	void endTransmission(WireEndMode mode);
	WireStatus requestFrom(uint8_t addr,uint8_t length);
public:
	I2C(I2C_TypeDef *I2CPORT);
	static void ConfigMaster(I2C_TypeDef *I2Cx);
	static void ConfigSlave(I2C_TypeDef *I2Cx,uint8_t OwnAddr);
	void beginTransmission(uint8_t addr);
	WireStatus Write(uint8_t val);
	WireStatus Write(uint8_t *data,uint8_t length);
	void endTransmission(void);
	WireStatus requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t Reg);
	WireStatus requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t HighByte,uint8_t LowByte);
	uint8_t Read(void);
	WireStatus Read(uint8_t *buf,uint8_t length);
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

#endif /* INC_I2C_H_ */
