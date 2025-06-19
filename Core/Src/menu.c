#include "menu.h"
#include "encoder.h"
#include "adc.h"
#include "gpio.h"
#include "flash.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

	//首次进入页面标志
	static bool first_enter = true;		

	//电流、电容1-4电压、总电压
	 extern float VI,V1,V2,V3,V4,VV;

	//脉冲1、脉冲2、脉冲间隔、触发时间、总点焊次数、开机点焊次数、充电控制电压，均衡控制电压，内阻（电流校准）
	uint16_t WELD_PULSE1,WELD_PULSE2,PULSE,Waiting_time,Total_times,Times = 0;
	float CCV,BCV,res;

// 菜单项定义
static MenuItem menuItems[MENU_ITEMS_COUNT] = {
	//   名称      值    最小值 最大值 步进  精度 单位
    {"脉冲①  ", 11.0f, 0.0f, 99.0f, 1.0f, 0,"ms"},
    {"脉冲②  ", 22.0f, 0.0f, 99.0f, 1.0f, 0,"ms"},
    {"脉冲间隔", 3.0f, 0.0f, 99.0f, 1.0f, 0,"ms"},
    {"触发时间", 2.0f, 0.0f, 99.0f, 1.0f, 0,"s"},
		{"电流校准", 3.4f, 0.0f, 99.0f, 0.1f, 1,"mR"},
    {"充电电压", 10.4f, 1.0f, 12.0f, 0.01f, 2,"V"},
    {"均衡电压", 2.70f, 1.0f, 3.0f, 0.01f, 2,"V"},
    {"保存", 0, 0, 0, 0, 0,0},
    {"退出", 0, 0, 0, 0, 0,0}
};

static MenuState currentState = MAIN_SCREEN;
static uint8_t currentPage = 0;
static uint8_t selectedItem = 0;
static uint8_t editingItem = 0;


// 菜单初始化
void Menu_Init(void) {
    // 这里可以添加从EEPROM加载保存的设置
			Data_reading();
}

// 更新菜单显示
void Menu_Update(void) {
    switch(currentState) {
        case MAIN_SCREEN:
            Menu_DrawMainScreen();
            break;
        case MENU_SCREEN:
            Menu_DrawMenuScreen();
            break;
        case EDIT_SCREEN:
            Menu_DrawEditScreen();
            break;
    }
}

// 处理编码器输入
void Menu_ProcessEncoder(int8_t direction, uint8_t key_state) {
    if(key_state == KEY_LONG_PRESS) {
        if(currentState == MAIN_SCREEN) {
            currentState = MENU_SCREEN;		//进入参数页面
						first_enter = true;
            selectedItem = 0;
            currentPage = 0;
            Menu_Update();
        } else {
            currentState = MAIN_SCREEN;
						Data_reading();
            Menu_Update();
        }
        return;
    }
    
    switch(currentState) {
        case MENU_SCREEN:
            if(key_state == KEY_SHORT_PRESS) {
                if(selectedItem == MENU_ITEMS_COUNT - 2) { // 保存
                    Menu_SaveSettings();
										//保存成功
                    currentState = MAIN_SCREEN;
                } 
                else if(selectedItem == MENU_ITEMS_COUNT - 1) { // 退出
										Data_reading();
										//数据未保存，退出
                    currentState = MAIN_SCREEN;
                } 
                else {
                    editingItem = 1;
                    currentState = EDIT_SCREEN;		//进入编辑页面
										first_enter = true;
                }
                Menu_Update();
            } 
            else if(direction != 0) {
                selectedItem += direction;
                
                // 边界检查
                if(selectedItem < 0) selectedItem = MENU_ITEMS_COUNT - 1;
                if(selectedItem >= MENU_ITEMS_COUNT) selectedItem = 0;
                
                // 更新页面
                currentPage = selectedItem / ITEMS_PER_PAGE;
                Menu_Update();
            }
            break;
            
        case EDIT_SCREEN:
            if(key_state == KEY_SHORT_PRESS) {
                editingItem = 0;
                currentState = MENU_SCREEN;		//退出编辑页面回到参数页面
								first_enter = true;
								Menu_Update();
            } 
            else if(direction != 0) {
                uint8_t itemIdx = selectedItem;
                menuItems[itemIdx].value += direction * menuItems[itemIdx].step;
                
                // 边界检查
                if(menuItems[itemIdx].value < menuItems[itemIdx].min) {
                    menuItems[itemIdx].value = menuItems[itemIdx].min;
                }
                if(menuItems[itemIdx].value > menuItems[itemIdx].max) {
                    menuItems[itemIdx].value = menuItems[itemIdx].max;
                }
                Menu_Update();
            }
            break;
            
        default:
            break;
    }
}

