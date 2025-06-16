/*
 * dma.h
 *
 *  Created on: Jun 8, 2025
 *      Author: wataoxp
 */

#ifndef DMA_H_
#define DMA_H_

#include "periph.h"

class DMA{
private:
	DMA_TypeDef *DMAx;
	uint32_t Channel;
	void SetISR(void);
public:
	DMA(DMA_TypeDef *DMAport,uint32_t channel);
	void Config(uint32_t Request);
	uint32_t AddressSet(uint8_t *MemoryAddress,uint32_t *PeriphAddress);
	uint32_t AddressSetM2M(uint8_t *SrcAddress,uint8_t *DstAddress);
	void SPItoDMA(SPI_TypeDef *SPIx,uint8_t *MemoryAddress);
	void StartDMA(uint32_t length);
	uint32_t StopDMAisChannel1(void);
	uint32_t StopDMAisChannel2(void);

};



#endif /* DMA_H_ */
