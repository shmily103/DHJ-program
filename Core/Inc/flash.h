#ifndef __FLASH_H
#define __FLASH_H
#include <stdint.h>
#include "main.h"  

#define STM32_FLASH_SIZE 	64 	 	//所选STM32的FLASH容量大小(单位为K)
    #if     STM32_FLASH_SIZE < 256      //设置扇区大小
    #define STM_SECTOR_SIZE     1024    //1K字节
    #else 
    #define STM_SECTOR_SIZE	    2048    //2K字节
#endif
 
#define STM32_FLASH_BASE            0x08000000 		//STM32 FLASH的起始地址
#define FLASH_USER_START_ADDR   ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 61 ) //写Flash的地址，这里从第61页开始
#define FLASH_USER_END_ADDR     ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 62 ) //写Flash的地址，这里以第62页结束

#define PulseCount_START_ADDR   ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 63 ) //写Flash的地址，这里从第63页开始
#define PulseCount_END_ADDR     ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 64 ) //写Flash的地址，这里以第64页结束

uint32_t FLASH_ReadWord(uint32_t Address);
uint16_t FLASH_ReadHalfWord(uint32_t Address);
uint8_t FLASH_ReadByte(uint32_t Address);

void FLASH_EraseAllPages(void);
void FLASH_ErasePage(void);
void PulseCount_ErasePage(void);

void FLASH_ProgramWord(uint32_t Address, uint32_t Data);
void FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);

#endif