// 绘制主屏幕
void Menu_DrawMainScreen(void) {
	Data_reading();
	
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);			

	LCD_Fill(0, 0, 52, 39,Light_yellow);//首页，x1,y1起始坐标 x2,y2终止坐标 color矩形的颜色
	LCD_Fill(53, 0,104 ,39,Light_green);
	LCD_Fill(106, 0,160,39,Mint_Green);
	LCD_Fill(0, 40, 52, 79,Light_blue);
	LCD_Fill(53, 40,104 ,79,Light_pink);
	LCD_Fill(106, 40,160,79,Light_purple);
	
//	LCD_ShowChinese(3,3,"脉冲①",DARKBLUE,Light_yellow,16,0);//显示汉字串
	LCD_ShowAuto(3,3,"脉冲 1",DARKBLUE,Light_yellow,16,0);//显示汉字串
//	LCD_ShowString(38,3,"1",WHITE,BROWN,16,0);//显示字符串
	LCD_ShowChinese(64,3,"间隔",DARKBLUE,Light_green,16,0);//显示汉字串
	LCD_ShowChinese(109,3,"脉冲②",DARKBLUE,Mint_Green,16,0);//显示汉字串
//	LCD_ShowString(144,3,"2",WHITE,GRAY,16,0);//显示字符串  
	LCD_ShowChinese(11,43,"等待",DARKBLUE,Light_blue,16,0);//显示汉字串
	LCD_ShowChinese(56,43,"总次数",DARKBLUE,Light_pink,16,0);//显示汉字串
	LCD_ShowChinese(117,43,"次数",DARKBLUE,Light_purple,16,0);//显示汉字串
	
	LCD_Fill(0,86,LCD_W,LCD_H,BLACK);	
  LCD_ShowString(3,86,"V:",GRED,BLACK,16,0);//显示汉字串
	LCD_ShowChinese(64,86,"电流",GRED,BLACK,16,0);//显示汉字串
  LCD_ShowString(97,86,":",GRED,BLACK,16,0);//显示汉字串
	
	//在指定位置画一个矩形
	LCD_DrawRectangle(0, 109, 37, 127,WHITE);
	LCD_DrawRectangle(37, 113, 39, 123,WHITE);
	LCD_DrawRectangle(40, 109, 77, 127,WHITE);
	LCD_DrawRectangle(77, 113, 79, 123,WHITE);
	LCD_DrawRectangle(80, 109,117, 127,WHITE);
	LCD_DrawRectangle(117, 113,119, 123,WHITE);		
	LCD_DrawRectangle(120, 109,157, 127,WHITE);
	LCD_DrawRectangle(157, 113, 159, 123,WHITE);
	
}

