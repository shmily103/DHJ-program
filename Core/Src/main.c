/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "lcd_init.h"
#include "lcd.h"
#include "flash.h"
#include "encoder.h"
#include "menu.h"
#include <stdio.h>
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
	//电流、电容1-4电压、总电压
	extern float VI,V1,V2,V3,V4,VV;
	//脉冲1、脉冲2、脉冲间隔、触发时间、总点焊次数、开机点焊次数、充电控制电压，均衡控制电压，内阻（电流校准）
	extern 	uint16_t WELD_PULSE1,WELD_PULSE2,PULSE,Waiting_time,Total_times,Times;
	extern	float CCV,BCV,res;	
	//临时数组
	uint8_t temp[16];

//	//点焊脉冲控制参数变量
volatile uint8_t pulse_state = 0;
volatile uint32_t pulse_counter = 0;
//TIM_HandleTypeDef htim2;
volatile uint32_t trigger_start_time = 0;  // 触发开始时间戳
volatile uint8_t trigger_active = 0;       // 触发激活标志
	
	//掉电检测引脚状态
	uint8_t lastSTState = 1; // 默认高电平
	
/* USER CODE END PTD */

/* Private function prototypes -----------------------------------------------*/
	void SystemClock_Config(void);
	//调用函数
	void MX_GPIO_Init(void);
	void MX_TIM2_Init(void);
	void MX_TIM4_Init(void);
	void MX_ADC1_Init(void);
	void initialize(void);	//初始化功能检查
	void PulseCounts(void);//脉冲计数，电流显示
	void Pulse(void);	//脉冲控制
	void Data_reading(void);	//参数读取
	void Charge_equalization(void);	//充电均衡控制
	void Power_off(void);//掉电保存点焊次数

//主函数
int main(void){

  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
	//初始化
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
	MX_TIM2_Init();
	MX_TIM4_Init();
	
	// 启动定时器
    HAL_TIM_Base_Start_IT(&htim2);
	
  /* USER CODE BEGIN 2 */
	Data_reading();	//Flash参数读取
	LCD_Init();	//显示初始化
	initialize();	//初始化功能检查
	ADC_StartSampling();	//ADC开启
	Encoder_Init();	//EC11编码器初始化
	Menu_Init();	//菜单初始化
	Menu_DrawMainScreen();	//绘制页面
	Charge_equalization();	//充电均衡控制
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//电压读取
		Averages();
		//充电、均衡控制
		Charge_equalization();
		//脉冲控制
		Pulse();
		//掉电保存点焊次数
		Power_off();
	
		// 扫描按键
    Encoder_KeyScan();        
    // 获取编码器方向和按键状态
    int8_t direction = Encoder_GetDirection();
    uint8_t key_state = Encoder_GetKeyState();
    // 处理菜单输入
    if(direction != 0 || key_state != KEY_NO_PRESS) {
       Menu_ProcessEncoder(direction, key_state);
       }
    else {
			 display_update();	
				}
		
  }
}

//初始化准备
void initialize(void){

	//欢迎屏
	LCD_Fill(0,0,LCD_W,LCD_H,LBBLUE);
	LCD_ShowChinese(30,30,"点焊机",RED ,LBBLUE,32,0);
	LCD_ShowString(20,90,"Design By Panzy",GREEN,LBBLUE,16,0);
	HAL_Delay(1500);
	
	//端口检测
	BUZZER_1;
  DISCHARGE_CAP1_1;
  DISCHARGE_CAP2_1;
  DISCHARGE_CAP3_1;
  DISCHARGE_CAP4_1;
  WELD_PULSE_1;
  CHARGE_CTRL_1;
	HAL_Delay(100);
	BUZZER_0;
	DISCHARGE_CAP1_0;
	DISCHARGE_CAP2_0;
	DISCHARGE_CAP3_0;
  DISCHARGE_CAP4_0;
  WELD_PULSE_0;
  CHARGE_CTRL_0;
	
	sprintf((char*)temp,"%d",Total_times);
	LCD_ShowString(60,60,temp,DARKBLUE,Light_pink,16,0);			
	sprintf((char*)temp,"%d",Times);
	LCD_ShowString(114,60,temp,DARKBLUE,Light_purple,16,0);
}

