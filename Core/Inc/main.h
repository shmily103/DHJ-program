/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOA
#define DISCHARGE_CAP1_Pin GPIO_PIN_1
#define DISCHARGE_CAP1_GPIO_Port GPIOA
#define ADC_CAP1_Pin GPIO_PIN_2
#define ADC_CAP1_GPIO_Port GPIOA
#define DISCHARGE_CAP2_Pin GPIO_PIN_3
#define DISCHARGE_CAP2_GPIO_Port GPIOA
#define ADC_CAP2_Pin GPIO_PIN_4
#define ADC_CAP2_GPIO_Port GPIOA
#define DISCHARGE_CAP3_Pin GPIO_PIN_5
#define DISCHARGE_CAP3_GPIO_Port GPIOA
#define ADC_CAP3_Pin GPIO_PIN_6
#define ADC_CAP3_GPIO_Port GPIOA
#define DISCHARGE_CAP4_Pin GPIO_PIN_7
#define DISCHARGE_CAP4_GPIO_Port GPIOA
#define ADC_CAP4_Pin GPIO_PIN_0
#define ADC_CAP4_GPIO_Port GPIOB
#define ADC_II_Pin GPIO_PIN_1
#define ADC_II_GPIO_Port GPIOB
#define TRIGGER_Pin GPIO_PIN_10
#define TRIGGER_GPIO_Port GPIOB
#define WELD_PULSE_Pin GPIO_PIN_11
#define WELD_PULSE_GPIO_Port GPIOB
#define TFT_BLK_Pin GPIO_PIN_12
#define TFT_BLK_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_13
#define TFT_CS_GPIO_Port GPIOB
#define TFT_DC_Pin GPIO_PIN_14
#define TFT_DC_GPIO_Port GPIOB
#define TFT_RES_Pin GPIO_PIN_15
#define TFT_RES_GPIO_Port GPIOB
#define TFT_SDA_Pin GPIO_PIN_11
#define TFT_SDA_GPIO_Port GPIOA
#define TFT_SCL_Pin GPIO_PIN_12
#define TFT_SCL_GPIO_Port GPIOA
#define EC_KEY_Pin GPIO_PIN_5
#define EC_KEY_GPIO_Port GPIOB
#define EC_KEY_EXTI_IRQn EXTI9_5_IRQn
#define EC_A_Pin GPIO_PIN_6
#define EC_A_GPIO_Port GPIOB
#define EC_B_Pin GPIO_PIN_7
#define EC_B_GPIO_Port GPIOB
#define CHARGE_CTRL_Pin GPIO_PIN_8
#define CHARGE_CTRL_GPIO_Port GPIOB
#define ST_Pin GPIO_PIN_9
#define ST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
