#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f1xx_hal.h"

#define EC11_A_PORT GPIOB
#define EC11_A_PIN GPIO_PIN_6
#define EC11_B_PORT GPIOB
#define EC11_B_PIN GPIO_PIN_7
#define EC11_KEY_PORT GPIOB
#define EC11_KEY_PIN GPIO_PIN_5

// ����״̬����
#define KEY_SHORT_PRESS 1
#define KEY_LONG_PRESS 2
#define KEY_NO_PRESS 0

// ����������
#define ENCODER_CW  -1   // ��ʱ��
#define ENCODER_CCW  1   // ˳ʱ��

void Encoder_Init(void);
int16_t Encoder_GetCount(void);
void Encoder_ResetCount(void);
uint8_t Encoder_GetKeyState(void);
void Encoder_KeyScan(void);
int8_t Encoder_GetDirection(void);

#endif