//充电均衡控制
void Charge_equalization(void){
			Averages();

			if(V1 >= BCV){
			DISCHARGE_CAP1_1;}
			else {DISCHARGE_CAP1_0;}
			
			if(V2 >= BCV){
			DISCHARGE_CAP2_1;}
			else {DISCHARGE_CAP2_0;}
			
			if(V3 >= BCV){
			DISCHARGE_CAP3_1;}
			else {DISCHARGE_CAP3_0;}
			
			if(V4 >= BCV){
			DISCHARGE_CAP4_1;}
			else {DISCHARGE_CAP4_0;}
				
			if(V1 >= BCV || V2 >= BCV || V3 >= BCV || V4 >= BCV && VV <= CCV){
			CHARGE_CTRL_1;}
			else {CHARGE_CTRL_0;}
	
}


//点焊脉冲控制
void Pulse(void) {
				if (trigger_active && (HAL_GetTick() - trigger_start_time) >= (Waiting_time * 1000))
						{
								trigger_active = 0;  // 清除触发标志
        
        // 检查是否需要输出脉冲
        if (WELD_PULSE1 > 0 || WELD_PULSE2 > 0) {
            Times++;
            
            // 第一个脉冲
            if (WELD_PULSE1 > 0) {
                BUZZER_1;
                WELD_PULSE_1;
								float VI = Get_ADC(4);
                pulse_state = 1;
                pulse_counter = WELD_PULSE1;
                while (pulse_counter > 0);
                WELD_PULSE_0;
                BUZZER_0;
            }
            
            // 脉冲间隔
            if (WELD_PULSE2 > 0) {
                HAL_Delay(PULSE);
                
                // 第二个脉冲
                BUZZER_1;
                WELD_PULSE_1;
								float VI = Get_ADC(4);
                pulse_state = 2;
                pulse_counter = WELD_PULSE2;
                while (pulse_counter > 0);
                WELD_PULSE_0;
                BUZZER_0;
            }
            
            MenuState state = getCurrentState();
            if(state == MAIN_SCREEN) {
                sprintf((char*)temp,"%d",Total_times + Times);
                LCD_ShowString(60,60,temp,DARKBLUE,Light_pink,16,0);			
                sprintf((char*)temp,"%d",Times);
                LCD_ShowString(114,60,temp,DARKBLUE,Light_purple,16,0);

                LCD_Fill(105,86,LCD_W,102,BLACK);
                sprintf((char*)temp,"%.0fA",VI / res * 1000);
                LCD_ShowString(105,86,temp,GREEN,BLACK,16,0);
            }
        }
    }
    HAL_Delay(50);
}


//掉电保存点焊次数
void Power_off(void){
	static uint8_t executed = 0;  // 静态标志变量，初始为0（未执行）
	// 读取触发引脚状态
	uint8_t STState = HAL_GPIO_ReadPin(ST_GPIO_Port, ST_Pin);
	if (!executed && lastSTState == 1 && STState == 0 && (Total_times + Times) > Total_times)
	{
		PulseCount_ErasePage();
		FLASH_ProgramHalfWord(PulseCount_START_ADDR, Total_times + Times);	//总点焊次数
		executed = 1;  // 设置标志为已执行

	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        if (pulse_counter > 0) {
            pulse_counter--;
        }
    }
}

// PB10外部中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == TRIGGER_Pin) {
        if (HAL_GPIO_ReadPin(TRIGGER_GPIO_Port, TRIGGER_Pin) == GPIO_PIN_RESET) {
            trigger_start_time = HAL_GetTick();
            trigger_active = 1;
        }
				else
        {
            // 检测到高电平，取消触发
            trigger_active = 0;
        }
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
