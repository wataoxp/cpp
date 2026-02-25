/*
 * i2c.c
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#include "i2c.h"
#include "delay.h"
#include <string.h>

using namespace Wires;

I2C::I2C(I2C_TypeDef *I2CPORT) :I2Cx(I2CPORT)
{
	;
}
inline uint8_t I2C::GetMemAddLowByte(uint16_t address)
{
	return (uint8_t)(address & 0xFF);
}
inline uint8_t I2C::GetMemAddHighByte(uint16_t address)
{
	return (uint8_t)(address >> 8);
}

inline uint32_t I2C::GetWireBus(void)
{
	uint32_t Periphs = 0;

	if(I2Cx == I2C1)
	{
		Periphs = LL_APB1_GRP1_PERIPH_I2C1;
	}
#ifdef I2C2
	else if(I2Cx == I2C2)
	{
		Periphs = LL_APB1_GRP1_PERIPH_I2C2;
	}
#endif
#ifdef I2C3
	else if(I2Cx == I2C3)
	{
		Periphs = LL_APB1_GRP1_PERIPH_I2C2;
	}
#endif
	else
	{
		Periphs = Failed;
	}

	return Periphs;
}

/* Config */

uint32_t I2C::ConfigMaster(CoreClock clock)
{
	uint32_t Periphs;
	uint32_t Timing;

	Periphs = GetWireBus();

	if(Periphs == Failed)
	{
		return Failed;
	}

	Timing = (clock == PLLCLOCK)? SCL64MHz:SCL16MHz;

	LL_APB1_GRP1_EnableClock(Periphs);

	LL_I2C_Disable(I2Cx);
	LL_I2C_ConfigFilters(I2Cx, LL_I2C_ANALOGFILTER_ENABLE,0);
	LL_I2C_SetTiming(I2Cx, Timing);

	LL_I2C_EnableAutoEndMode(I2Cx);
	LL_I2C_EnableClockStretching(I2Cx);

	LL_I2C_Enable(I2Cx);

	return success;
}

template <typename wait>
bool I2C::IsActiveDevice(uint8_t addr)
{
	bool Result = false;
	uint32_t tmp = 0,count = 0;

	do
	{
		while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);
		LL_I2C_HandleTransfer(I2Cx, addr, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

		LL_I2C_TransmitData8(I2Cx, 0);

		wait::mDelay(5);		// 送信完了を疑似的に待つ
		tmp = LL_I2C_IsActiveFlag_NACK(I2Cx);	// NACKを受信したなら継続
		LL_I2C_ClearFlag_NACK(I2Cx);

		while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
		LL_I2C_ClearFlag_STOP(I2Cx);

		count++;
	}while((tmp) && (count < Wires::IsActiveMaxLoop));

	if(count < Wires::IsActiveMaxLoop) Result = true;

	return Result;
}

template bool I2C::IsActiveDevice<DelayMode::Standard>(uint8_t addr);
template bool I2C::IsActiveDevice<DelayMode::RtosMode>(uint8_t addr);

void I2C::ConfigSlave(uint8_t OwnAddr)
{
	uint32_t Timing = SCL64MHz;

	LL_I2C_Disable(I2Cx);
	LL_I2C_ConfigFilters(I2Cx, LL_I2C_ANALOGFILTER_ENABLE,0);
	LL_I2C_SetTiming(I2Cx, Timing);

	/* I2C Interrupt */
	LL_I2C_EnableIT_ADDR(I2Cx);
	LL_I2C_EnableIT_STOP(I2Cx);
	LL_I2C_EnableIT_RX(I2Cx);
	LL_I2C_EnableIT_TX(I2Cx);		//TXフラグはすぐに立てるべきではない？

	/* Slave Mode */
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	LL_I2C_DisableGeneralCall(I2Cx);
	LL_I2C_SetOwnAddress1(I2Cx, (OwnAddr << OAR1_BitPos), LL_I2C_OWNADDRESS1_7BIT);
	LL_I2C_EnableOwnAddress1(I2Cx);
	LL_I2C_DisableOwnAddress2(I2Cx);

	LL_I2C_Enable(I2Cx);
}

/* Connection */

// データのみ
uint32_t I2C::Transmit(uint8_t addr,uint8_t *TxBuf,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	LL_I2C_HandleTransfer(I2Cx, addr, LL_I2C_ADDRSLAVE_7BIT, length, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	for(uint8_t i = 0;i < length;i++)
	{
		LL_I2C_TransmitData8(I2Cx, TxBuf[i]);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);

	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	return success;
}

// デバイス内部のアドレスを指定
uint32_t I2C::MemWrite(uint8_t addr,uint16_t Reg,MemAdd MemAddSize,uint8_t *TxBuf,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	LL_I2C_HandleTransfer(I2Cx, addr, LL_I2C_ADDRSLAVE_7BIT, (length+MemAddSize), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	if(MemAddSize == MemAddSize8)
	{
		LL_I2C_TransmitData8(I2Cx, GetMemAddLowByte(Reg));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, GetMemAddHighByte(Reg));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, GetMemAddLowByte(Reg));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	for(uint8_t i = 0;i < length;i++)
	{
		LL_I2C_TransmitData8(I2Cx, TxBuf[i]);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);

	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	return success;
}

uint32_t I2C::Receive(uint8_t addr,uint8_t *RxBuf,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	LL_I2C_HandleTransfer(I2Cx, addr, LL_I2C_ADDRSLAVE_7BIT, length, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	for(uint8_t i = 0;i < length;i++)
	{
		while(LL_I2C_IsActiveFlag_RXNE(I2Cx) == 0);
		RxBuf[i] = LL_I2C_ReceiveData8(I2Cx);
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);

	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	return success;
}

uint32_t I2C::MemRead(uint8_t addr,uint16_t Reg,MemAdd MemAddSize,uint8_t *RxBuf,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);
	LL_I2C_HandleTransfer(I2Cx, addr, LL_I2C_ADDRSLAVE_7BIT, MemAddSize, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	if(MemAddSize == MemAddSize8)
	{
		LL_I2C_TransmitData8(I2Cx, GetMemAddLowByte(Reg));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, GetMemAddHighByte(Reg));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, GetMemAddLowByte(Reg));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(LL_I2C_IsActiveFlag_TC(I2Cx) == 0);
	LL_I2C_HandleTransfer(I2Cx, addr, LL_I2C_ADDRSLAVE_7BIT, length, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	for(uint8_t i = 0;i < length;i++)
	{
		while(LL_I2C_IsActiveFlag_RXNE(I2Cx) == 0);
		RxBuf[i] = LL_I2C_ReceiveData8(I2Cx);
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);

	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	return success;
}

//Reg1個だけ
void I2C::Write(uint8_t addr,uint8_t Reg)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2Cx->CR2 = 0;
	I2Cx->CR2 = addr | DirectionWrite << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | 1 << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND;

	LL_I2C_TransmitData8(I2Cx, Reg);
	while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}

//RegとDataを1個ずつ
void I2C::Write(uint8_t addr,uint8_t Reg,uint8_t Data)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2Cx->CR2 = 0;
	I2Cx->CR2 = addr | DirectionWrite << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | 2 << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND;

	LL_I2C_TransmitData8(I2Cx, Reg);
	while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);

	LL_I2C_TransmitData8(I2Cx, Data);
	while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}