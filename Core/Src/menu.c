#include "menu.h"
#include "encoder.h"
#include "adc.h"
#include "gpio.h"
#include "flash.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

	//�״ν���ҳ���־
	static bool first_enter = true;		

	//����������1-4��ѹ���ܵ�ѹ
	 extern float VI,V1,V2,V3,V4,VV;

	//����1������2��������������ʱ�䡢�ܵ㺸�����������㺸�����������Ƶ�ѹ��������Ƶ�ѹ�����裨����У׼��
	uint16_t WELD_PULSE1,WELD_PULSE2,PULSE,Waiting_time,Total_times,Times = 0;
	float CCV,BCV,res;

// �˵����
static MenuItem menuItems[MENU_ITEMS_COUNT] = {
	//   ����      ֵ    ��Сֵ ���ֵ ����  ���� ��λ
    {"�����  ", 11.0f, 0.0f, 99.0f, 1.0f, 0,"ms"},
    {"�����  ", 22.0f, 0.0f, 99.0f, 1.0f, 0,"ms"},
    {"������", 3.0f, 0.0f, 99.0f, 1.0f, 0,"ms"},
    {"����ʱ��", 2.0f, 0.0f, 99.0f, 1.0f, 0,"s"},
		{"����У׼", 3.4f, 0.0f, 99.0f, 0.1f, 1,"mR"},
    {"����ѹ", 10.4f, 1.0f, 12.0f, 0.01f, 2,"V"},
    {"�����ѹ", 2.70f, 1.0f, 3.0f, 0.01f, 2,"V"},
    {"����", 0, 0, 0, 0, 0,0},
    {"�˳�", 0, 0, 0, 0, 0,0}
};

static MenuState currentState = MAIN_SCREEN;
static uint8_t currentPage = 0;
static uint8_t selectedItem = 0;
static uint8_t editingItem = 0;


// �˵���ʼ��
void Menu_Init(void) {
    // ���������Ӵ�EEPROM���ر��������
			Data_reading();
}

// ���²˵���ʾ
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