// 绘制菜单屏幕
void Menu_DrawMenuScreen(void) {
    static uint8_t lastSelectedItem = 0xFF; // 初始值设为不可能的值
    static uint8_t lastPage = 0xFF;
//    static bool firstEnter = true;          // 首次进入标志
    
    uint8_t startIdx = currentPage * ITEMS_PER_PAGE;
    uint8_t endIdx = startIdx + ITEMS_PER_PAGE;
    if(endIdx > MENU_ITEMS_COUNT) endIdx = MENU_ITEMS_COUNT;
    
    // 首次进入或页面切换时完全重绘
    if(first_enter || lastPage != currentPage) {
        LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
        
        for(uint8_t i = startIdx; i < endIdx; i++) {
            uint8_t yPos = 8 + (i % ITEMS_PER_PAGE) * 25;
            
            // 绘制选中项背景
            if(i == selectedItem) {
                LCD_Fill(0, yPos - 2, LCD_W, yPos + 16, BLUE);
            }
            
            // 绘制菜单项
            if(i < MENU_ITEMS_COUNT - 2) { // 参数项
                char buf[36];
                char cnbuf[32];
                if(menuItems[i].decimal == 0) {
                    sprintf(buf, "%d %s", (int)menuItems[i].value, menuItems[i].unit);
                    sprintf(cnbuf, "%s", menuItems[i].name);
                } else if(menuItems[i].decimal == 1) {
                    sprintf(buf, "%.1f %s", menuItems[i].value, menuItems[i].unit);
                    sprintf(cnbuf, "%s", menuItems[i].name);
                } else {
                    sprintf(buf, "%.2f %s", menuItems[i].value, menuItems[i].unit);
                    sprintf(cnbuf, "%s", menuItems[i].name);
                }
                LCD_ShowChinese(10, yPos, (uint8_t *)cnbuf, 
                               (i == selectedItem) ? WHITE : BLACK, 
                               (i == selectedItem) ? BLUE : WHITE, 16, 0);
                LCD_ShowString(90, yPos, (uint8_t *)buf, 
                             (i == selectedItem) ? WHITE : BLACK, 
                             (i == selectedItem) ? BLUE : WHITE, 16, 0);
            } else { // 保存/退出项
                LCD_ShowChinese(10, yPos, (uint8_t *)menuItems[i].name, 
                             (i == selectedItem) ? WHITE : BLACK, 
                             (i == selectedItem) ? BLUE : WHITE, 16, 0);
            }
        }
        
        first_enter = false;
        lastPage = currentPage;
        lastSelectedItem = selectedItem;
        return;
    }
    
    // 局部刷新 - 仅当选中项变化时执行
    if(selectedItem != lastSelectedItem) {
        // 清除上一次选中的项目背景
        if(lastSelectedItem != 0xFF && lastSelectedItem >= startIdx && lastSelectedItem < endIdx) {
            uint8_t lastYPos = 8 + (lastSelectedItem % ITEMS_PER_PAGE) * 25;
            LCD_Fill(0, lastYPos - 2, LCD_W, lastYPos + 16, WHITE);
            
            // 重绘上一次选中的项目文本
            if(lastSelectedItem < MENU_ITEMS_COUNT - 2) { // 参数项
                char buf[36];
                char cnbuf[32];
                if(menuItems[lastSelectedItem].decimal == 0) {
                    sprintf(buf, "%d %s", (int)menuItems[lastSelectedItem].value, menuItems[lastSelectedItem].unit);
                    sprintf(cnbuf, "%s", menuItems[lastSelectedItem].name);
                } else if(menuItems[lastSelectedItem].decimal == 1) {
                    sprintf(buf, "%.1f %s", menuItems[lastSelectedItem].value, menuItems[lastSelectedItem].unit);
                    sprintf(cnbuf, "%s", menuItems[lastSelectedItem].name);
                } else {
                    sprintf(buf, "%.2f %s", menuItems[lastSelectedItem].value, menuItems[lastSelectedItem].unit);
                    sprintf(cnbuf, "%s", menuItems[lastSelectedItem].name);
                }
                LCD_ShowChinese(10, lastYPos, (uint8_t *)cnbuf, BLACK, WHITE, 16, 0);
                LCD_ShowString(90, lastYPos, (uint8_t *)buf, BLACK, WHITE, 16, 0);
            } else { // 保存/退出项
                LCD_ShowChinese(10, lastYPos, (uint8_t *)menuItems[lastSelectedItem].name, 
                             BLACK, WHITE, 16, 0);
            }
        }
        
        // 绘制当前选中的项目背景
        if(selectedItem >= startIdx && selectedItem < endIdx) {
            uint8_t yPos = 8 + (selectedItem % ITEMS_PER_PAGE) * 25;
            LCD_Fill(0, yPos - 2, LCD_W, yPos + 16, BLUE);
            
            // 绘制当前选中的项目文本
            if(selectedItem < MENU_ITEMS_COUNT - 2) { // 参数项
                char buf[36];
                char cnbuf[32];
                if(menuItems[selectedItem].decimal == 0) {
                    sprintf(buf, "%d %s", (int)menuItems[selectedItem].value, menuItems[selectedItem].unit);
                    sprintf(cnbuf, "%s", menuItems[selectedItem].name);
                } else if(menuItems[selectedItem].decimal == 1) {
                    sprintf(buf, "%.1f %s", menuItems[selectedItem].value, menuItems[selectedItem].unit);
                    sprintf(cnbuf, "%s", menuItems[selectedItem].name);
                } else {
                    sprintf(buf, "%.2f %s", menuItems[selectedItem].value, menuItems[selectedItem].unit);
                    sprintf(cnbuf, "%s", menuItems[selectedItem].name);
                }
                LCD_ShowChinese(10, yPos, (uint8_t *)cnbuf, WHITE, BLUE, 16, 0);
                LCD_ShowString(90, yPos, (uint8_t *)buf, WHITE, BLUE, 16, 0);
            } else { // 保存/退出项
                LCD_ShowChinese(10, yPos, (uint8_t *)menuItems[selectedItem].name, 
                             WHITE, BLUE, 16, 0);
            }
        }
        
        lastSelectedItem = selectedItem;
    }
    
    // 如果菜单项的值有变化，需要额外处理重绘
    // 这里可以添加值变化的检测和重绘逻辑
}

