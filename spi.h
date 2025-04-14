/*
 * spi.h
 *
 *  Created on: Feb 21, 2025
 *      Author: wataoxp
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "periph.h"
#include "stm32g0xx_ll_spi.h"

#define NSS_AUTO_CONTROL 0
#define NSS_SOFT_CONTROL SPI_CR1_SSI
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

typedef enum{
	spi_Succses,
	spi_Leak,
	spi_ConfigError,
}SPIStatus;

class SPI{
private:
	SPI_TypeDef *SPIx;
public:
	SPI(SPI_TypeDef *SPIPORT);
	static SPIStatus Config(SPI_TypeDef *SPIx,SPI_InitTypedef *config);
	static void StructInit(SPI_InitTypedef *config,uint32_t SPI_Mode,uint32_t NSS_Mode);
	SPIStatus Transfer(uint8_t *data,uint16_t length);
	void TransmitReceive(uint8_t *TXbuf,uint8_t *RXbuf,uint16_t length);
};

//ErrorStatus SPI_Init(SPI_TypeDef *SPIx,SPI_InitTypedef *config);
//void SPI_StructInit(SPI_InitTypedef *config,uint32_t SPI_Mode,uint32_t NSS_Mode);
//uint8_t SPI_Transmit8(SPI_TypeDef *SPIx,uint8_t *buf,uint16_t length);
void SPI_MasterTransmitReceive8(SPI_TypeDef *SPIx,uint8_t *TXbuf,uint8_t *Rxbuf,uint32_t size);

#endif /* INC_SPI_H_ */
