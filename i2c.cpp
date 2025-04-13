/*
 * i2c.c
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#include "i2c.h"
#include <string.h>

#define I2C_BUFFER_SIZE I2C_BUFFER_HALF

I2C::I2C(I2C_TypeDef *I2CPORT)
{
	I2Cx = I2CPORT;
}

void I2C::ConfigMaster(I2C_TypeDef *I2Cx)
{
	uint32_t Timing = I2C_CLOCK_400;

	LL_I2C_Disable(I2Cx);
	LL_I2C_ConfigFilters(I2Cx, LL_I2C_ANALOGFILTER_ENABLE,0);
	LL_I2C_SetTiming(I2Cx, Timing);

	LL_I2C_Enable(I2Cx);
	LL_I2C_EnableAutoEndMode(I2Cx);
	LL_I2C_EnableClockStretching(I2Cx);
}
void I2C::beginTransmission(uint8_t addr)
{
	address = addr;
	index = 0;
}
WireStatus I2C::Write(uint8_t val)
{
	if(index >= I2C_BUFFER_SIZE)
	{
		return wire_Over;
	}
	Buffer[index++] = val;

	return wire_Succses;
}
WireStatus I2C::Write(uint8_t *data,uint8_t length)
{
	if((index + length) >= I2C_BUFFER_SIZE)
	{
		return wire_Over;
	}
	for(uint8_t i = 0;i < length;i++)
	{
		Buffer[index++] = data[i];
	}
	return wire_Succses;
}
void I2C::endTransmission(void)
{
	this->endTransmission(AutoEnd);
}
void I2C::endTransmission(WireEndMode mode)
{
	while((I2Cx->ISR & I2C_ISR_BUSY) != 0);

	I2Cx->CR2 = 0;
	I2Cx->CR2 = address | 0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | index << I2C_CR2_NBYTES_Pos | mode << I2C_CR2_AUTOEND_Pos;

	for(uint8_t i = 0;i < index;i++)
	{
		I2Cx->TXDR = Buffer[i];
		while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	}

	if(mode == AutoEnd)
	{
		while((I2Cx->ISR & I2C_ISR_STOPF) == 0);
		I2Cx->ICR |= I2C_ICR_STOPCF;
	}
	else
	{
		while((I2Cx->ISR & I2C_ISR_TC) == 0);
	}
	index = 0;
}
WireStatus I2C::requestFrom(uint8_t addr,uint8_t length)
{
	uint8_t i;

	I2Cx->CR2 = 0;
	I2Cx->CR2 = address | 1 << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | length << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND;

	for(i = 0;i < length;i++)
	{
		while((I2Cx->ISR & I2C_ISR_RXNE) == 0);
		Buffer[i] = I2Cx->RXDR;
	}

	while((I2Cx->ISR & I2C_ISR_STOPF) == 0);
	I2Cx->ICR |= I2C_ICR_STOPCF;

	index = 0;

	return (i == length)? wire_match:wire_mismatch;
}

WireStatus I2C::requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t Reg)
{
	this->beginTransmission(addr);
	this->Write(Reg);
	this->endTransmission(mode);

	return this->requestFrom(addr, length);
}
WireStatus I2C::requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t HighByte,uint8_t LowByte)
{
	this->beginTransmission(addr);
	this->Write(HighByte);
	this->Write(LowByte);
	this->endTransmission(mode);

	return this->requestFrom(addr, length);
}
uint8_t I2C::Read(void)
{
	if(index < I2C_BUFFER_SIZE)
	{
		return Buffer[index++];
	}
	return 0;
}
WireStatus I2C::Read(uint8_t *buf,uint8_t length)
{
	if(length > I2C_BUFFER_SIZE)
	{
		return wire_Over;
	}
	for(uint8_t i = 0;i < length;i++)
	{
		buf[i] = Buffer[i];
	}
	return wire_Succses;
}
void I2C::ConfigSlave(I2C_TypeDef *I2Cx,uint8_t OwnAddr)
{
	uint32_t Timing = I2C_CLOCK_400;

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
	LL_I2C_SetOwnAddress1(I2Cx, (OwnAddr << I2C_OA1_7BIT_Pos), LL_I2C_OWNADDRESS1_7BIT);
	LL_I2C_EnableOwnAddress1(I2Cx);
	LL_I2C_DisableOwnAddress2(I2Cx);

	LL_I2C_Enable(I2Cx);
}
#if 0
void I2C::PushTransmit(uint8_t address,uint8_t data)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2C::CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, 1, LL_I2C_MODE_AUTOEND);

	LL_I2C_TransmitData8(I2Cx, data);
	while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C::MasterTransmit(uint8_t address,uint8_t *data,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2C::CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, length, LL_I2C_MODE_AUTOEND);

	while(length)
	{
		LL_I2C_TransmitData8(I2Cx, *data++);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		length--;
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C::MasterReceive(uint8_t address,uint8_t *buffer,uint8_t length)
{
	I2C::CR2SetUP(I2Cx, address, LL_I2C_REQUEST_READ, length, LL_I2C_MODE_AUTOEND);

	while(length)
	{
		while(LL_I2C_IsActiveFlag_RXNE(I2Cx) == 0);
		*buffer++ = LL_I2C_ReceiveData8(I2Cx);
		length--;
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C::PushMemWrite(uint8_t address,uint8_t data,uint16_t Reg,uint8_t RegSize)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2C::CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+1, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	LL_I2C_TransmitData8(I2Cx, data);

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C::SeqMemWrite(uint8_t address,uint8_t *data,uint16_t Reg,uint8_t RegSize,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2C::CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+length, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(length)
	{
		LL_I2C_TransmitData8(I2Cx, *data++);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		length--;
	}
	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void PushI2C_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t data,uint16_t Reg,uint8_t RegSize)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+1, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	LL_I2C_TransmitData8(I2Cx, data);

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void SeqI2C_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint16_t Reg,uint8_t RegSize,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+length, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(length)
	{
		LL_I2C_TransmitData8(I2Cx, *data++);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		length--;
	}
	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C_Mem_Read(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *buffer,uint16_t Reg,uint8_t RegSize,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);
	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE , RegSize, LL_I2C_MODE_SOFTEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(LL_I2C_IsActiveFlag_TC(I2Cx) == 0); 	//転送が完了したかチェック

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_READ, length, LL_I2C_MODE_AUTOEND);

	while(length)
	{
		while(LL_I2C_IsActiveFlag_RXNE(I2Cx) == 0);
		*buffer++ = LL_I2C_ReceiveData8(I2Cx);
		length--;
	}
	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
#endif
