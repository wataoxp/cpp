/*
 * spi.h
 *
 *  Created on: Feb 21, 2025
 *      Author: wataoxp
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "periph.h"

#define NSS_AUTO_CONTROL 0
#define NSS_SOFT_CONTROL SPI_CR1_SSM
#define SSOE_OUTPUT_ENABLE SPI_CR2_SSOE
#define SSOE_OUTPUT_DISABLE 0
#define MSTR_MASTER SPI_CR1_MSTR
#define MSTR_SLAVE 0

namespace Serials{
	typedef enum{
		Succses,
		Leak,
		ConfigError,
	}SPIStatus;
}

typedef struct{
	/* CR1 */
	uint32_t TransferDirection;		//3線or4線(BIDIMODE)3線の場合は送信or受信専用の設定(BIDIOE)
	uint32_t Mode;					//MSTR
	uint32_t ClockPolarity;			//CPOL アイドル時のSCK極性
	uint32_t ClockPhase;			//CPHA 0なら最初のエッジ、1なら2番目のエッジで転送
	uint32_t NSS;					//SSMおよびSSOE

	uint32_t BaudRate;				//BR SCK分周比
	uint32_t BitOrder;				//LSBFIRST 0ならMSB、1ならLSB

	uint32_t SSI;					//内部NSS値 Modeでまとめて設定。マルチマスタでないなら常にセットして良い

	/* CR2 */
	uint32_t DataWidth;				//DS 転送データビット幅
	uint32_t SSOE;					//SSOE NSSメンバでまとめて設定する

}SPI_InitTypedef;

/*** NSSのマクロと挙動 ***
 *
 *  LL_SPI_NSS_SOFT SSMをセットし、NSSはソフト管理とする。SSIはLL_SPI_MODE_MASTERでセット
 *  LL_SPI_NSS_HARD_INPUT NSS、SSOE共に0、マルチマスタ用。SSIもクリアすること
 *  LL_SPI_NSS_HARD_OUTPUT NSSはMODIFYでクリア。SSOEをセット
 *
 *  マスター、スレーブともにシングル→NSS=0、SSOE=1
 *  マルチスレーブ→NSS=1、SSOEは不問
 *  マルチマスタ、またはスレーブ→NSS=0、SSOE=0
 *********************/

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

class SPI{
private:
	SPI_TypeDef *SPIx;
public:
	SPI(SPI_TypeDef *SPIPORT);

	void Config(SPI_InitTypedef *pConfig);

	void MasterTransmit(uint8_t *data,uint16_t length);
	void Receive(uint8_t *RXbuf,uint16_t length);

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
	inline void ChipSelect(GPIO_TypeDef *GPIOx,uint32_t PinPos)
	{
		GPIO_CLEAR(GPIOx,PinPos);
	}
	inline void ChipDeSelect(GPIO_TypeDef *GPIOx,uint32_t PinPos)
	{
		GPIO_WRITE(GPIOx,PinPos);
	}
};

#if 0
void SetRegister(SPI_InitTypedef *Config);
void StructInit(SPI_InitTypedef *Config,uint32_t NSS);
uint32_t SerialPinConfig(SerialPinStruct *obj,uint32_t SckAf,uint32_t MosiAf);
uint32_t SetPinNSS(SerialPinStruct *obj,uint32_t NssMode,uint32_t Alternate);
uint32_t SetPinMISO(SerialPinStruct *obj,uint32_t Alternate);
#endif

#endif /* INC_SPI_H_ */
