#include "flash.h"

/**
  * 函    数：FLASH读取一个32位的字
  * 参    数：Address 要读取数据的字地址
  * 返 回 值：指定地址下的数据
  */
uint32_t FLASH_ReadWord(uint32_t Address)
{
	return *((__IO uint32_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个16位的半字
  * 参    数：Address 要读取数据的半字地址
  * 返 回 值：指定地址下的数据
  */
uint16_t FLASH_ReadHalfWord(uint32_t Address)
{
	return *((__IO uint16_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个8位的字节
  * 参    数：Address 要读取数据的字节地址
  * 返 回 值：指定地址下的数据
  */
uint8_t FLASH_ReadByte(uint32_t Address)
{
	return *((__IO uint8_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH全擦除
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，FLASH的所有页都会被擦除，包括程序文件本身，擦除后，程序将不复存在
  */
void FLASH_EraseAllPages(void)
{
	HAL_FLASH_Unlock();					//解锁
	FLASH_EraseAllPages();			//全擦除
	HAL_FLASH_Lock();					//加锁
}

/**
  * 函    数：FLASH页擦除
  * 参    数：PageAddress 要擦除页的页地址
  * 返 回 值：无
  */
void FLASH_ErasePage(void)
{
	uint32_t SectorError=0;
	FLASH_EraseInitTypeDef FLASH_EraseInit;
	
	HAL_FLASH_Unlock();					//解锁
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	
	FLASH_EraseInit.TypeErase=FLASH_TYPEERASE_PAGES;
	FLASH_EraseInit.PageAddress=FLASH_USER_START_ADDR;
	FLASH_EraseInit.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / STM_SECTOR_SIZE;
	
	HAL_FLASHEx_Erase(&FLASH_EraseInit,&SectorError);	//页擦除
	
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	HAL_FLASH_Lock();					//加锁
}

void PulseCount_ErasePage(void)
{
	uint32_t SectorError=0;
	FLASH_EraseInitTypeDef FLASH_EraseInit;
	
	HAL_FLASH_Unlock();					//解锁
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	
	FLASH_EraseInit.TypeErase=FLASH_TYPEERASE_PAGES;
	FLASH_EraseInit.PageAddress=PulseCount_START_ADDR;
	FLASH_EraseInit.NbPages = (PulseCount_END_ADDR - PulseCount_START_ADDR) / STM_SECTOR_SIZE;
	
	HAL_FLASHEx_Erase(&FLASH_EraseInit,&SectorError);	//页擦除
	
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	HAL_FLASH_Lock();					//加锁
}

/**
  * 函    数：FLASH编程字
  * 参    数：Address 要写入数据的字地址
  * 参    数：Data 要写入的32位数据
  * 返 回 值：无
  */
void FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
	HAL_FLASH_Unlock();							//解锁
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Address, Data);		//编程字
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	HAL_FLASH_Lock();							//加锁
}

/**
  * 函    数：FLASH编程半字
  * 参    数：Address 要写入数据的半字地址
  * 参    数：Data 要写入的16位数据
  * 返 回 值：无
  */
void FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
	HAL_FLASH_Unlock();							//解锁
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Address, Data);		//编程半字
	FLASH_WaitForLastOperation(HAL_FLASH_TIMEOUT_VALUE);   //等待完成
	HAL_FLASH_Lock();							//加锁
}