// �������������
void Menu_ProcessEncoder(int8_t direction, uint8_t key_state) {
    if(key_state == KEY_LONG_PRESS) {
        if(currentState == MAIN_SCREEN) {
            currentState = MENU_SCREEN;		//�������ҳ��
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
                if(selectedItem == MENU_ITEMS_COUNT - 2) { // ����
                    Menu_SaveSettings();
										//����ɹ�
                    currentState = MAIN_SCREEN;
                } 
                else if(selectedItem == MENU_ITEMS_COUNT - 1) { // �˳�
										Data_reading();
										//����δ���棬�˳�
                    currentState = MAIN_SCREEN;
                } 
                else {
                    editingItem = 1;
                    currentState = EDIT_SCREEN;		//����༭ҳ��
										first_enter = true;
                }
                Menu_Update();
            } 
            else if(direction != 0) {
                selectedItem += direction;
                
                // �߽���
                if(selectedItem < 0) selectedItem = MENU_ITEMS_COUNT - 1;
                if(selectedItem >= MENU_ITEMS_COUNT) selectedItem = 0;
                
                // ����ҳ��
                currentPage = selectedItem / ITEMS_PER_PAGE;
                Menu_Update();
            }
            break;
            
        case EDIT_SCREEN:
            if(key_state == KEY_SHORT_PRESS) {
                editingItem = 0;
                currentState = MENU_SCREEN;		//�˳��༭ҳ��ص�����ҳ��
								first_enter = true;
								Menu_Update();
            } 
            else if(direction != 0) {
                uint8_t itemIdx = selectedItem;
                menuItems[itemIdx].value += direction * menuItems[itemIdx].step;
                
                // �߽���
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

// ��������Ļ
void Menu_DrawMainScreen(void) {
	Data_reading();
	
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);			

	LCD_Fill(0, 0, 52, 39,Light_yellow);//��ҳ��x1,y1��ʼ���� x2,y2��ֹ���� color���ε���ɫ
	LCD_Fill(53, 0,104 ,39,Light_green);
	LCD_Fill(106, 0,160,39,Mint_Green);
	LCD_Fill(0, 40, 52, 79,Light_blue);
	LCD_Fill(53, 40,104 ,79,Light_pink);
	LCD_Fill(106, 40,160,79,Light_purple);
	
//	LCD_ShowChinese(3,3,"�����",DARKBLUE,Light_yellow,16,0);//��ʾ���ִ�
	LCD_ShowAuto(3,3,"���� 1",DARKBLUE,Light_yellow,16,0);//��ʾ���ִ�
//	LCD_ShowString(38,3,"1",WHITE,BROWN,16,0);//��ʾ�ַ���
	LCD_ShowChinese(64,3,"���",DARKBLUE,Light_green,16,0);//��ʾ���ִ�
	LCD_ShowChinese(109,3,"�����",DARKBLUE,Mint_Green,16,0);//��ʾ���ִ�
//	LCD_ShowString(144,3,"2",WHITE,GRAY,16,0);//��ʾ�ַ���  
	LCD_ShowChinese(11,43,"�ȴ�",DARKBLUE,Light_blue,16,0);//��ʾ���ִ�
	LCD_ShowChinese(56,43,"�ܴ���",DARKBLUE,Light_pink,16,0);//��ʾ���ִ�
	LCD_ShowChinese(117,43,"����",DARKBLUE,Light_purple,16,0);//��ʾ���ִ�
	
	LCD_Fill(0,86,LCD_W,LCD_H,BLACK);	
  LCD_ShowString(3,86,"V:",GRED,BLACK,16,0);//��ʾ���ִ�
	LCD_ShowChinese(64,86,"����",GRED,BLACK,16,0);//��ʾ���ִ�
  LCD_ShowString(97,86,":",GRED,BLACK,16,0);//��ʾ���ִ�
	
	//��ָ��λ�û�һ������
	LCD_DrawRectangle(0, 109, 37, 127,WHITE);
	LCD_DrawRectangle(37, 113, 39, 123,WHITE);
	LCD_DrawRectangle(40, 109, 77, 127,WHITE);
	LCD_DrawRectangle(77, 113, 79, 123,WHITE);
	LCD_DrawRectangle(80, 109,117, 127,WHITE);
	LCD_DrawRectangle(117, 113,119, 123,WHITE);		
	LCD_DrawRectangle(120, 109,157, 127,WHITE);
	LCD_DrawRectangle(157, 113, 159, 123,WHITE);
	
}

// ���Ʋ˵���Ļ
void Menu_DrawMenuScreen(void) {
    static uint8_t lastSelectedItem = 0xFF; // ��ʼֵ��Ϊ�����ܵ�ֵ
    static uint8_t lastPage = 0xFF;
//    static bool firstEnter = true;          // �״ν����־
    
    uint8_t startIdx = currentPage * ITEMS_PER_PAGE;
    uint8_t endIdx = startIdx + ITEMS_PER_PAGE;
    if(endIdx > MENU_ITEMS_COUNT) endIdx = MENU_ITEMS_COUNT;
    
    // �״ν����ҳ���л�ʱ��ȫ�ػ�
    if(first_enter || lastPage != currentPage) {
        LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
        
        for(uint8_t i = startIdx; i < endIdx; i++) {
            uint8_t yPos = 8 + (i % ITEMS_PER_PAGE) * 25;
            
            // ����ѡ�����
            if(i == selectedItem) {
                LCD_Fill(0, yPos - 2, LCD_W, yPos + 16, BLUE);
            }
            
            // ���Ʋ˵���
            if(i < MENU_ITEMS_COUNT - 2) { // ������
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
            } else { // ����/�˳���
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
    
    // �ֲ�ˢ�� - ����ѡ����仯ʱִ��
    if(selectedItem != lastSelectedItem) {
        // �����һ��ѡ�е���Ŀ����
        if(lastSelectedItem != 0xFF && lastSelectedItem >= startIdx && lastSelectedItem < endIdx) {
            uint8_t lastYPos = 8 + (lastSelectedItem % ITEMS_PER_PAGE) * 25;
            LCD_Fill(0, lastYPos - 2, LCD_W, lastYPos + 16, WHITE);
            
            // �ػ���һ��ѡ�е���Ŀ�ı�
            if(lastSelectedItem < MENU_ITEMS_COUNT - 2) { // ������
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
            } else { // ����/�˳���
                LCD_ShowChinese(10, lastYPos, (uint8_t *)menuItems[lastSelectedItem].name, 
                             BLACK, WHITE, 16, 0);
            }
        }
        
        // ���Ƶ�ǰѡ�е���Ŀ����
        if(selectedItem >= startIdx && selectedItem < endIdx) {
            uint8_t yPos = 8 + (selectedItem % ITEMS_PER_PAGE) * 25;
            LCD_Fill(0, yPos - 2, LCD_W, yPos + 16, BLUE);
            
            // ���Ƶ�ǰѡ�е���Ŀ�ı�
            if(selectedItem < MENU_ITEMS_COUNT - 2) { // ������
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
            } else { // ����/�˳���
                LCD_ShowChinese(10, yPos, (uint8_t *)menuItems[selectedItem].name, 
                             WHITE, BLUE, 16, 0);
            }
        }
        
        lastSelectedItem = selectedItem;
    }
    
    // ����˵����ֵ�б仯����Ҫ���⴦���ػ�
    // ����������ֵ�仯�ļ����ػ��߼�
}

// ���Ʊ༭��Ļ
void Menu_DrawEditScreen(void) {
	
    
    char buf[32];
    uint8_t itemIdx = selectedItem;
    
    if(first_enter) {
        LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
        first_enter = false;
				LCD_ShowChinese(10, 40, (uint8_t *)menuItems[itemIdx].name, BLACK, WHITE, 16, 0);

		}	
	
    if(itemIdx < MENU_ITEMS_COUNT - 2) { // ������
        if(menuItems[itemIdx].decimal == 0) {
            sprintf(buf, "%s %d %s", menuItems[itemIdx].name, (int)menuItems[itemIdx].value, menuItems[itemIdx].unit);
        } else if(menuItems[itemIdx].decimal == 1) {
            sprintf(buf, "%s %.1f %s", menuItems[itemIdx].name, menuItems[itemIdx].value, menuItems[itemIdx].unit);
        } else {
            sprintf(buf, "%s %.2f %s", menuItems[itemIdx].name, menuItems[itemIdx].value, menuItems[itemIdx].unit);
        }
        
        // ���Ʋ�������
//        LCD_ShowChinese(10, 40, (uint8_t *)menuItems[itemIdx].name, BLACK, WHITE, 16, 0);
        
        // ������ʾ����ֵ
        LCD_Fill(90, 40,LCD_W, 56, BLUE);
        LCD_ShowString(90, 40, (uint8_t *)(buf + strlen(menuItems[itemIdx].name)), WHITE, BLUE, 16, 0);
        
    }
		
}

// �������õ�EEPROM
void Menu_SaveSettings(void) {
    // ������ӱ��浽EEPROM�Ĵ���
    // ʾ��: ���浽Flash���ⲿEEPROM
	FLASH_ErasePage();
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR, menuItems[0].value);	        //����1
  FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+2, menuItems[1].value);	      //����2
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+4, menuItems[2].value);	      //������
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+6, menuItems[3].value);	      //�����ȴ�ʱ��
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+8, menuItems[4].value*100);	  //����У׼ϵ�������裩
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+10, menuItems[5].value*100);	//�����Ƶ�ѹ
	FLASH_ProgramHalfWord(FLASH_USER_START_ADDR+12, menuItems[6].value*100);	//����ŵ���Ƶ�ѹ
}

