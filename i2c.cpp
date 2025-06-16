/*
 * i2c.c
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#include "i2c.h"
#include "gpio.h"
#include <string.h>

#define I2C_BUFFER_SIZE I2C_BUFFER_HALF

constexpr uint8_t DirectionWrite = 0;
constexpr uint8_t DirectionRead = 1;

using namespace Wires;

I2C::I2C(I2C_TypeDef *I2CPORT) :I2Cx(I2CPORT)
{
	;
}

void I2C::ConfigMaster(void)
{
	uint32_t Timing = I2C_CLOCK_400;
	uint32_t Periphs;

	if(I2Cx == I2C1)
	{
		Periphs = LL_APB1_GRP1_PERIPH_I2C1;
	}
	else
	{
		Periphs = LL_APB1_GRP1_PERIPH_I2C2;
	}
	LL_APB1_GRP1_EnableClock(Periphs);

	LL_I2C_Disable(I2Cx);
	LL_I2C_ConfigFilters(I2Cx, LL_I2C_ANALOGFILTER_ENABLE,0);
	LL_I2C_SetTiming(I2Cx, Timing);

	LL_I2C_Enable(I2Cx);
	LL_I2C_EnableAutoEndMode(I2Cx);
	LL_I2C_EnableClockStretching(I2Cx);
}
uint32_t I2C::WirePinConfig(WirePinStruct *obj)
{
	uint32_t ret = 0;
	GPIO SCL(obj->PortSCL,obj->PinSCL);
	GPIO SDA(obj->PortSDA,obj->PinSDA);

	ret += SCL.Begin();
	ret += SDA.Begin();

	SCL.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_OPENDRAIN);
	SDA.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_OPENDRAIN);

	SCL.AlternateInit(obj->AlternateSCL);
	SDA.AlternateInit(obj->AlternateSDA);

	return ret;
}
/* I2C Function */
uint32_t I2C::Transmit(uint8_t addr,uint8_t *TxBuf,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2Cx->CR2 = 0;
	I2Cx->CR2 = addr | DirectionWrite << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | length << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND;

	for(uint8_t i = 0;i < length;i++)
	{
		LL_I2C_TransmitData8(I2Cx, TxBuf[i]);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);

	return Success;
}
uint32_t I2C::Transmit(uint8_t addr,uint8_t Reg,uint8_t *TxBuf,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2Cx->CR2 = 0;
	I2Cx->CR2 = addr | DirectionWrite << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | (length+1) << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND;

	LL_I2C_TransmitData8(I2Cx, Reg);
	while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);

	for(uint8_t i = 0;i < length;i++)
	{
		LL_I2C_TransmitData8(I2Cx, TxBuf[i]);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);

	return Success;
}
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
uint32_t I2C::Receive(uint8_t addr,uint8_t *RxBuf,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	I2Cx->CR2 = 0;
	I2Cx->CR2 = addr | DirectionRead << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | length << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND;

	for(uint8_t i = 0;i < length;i++)
	{
		while(LL_I2C_IsActiveFlag_RXNE(I2Cx) == 0);
		RxBuf[i] = LL_I2C_ReceiveData8(I2Cx);
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);

	return Success;
}
void I2C::ConfigSlave(uint8_t OwnAddr)
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

//uint32_t I2C::Write(uint8_t val)
//{
//	if(index >= I2C_BUFFER_SIZE)
//	{
//		return uint32_t::TxOver;
//	}
//	Buffer[index++] = val;
//
//	return uint32_t::succses;
//}
///* Wire Functions */
//void I2C::beginTransmission(uint8_t addr)
//{
//	address = addr;
//	index = 0;
//}
//uint32_t I2C::Write(uint8_t *data,uint8_t length)
//{
//	if((index + length) >= I2C_BUFFER_SIZE)
//	{
//		return uint32_t::TxOver;
//	}
//	for(uint8_t i = 0;i < length;i++)
//	{
//		Buffer[index++] = data[i];
//	}
//	return uint32_t::succses;
//}
//void I2C::endTransmission(void)
//{
//	this->endTransmission(AutoEnd);
//}
//void I2C::endTransmission(WireEndMode mode)
//{
//	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);
//
//	I2Cx->CR2 = 0;
//	I2Cx->CR2 = address | 0 << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | index << I2C_CR2_NBYTES_Pos | mode << I2C_CR2_AUTOEND_Pos;
//
//	for(uint8_t i = 0;i < index;i++)
//	{
//		LL_I2C_TransmitData8(I2Cx, Buffer[i]);
//		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
//	}
//
//	if(mode == AutoEnd)
//	{
//		while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
//		LL_I2C_ClearFlag_STOP(I2Cx);
//	}
//	else
//	{
//		while(LL_I2C_IsActiveFlag_TC(I2Cx) == 0);
//	}
//	index = 0;
//}
//uint32_t I2C::requestFrom(uint8_t addr,uint8_t length)
//{
//	uint8_t i;
//
//	I2Cx->CR2 = 0;
//	I2Cx->CR2 = address | 1 << I2C_CR2_RD_WRN_Pos | I2C_CR2_START | length << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND;
//
//	for(i = 0;i < length;i++)
//	{
//		while(LL_I2C_IsActiveFlag_RXNE(I2Cx) == 0);
//		Buffer[i] = LL_I2C_ReceiveData8(I2Cx);
//	}
//
//	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
//	LL_I2C_ClearFlag_STOP(I2Cx);
//
//	index = 0;
//
//	return (i == length)? uint32_t::succses:uint32_t::RxOver;
//}
//
//uint32_t I2C::requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t Reg)
//{
//	this->beginTransmission(addr);
//	this->Write(Reg);
//	this->endTransmission(mode);
//
//	return this->requestFrom(addr, length);
//}
//uint32_t I2C::requestFrom(uint8_t addr,uint8_t length,WireEndMode mode,uint8_t HighByte,uint8_t LowByte)
//{
//	this->beginTransmission(addr);
//	this->Write(HighByte);
//	this->Write(LowByte);
//	this->endTransmission(mode);
//
//	return this->requestFrom(addr, length);
//}
//uint8_t I2C::Read(void)
//{
//	if(index < I2C_BUFFER_SIZE)
//	{
//		return Buffer[index++];
//	}
//	return 0;
//}
//uint32_t I2C::Read(uint8_t *buf,uint8_t length)
//{
//	if(length > I2C_BUFFER_SIZE)
//	{
//		return uint32_t::RxOver;
//	}
//	for(uint8_t i = 0;i < length;i++)
//	{
//		buf[i] = Buffer[i];
//	}
//	return uint32_t::succses;
//}