// 绘制编辑屏幕
void Menu_DrawEditScreen(void) {
	
    
    char buf[32];
    uint8_t itemIdx = selectedItem;
    
    if(first_enter) {
        LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
        first_enter = false;
				LCD_ShowChinese(10, 40, (uint8_t *)menuItems[itemIdx].name, BLACK, WHITE, 16, 0);

		}	
	
    if(itemIdx < MENU_ITEMS_COUNT - 2) { // 参数项
        if(menuItems[itemIdx].decimal == 0) {
            sprintf(buf, "%s %d %s", menuItems[itemIdx].name, (int)menuItems[itemIdx].value, menuItems[itemIdx].unit);
        } else if(menuItems[itemIdx].decimal == 1) {
            sprintf(buf, "%s %.1f %s", menuItems[itemIdx].name, menuItems[itemIdx].value, menuItems[itemIdx].unit);
        } else {
            sprintf(buf, "%s %.2f %s", menuItems[itemIdx].name, menuItems[itemIdx].value, menuItems[itemIdx].unit);
        }
        
        // 绘制参数名称
//        LCD_ShowChinese(10, 40, (uint8_t *)menuItems[itemIdx].name, BLACK, WHITE, 16, 0);
        
        // 高亮显示参数值
        LCD_Fill(90, 40,LCD_W, 56, BLUE);
        LCD_ShowString(90, 40, (uint8_t *)(buf + strlen(menuItems[itemIdx].name)), WHITE, BLUE, 16, 0);
        
    }
		
}

// 保存设置到EEPROM
void Menu_SaveSettings(void) {
    // 这里添加保存到EEPROM的代码
    // 示例: 保存到Flash或外部EEPROM
	FLASH_ErasePage();
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR, menuItems[0].value);	        //脉冲1
  FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+2, menuItems[1].value);	      //脉冲2
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+4, menuItems[2].value);	      //脉冲间隔
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+6, menuItems[3].value);	      //触发等待时间
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+8, menuItems[4].value*100);	  //电流校准系数（内阻）
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+10, menuItems[5].value*100);	//充电控制电压
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+12, menuItems[6].value*100);	//均衡放电控制电压
}