// ������ȡ
void Data_reading(void){
				//��ʼ����ȡ��ֵ
			 menuItems[0].value = FLASH_ReadHalfWord(FLASH_USER_START_ADDR);         //����1
			 WELD_PULSE1 = FLASH_ReadHalfWord(FLASH_USER_START_ADDR);         //����1	
	
			 menuItems[1].value =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+2);       //����2
			 WELD_PULSE2 =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+2);       //����2	
	
			 menuItems[2].value =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+4);       //���ʱ��
			 PULSE =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+4);       //���ʱ��	
	
			 menuItems[3].value =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+6);       //�����ȴ�ʱ��
			 Waiting_time =FLASH_ReadHalfWord(FLASH_USER_START_ADDR+6);       //�����ȴ�ʱ��
	
			 menuItems[4].value = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+8) / 100;   //����У׼
			 res = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+8) / 100;   //����У׼
	
			 menuItems[5].value =(float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+10)/100;  //�����Ƶ�ѹ
			 CCV = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+10) / 100;  //�����Ƶ�ѹ
	
			 menuItems[6].value = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+12)/100;  //����ŵ���Ƶ�ѹ
			 BCV = (float)FLASH_ReadHalfWord(FLASH_USER_START_ADDR+12) / 100;  //����ŵ���Ƶ�ѹ
			 
		 	Total_times=FLASH_ReadHalfWord(PulseCount_START_ADDR); //�ܵ㺸����
	
}

//ʵʱ��ʾ����
void display_update(void) {

		if(currentState == MAIN_SCREEN) {
				uint8_t temp[16];
				Averages();
			
			//��ָ��λ�û�һ������
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

// ��ȡcurrentState����main�ļ�ʹ��
MenuState getCurrentState() {
    return currentState;
}