/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

#define BUZZER_0 			 	   HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)// ÷√0
#define BUZZER_1   				 HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)//÷√1
#define DISCHARGE_CAP4_0   HAL_GPIO_WritePin(DISCHARGE_CAP1_GPIO_Port, DISCHARGE_CAP1_Pin, GPIO_PIN_RESET)// ÷√0
#define DISCHARGE_CAP4_1   HAL_GPIO_WritePin(DISCHARGE_CAP1_GPIO_Port, DISCHARGE_CAP1_Pin, GPIO_PIN_SET)//÷√1
#define DISCHARGE_CAP3_0   HAL_GPIO_WritePin(DISCHARGE_CAP2_GPIO_Port, DISCHARGE_CAP2_Pin, GPIO_PIN_RESET)// ÷√0
#define DISCHARGE_CAP3_1   HAL_GPIO_WritePin(DISCHARGE_CAP2_GPIO_Port, DISCHARGE_CAP2_Pin, GPIO_PIN_SET)//÷√1
#define DISCHARGE_CAP2_0   HAL_GPIO_WritePin(DISCHARGE_CAP3_GPIO_Port, DISCHARGE_CAP3_Pin, GPIO_PIN_RESET)// ÷√0
#define DISCHARGE_CAP2_1   HAL_GPIO_WritePin(DISCHARGE_CAP3_GPIO_Port, DISCHARGE_CAP3_Pin, GPIO_PIN_SET)//÷√1
#define DISCHARGE_CAP1_0   HAL_GPIO_WritePin(DISCHARGE_CAP4_GPIO_Port, DISCHARGE_CAP4_Pin, GPIO_PIN_RESET)// ÷√0
#define DISCHARGE_CAP1_1   HAL_GPIO_WritePin(DISCHARGE_CAP4_GPIO_Port, DISCHARGE_CAP4_Pin, GPIO_PIN_SET)//÷√1
#define WELD_PULSE_0   		 HAL_GPIO_WritePin(WELD_PULSE_GPIO_Port, WELD_PULSE_Pin, GPIO_PIN_RESET)// ÷√0
#define WELD_PULSE_1   		 HAL_GPIO_WritePin(WELD_PULSE_GPIO_Port, WELD_PULSE_Pin, GPIO_PIN_SET)//÷√1
#define CHARGE_CTRL_0  		 HAL_GPIO_WritePin(CHARGE_CTRL_GPIO_Port, CHARGE_CTRL_Pin, GPIO_PIN_RESET)// ÷√0
#define CHARGE_CTRL_1  		 HAL_GPIO_WritePin(CHARGE_CTRL_GPIO_Port, CHARGE_CTRL_Pin, GPIO_PIN_SET)//÷√1

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

