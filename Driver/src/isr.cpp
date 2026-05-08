/*
 * isr.cpp
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */


#include "isr.h"
#include "debounce.h"

static FlagISR iFlag;

/* Getter/Setter */
const FlagISR& GetISRStruct(void)
{
	return (const FlagISR&)iFlag;
}

void ResetExtCommand(void)	// もし割り込みとの競合が発生するならここで割り込みを一時停止する
{
	iFlag.ExtCommand = 0;
}

/* Interrupt Service Routine */

extern "C"{

void SysTick_Handler(void)
{
	static __IO uint32_t mTick;

	mTick++;

	if((mTick & DebouceUnits::CheckIntervalMask) != 0)
	{
		return;
	}
	Debounce::ButtonFilter(iFlag.ExtCommand);
}


void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

}// extern "C"

