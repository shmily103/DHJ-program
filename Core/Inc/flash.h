#ifndef __FLASH_H
#define __FLASH_H
#include <stdint.h>
#include "main.h"  

#define STM32_FLASH_SIZE 	64 	 	//��ѡSTM32��FLASH������С(��λΪK)
    #if     STM32_FLASH_SIZE < 256      //����������С
    #define STM_SECTOR_SIZE     1024    //1K�ֽ�
    #else 
    #define STM_SECTOR_SIZE	    2048    //2K�ֽ�
#endif
 
#define STM32_FLASH_BASE            0x08000000 		//STM32 FLASH����ʼ��ַ
#define FLASH_USER_START_ADDR   ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 61 ) //дFlash�ĵ�ַ������ӵ�61ҳ��ʼ
#define FLASH_USER_END_ADDR     ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 62 ) //дFlash�ĵ�ַ�������Ե�62ҳ����

#define PulseCount_START_ADDR   ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 63 ) //дFlash�ĵ�ַ������ӵ�63ҳ��ʼ
#define PulseCount_END_ADDR     ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 64 ) //дFlash�ĵ�ַ�������Ե�64ҳ����

uint32_t FLASH_ReadWord(uint32_t Address);
uint16_t FLASH_ReadHalfWord(uint32_t Address);
uint8_t FLASH_ReadByte(uint32_t Address);

void FLASH_EraseAllPages(void);
void FLASH_ErasePage(void);
void PulseCount_ErasePage(void);

void FLASH_ProgramWord(uint32_t Address, uint32_t Data);
void FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);

#endif
