/*
 * flash.cpp
 *
 *  Created on: May 8, 2026
 *      Author: wataoxp
 */

#include "flash.h"

using namespace FlashUnits;

Flash::Flash()
{
	;
}

inline void Flash::WaitBusy()
{
	while(READ_BIT(FLASH->SR,FLASH_SR_BSY1) != 0);
}

inline uint32_t Flash::GetLockFlag()
{
	return READ_BIT(FLASH->CR,FLASH_CR_LOCK);
}

inline uint32_t Flash::CheckErrorFlag()
{
	return READ_BIT(FLASH->SR,FLASH_SR_EOP | FLASH_SR_WRPERR | FLASH_SR_PGSERR);
}

inline void Flash::ClearErrorFlag()
{
	// エラーフラグは1を書き込むとクリアされる
	WRITE_REG(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPERR | FLASH_SR_PGSERR);
}

inline void Flash::EnableDelete()
{
	SET_BIT(FLASH->CR,FLASH_CR_PER);
}

inline void Flash::DisableDelete()
{
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
}

inline void Flash::StartDelete()
{
	SET_BIT(FLASH->CR,FLASH_CR_STRT);
}

inline void Flash::EnableWrite()
{
	SET_BIT(FLASH->CR,FLASH_CR_PG);
}

inline void Flash::DisableWrite()
{
	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
}

void Flash::Lock()
{
	SET_BIT(FLASH->CR,FLASH_CR_LOCK);
}

// CRのロックを解除
void Flash::UnLock()
{
	// KEYRレジスタに連続して書き込む
	WRITE_REG(FLASH->KEYR,LockKey::Key01);
	WRITE_REG(FLASH->KEYR,LockKey::Key02);
}

uint32_t Flash::ErasePage(uint32_t Page)
{
	// 消去するページを指定
	uint32_t DeletePage = Page << FLASH_CR_PNB_Pos;

	WaitBusy();

	if(GetLockFlag() != 0)
	{
		UnLock();
	}

	if(CheckErrorFlag() != 0)
	{
		ClearErrorFlag();
	}

	MODIFY_REG(FLASH->CR,FLASH_CR_PNB,DeletePage);

	EnableDelete();
	StartDelete();

	WaitBusy();

	DisableDelete();

	return 0;
}

uint32_t Flash::WriteDoubleWord(uint32_t address,uint64_t data)
{
	__IO uint32_t* pAddress = (__IO uint32_t*)address;

	WaitBusy();

	if(GetLockFlag() != 0)
	{
		UnLock();
	}

	if(CheckErrorFlag() != 0)
	{
		ClearErrorFlag();
	}

	EnableWrite();

	pAddress[0] = (uint32_t)data;
	pAddress[1] = (uint32_t)(data >> 32);

	WaitBusy();

	DisableWrite();

	return 0;
}

void Flash::ReadDoubleWord(uint32_t address,FlashData* data)
{
	__IO uint32_t* pAddress = (__IO uint32_t*)address;

	data->Low = pAddress[0];
	data->High = pAddress[1];
}

