/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
#ifndef __ADC_H__
#define __ADC_H__





#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
	//����������1-4��ѹ���ܵ�ѹ
	extern float VI,V1,V2,V3,V4,VV;
// �û�������

#define ADC_CHANNELS     5   // ʹ�õ�ADCͨ������
#define SAMPLE_TIMES    100 // ÿ��ͨ���Ĳ�������

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;
extern uint16_t adcDmaBuffer[ADC_CHANNELS * SAMPLE_TIMES];
extern uint16_t adcAverages[ADC_CHANNELS];

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */

void ADC_StartSampling(void);
float Get_ADC(int ch);
void Averages(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

