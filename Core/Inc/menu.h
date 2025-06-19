#ifndef __MENU_H
#define __MENU_H

#include "lcd_init.h"
#include "lcd.h"

extern 	uint16_t WELD_PULSE1,WELD_PULSE2,PULSE,Waiting_time,Total_times,Times;
extern	float CCV,BCV,res;

#define MENU_ITEMS_COUNT 9
#define ITEMS_PER_PAGE 5

typedef struct {
    char name[16];      // 参数名称
    float value;        // 参数值
    float min;          // 最小值
    float max;          // 最大值
    float step;         // 步进值
    uint8_t decimal;    // 小数位数
		char unit[4];			// 单位
} MenuItem;


// 菜单状态
typedef enum {
    MAIN_SCREEN,
    MENU_SCREEN,
    EDIT_SCREEN
} MenuState;

void Menu_Init(void);
void Menu_Update(void);
void Menu_ProcessEncoder(int8_t direction, uint8_t key_state);
void Menu_DrawMainScreen(void);
void Menu_DrawMenuScreen(void);
void Menu_DrawEditScreen(void);
void Menu_SaveSettings(void);
void Data_reading(void);
void display_update(void);
MenuState getCurrentState();          //获取currentState，供main文件使用
#endif