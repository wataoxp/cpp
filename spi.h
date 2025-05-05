/*
 * spi.h
 *
 *  Created on: Feb 21, 2025
 *      Author: wataoxp
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "stm32g0xx_ll_spi.h"

#define NSS_AUTO_CONTROL 0
#define NSS_SOFT_CONTROL SPI_CR1_SSM
#define SSOE_OUTPUT_ENABLE SPI_CR2_SSOE
#define SSOE_OUTPUT_DISABLE 0
#define MSTR_MASTER SPI_CR1_MSTR
#define MSTR_SLAVE 0


typedef struct{
	uint32_t TransferDirection;		//BIDIMODE&BIDIOE
	uint32_t Mode;					//MSTR
	uint32_t ClockPolarity;			//CPOL
	uint32_t ClockPhase;			//CPHA
	uint32_t NSS;					//SSM
	uint32_t SSI;					//NSSの入力値
	uint32_t SSOE;					//SSOE
	uint32_t BaudRate;				//BR
	uint32_t BitOrder;				//LSBFIRST
	uint32_t DataWidth;				//DS
}SPI_InitTypedef;

typedef struct{
	GPIO_TypeDef *PortSCK;
	GPIO_TypeDef *PortMOSI;
	GPIO_TypeDef *PortNSS;
	GPIO_TypeDef *PortMISO;
	uint32_t PinSCK;
	uint32_t PinMOSI;
	uint32_t PinNSS;
	uint32_t PinMISO;
}SerialPinStruct;

typedef enum{
	spi_Succses,
	spi_Leak,
	spi_ConfigError,
}SPIStatus;

class SPI{
private:
	SPI_TypeDef *SPIx;
	GPIO_TypeDef *PortNSS;
	uint32_t PinNSS;
public:
	SPI(SPI_TypeDef *SPIPORT);
 	uint32_t ConfigMaster(uint32_t NSS);
	void SetRegister(SPI_InitTypedef *Config);
	void StructInit(SPI_InitTypedef *Config,uint32_t NSS);
	uint32_t SerialPinConfig(SerialPinStruct *obj,uint32_t SckAf,uint32_t MosiAf);
	uint32_t SetPinNSS(SerialPinStruct *obj,uint32_t NssMode,uint32_t Alternate);
	uint32_t SetPinMISO(SerialPinStruct *obj,uint32_t Alternate);

	void MasterTransmit(uint8_t *data,uint16_t length);
	void SoftTransfer(uint8_t *data,uint16_t length);
	void Receive(uint8_t *RXbuf,uint16_t length);
	void SoftReceive(uint8_t *RXbuf,uint16_t length);

	uint8_t Transfer(uint8_t data);
	void ClearFIFO(void);
	inline void begin(void)
	{
		LL_SPI_Enable(SPIx);
		while(LL_SPI_IsActiveFlag_BSY(SPIx) != 0);
	}
	inline void end(void)
	{
		while(LL_SPI_IsActiveFlag_BSY(SPIx) != 0);
		while(LL_SPI_GetTxFIFOLevel(SPIx) != 0);
		LL_SPI_Disable(SPIx);
	}
};

#endif /* INC_SPI_H_ */
