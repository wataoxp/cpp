/*
 * spi.c
 *
 *  Created on: Feb 21, 2025
 *      Author: wataoxp
 */
#include "spi.h"

using namespace Serials;

SPI::SPI(SPI_TypeDef *SPIPORT) :SPIx(SPIPORT)
{
	;
}
void SPI::Config(SPI_InitTypedef *pConfig)
{
	if(LL_SPI_IsEnabled(SPIx))
	{
		LL_SPI_Disable(SPIx);
	}

	if(SPIx == SPI1)
	{
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
	}
#ifdef STM32G0xx
	else
	{
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
	}
#endif
	LL_SPI_SetTransferDirection(SPIx, pConfig->TransferDirection);
	LL_SPI_SetClockPolarity(SPIx, pConfig->ClockPolarity);
	LL_SPI_SetClockPhase(SPIx, pConfig->ClockPhase);
	LL_SPI_SetTransferBitOrder(SPIx, pConfig->BitOrder);
	LL_SPI_SetBaudRatePrescaler(SPIx, pConfig->BaudRate);
	LL_SPI_SetMode(SPIx, pConfig->Mode);

	LL_SPI_SetDataWidth(SPIx, pConfig->DataWidth);
	LL_SPI_SetNSSMode(SPIx, pConfig->NSS);

    if(pConfig->DataWidth < LL_SPI_DATAWIDTH_9BIT)		//データサイズが8ビット以下ならFRXTHをセットする
    {
    	LL_SPI_SetRxFIFOThreshold(SPIx, LL_SPI_RX_FIFO_TH_QUARTER);
    }
    LL_SPI_DisableCRC(SPIx);
    LL_I2S_Disable(SPIx);								//I2SとSPIは排他的な関係

	LL_SPI_SetStandard(SPIx, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_DisableNSSPulseMgt(SPIx);
}
/*** SPI Tx&Rx ***/
void SPI::MasterTransmit(uint8_t *data,uint16_t length)
{
	LL_SPI_Enable(SPIx);

	while(LL_SPI_IsActiveFlag_BSY(SPIx) != 0);

	for(uint16_t i = 0;i < length;i++)
	{
		while(LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
		LL_SPI_TransmitData8(SPIx, data[i]);
	}
	while(LL_SPI_IsActiveFlag_BSY(SPIx) != 0);
	while(LL_SPI_GetTxFIFOLevel(SPIx) != 0);

	LL_SPI_Disable(SPIx);
}

//スレーブモード時はSPI有効をこの関数が呼ばれる前に実行しておくこと
void SPI::Receive(uint8_t *RXbuf,uint16_t length)
{
	LL_SPI_Enable(SPIx);

	while(LL_SPI_IsActiveFlag_BSY(SPIx) != 0);

	for(uint16_t i = 0;i < length;i++)
	{
		while(LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
		while(LL_SPI_IsActiveFlag_RXNE(SPIx) == 0);
		LL_SPI_TransmitData8(SPIx, 0);
		RXbuf[i] = LL_SPI_ReceiveData8(SPIx);
	}
	while(LL_SPI_IsActiveFlag_BSY(SPIx) != 0);
	while(LL_SPI_GetTxFIFOLevel(SPIx) != 0);

	LL_SPI_Disable(SPIx);
}
/*** Arduino SPI ***/
uint8_t SPI::Transfer(uint8_t data)
{
	LL_SPI_TransmitData8(SPIx, data);
	while(LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
	while(LL_SPI_IsActiveFlag_RXNE(SPIx) == 0);

	return LL_SPI_ReceiveData8(SPIx);
}

void SPI::ClearFIFO(void)
{
	while(LL_SPI_IsActiveFlag_BSY(SPIx) != 0);
	while(LL_SPI_GetRxFIFOLevel(SPIx) != LL_SPI_RX_FIFO_EMPTY)
	{
		LL_SPI_ReceiveData8(SPIx);
	}
}

#if 0
void SPI::SetRegister(SPI_InitTypedef *Config)
{
	uint16_t CR1Reg = Config->TransferDirection |  Config->Mode |Config->ClockPolarity | \
    		Config->ClockPhase | Config->NSS | Config->SSI | Config->BaudRate | Config->BitOrder;

	WRITE_REG(SPIx->CR1,CR1Reg);

    MODIFY_REG(SPIx->CR2,SPI_CR2_DS | SPI_CR2_SSOE,Config->DataWidth | Config->SSOE);

    if(Config->DataWidth < LL_SPI_DATAWIDTH_9BIT)		//データサイズが8ビット以下ならFRXTHをセットする
    {
    	LL_SPI_SetRxFIFOThreshold(SPIx, LL_SPI_RX_FIFO_TH_QUARTER);
    }
    LL_SPI_DisableCRC(SPIx);

    LL_I2S_Disable(SPIx);								//I2SとSPIは排他的な関係

	LL_SPI_SetStandard(SPIx, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_DisableNSSPulseMgt(SPIx);
}
void SPI::StructInit(SPI_InitTypedef *Config,uint32_t NSS)
{
	Config->TransferDirection = LL_SPI_FULL_DUPLEX;
	Config->DataWidth = LL_SPI_DATAWIDTH_8BIT;
	Config->ClockPolarity = LL_SPI_POLARITY_HIGH;
	Config->ClockPhase = LL_SPI_PHASE_2EDGE;
//	Config->ClockPolarity = LL_SPI_POLARITY_LOW;
//	Config->ClockPhase = LL_SPI_PHASE_1EDGE;
	Config->BitOrder = LL_SPI_MSB_FIRST;
	Config->BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;		//SPIクロック周波数分周比
	Config->Mode = SPI_CR1_MSTR;
	Config->SSOE = SPI_CR2_SSOE;
	Config->NSS = NSS;

	if(NSS == LL_SPI_NSS_SOFT)
	{
		Config->SSOE = SSOE_OUTPUT_DISABLE;
		Config->SSI = SPI_CR1_SSI;
	}
}
/**** SPI GPIO Setting ***/
//AFもメンバに入れる
uint32_t SPI::SerialPinConfig(SerialPinStruct *obj,uint32_t SckAf,uint32_t MosiAf)
{
	uint32_t ret = 0;

	GPIO SCK(obj->PortSCK,obj->PinSCK);
	GPIO MOSI(obj->PortMOSI,obj->PinMOSI);

	ret += SCK.Begin();
	ret += MOSI.Begin();

	SCK.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
	MOSI.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);

	SCK.AlternateInit(SckAf);
	MOSI.AlternateInit(MosiAf);

	return ret;
}
uint32_t SPI::SetPinNSS(SerialPinStruct *obj,uint32_t NssMode,uint32_t Alternate)
{
	uint32_t ret = 0;
	GPIO NSS(obj->PortNSS,obj->PinNSS);

	ret += NSS.Begin();

	if(NssMode == LL_SPI_NSS_HARD_OUTPUT)
	{
		NSS.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
		NSS.AlternateInit(Alternate);
	}
	else
	{
		NSS.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_OUTPUT, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
		NSS.OutputInit();
		GPIO_WRITE(obj->PortNSS,obj->PinNSS);			//アクティブローなのでハイにしておく
	}

	return ret;
}
uint32_t SPI::SetPinMISO(SerialPinStruct *obj,uint32_t Alternate)
{
	uint32_t ret = 0;
	GPIO MISO(obj->PortMISO,obj->PinMISO);

	ret += MISO.Begin();
	MISO.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
	MISO.AlternateInit(Alternate);

	return ret;
}
#endif