// 参数读取
void Data_reading(void){
				//初始化读取数值
			 menuItems[0].value = FLASH_ReadHalfWord(FLASH_USER_START_ADDR);         //脉冲1
			 WELD_PULSE1 = FLASH_ReadHalfWord(FLASH_USER_START_ADDR);         //脉冲1	
	
			 menuItems[1].value =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+2);       //脉冲2
			 WELD_PULSE2 =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+2);       //脉冲2	
	
			 menuItems[2].value =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+4);       //间隔时间
			 PULSE =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+4);       //间隔时间	
	
			 menuItems[3].value =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+6);       //触发等待时间
			 Waiting_time =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+6);       //触发等待时间
	
			 menuItems[4].value = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+8) / 100;   //电流校准
			 res = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+8) / 100;   //电流校准
	
			 menuItems[5].value =(float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+10)/100;  //充电控制电压
			 CCV = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+10) / 100;  //充电控制电压
	
			 menuItems[6].value = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+12)/100;  //均衡放电控制电压
			 BCV = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+12) / 100;  //均衡放电控制电压
			 
		 	Total_times=FLASH_ReadHalfWord(PulseCount_START_ADDR); //总点焊次数
	
}

//实时显示函数
void display_update(void) {

		if(currentState == MAIN_SCREEN) {
				uint8_t temp[16];
				Averages();
			
			//在指定位置画一个矩形
			LCD_DrawRectangle(0, 109, 37, 127,WHITE);
			LCD_DrawRectangle(37, 113, 39, 123,WHITE);
			LCD_DrawRectangle(40, 109, 77, 127,WHITE);
			LCD_DrawRectangle(77, 113, 79, 123,WHITE);
			LCD_DrawRectangle(80, 109,117, 127,WHITE);
			LCD_DrawRectangle(117, 113,119, 123,WHITE);		
			LCD_DrawRectangle(120, 109,157, 127,WHITE);
			LCD_DrawRectangle(157, 113, 159, 123,WHITE);	 

			sprintf((char*)temp,"%.2f", V1);
			if(V1 >= BCV){
			LCD_ShowString(4,110,temp,RED,BLACK,16,0);}
			else {LCD_ShowString(4,110,temp,GREEN,BLACK,16,0);}
			
			sprintf((char*)temp,"%.2f",V2);
			if(V2 >= BCV){
			LCD_ShowString(44,110,temp,RED,BLACK ,16,0);}
			else {LCD_ShowString(44,110,temp,GREEN,BLACK ,16,0);}
			
			sprintf((char*)temp,"%.2f",V3);
			if(V3 >= BCV){
			LCD_ShowString(84,110,temp,RED,BLACK,16,0);}
			else {LCD_ShowString(84,110,temp,GREEN,BLACK,16,0);}
			
			sprintf((char*)temp,"%.2f",V4);
			if(V4 >= BCV){
			LCD_ShowString(124,110,temp,RED,BLACK,16,0);}
			else {LCD_ShowString(124,110,temp,GREEN,BLACK,16,0);}
				
			sprintf((char*)temp,"%.1fV",VV);
			if(VV <= CCV){
			LCD_ShowString(20,86,temp,RED,BLACK,16,0);}
			else {LCD_ShowString(20,86,temp,GREEN,BLACK,16,0);}
			
//			sprintf((char*)temp,"%.1fA",VI);
//			LCD_ShowString(105,86,temp,GREEN,BLACK,16,0);
			
			sprintf((char*)temp,"%dms",WELD_PULSE1);
			LCD_ShowString(10,22,temp,DARKBLUE,Light_yellow,16,0);			
			sprintf((char*)temp,"%dms",PULSE);
			LCD_ShowString(62,22,temp,DARKBLUE,Light_green,16,0);			
			sprintf((char*)temp,"%dms",WELD_PULSE2);
			LCD_ShowString(114,22,temp,DARKBLUE,Mint_Green,16,0);			
			sprintf((char*)temp,"%dS",Waiting_time);
			LCD_ShowString(15,60,temp,DARKBLUE,Light_blue,16,0);			
			sprintf((char*)temp,"%d",Total_times + Times);
			LCD_ShowString(60,60,temp,DARKBLUE,Light_pink,16,0);			
			sprintf((char*)temp,"%d",Times);
			LCD_ShowString(114,60,temp,DARKBLUE,Light_purple,16,0);
      } 
}

// 获取currentState，供main文件使用
MenuState getCurrentState() {
    return currentState;
}