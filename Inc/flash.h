/*
 * falsh.h
 *
 *  Created on: May 8, 2026
 *      Author: wataoxp
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32g0xx.h"

namespace FlashUnits{
	struct LockKey{
		LockKey() = delete;

		// RM0444 P65
		constexpr static uint32_t Key01 =  0x45670123;
		constexpr static uint32_t Key02 =  0xCDEF89AB;
	};

	typedef struct{
		uint32_t High;
		uint32_t Low;
	}FlashData;
}


class Flash{
private:
	void WaitBusy();

	uint32_t CheckErrorFlag();
	void ClearErrorFlag();

	uint32_t GetLockFlag();

	void EnableDelete();
	void DisableDelete();
	void StartDelete();

	void EnableWrite();
	void DisableWrite();
public:
	Flash();
	void Lock();
	void UnLock();
	uint32_t ErasePage(uint32_t Page);
	uint32_t WriteDoubleWord(uint32_t address,uint64_t data);
	void ReadDoubleWord(uint32_t address,FlashUnits::FlashData* data);
};





#endif /* INC_FLASH_H_ */
