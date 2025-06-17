#include "encoder.h"
#include "main.h"
#include "gpio.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim4;

static uint8_t key_state = KEY_NO_PRESS;
static uint32_t key_press_time = 0;
static uint8_t key_pressed = 0;
static int8_t last_direction = 0;

// ��������ʼ��
void Encoder_Init(void) {
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    __HAL_TIM_SET_COUNTER(&htim4, 32768); // �����м�ֵ
}

// ��ȡ����������ֵ
int16_t Encoder_GetCount(void) {
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim4);
}

// ���ñ���������ֵ
void Encoder_ResetCount(void) {
    __HAL_TIM_SET_COUNTER(&htim4, 32768);
}

// ��ȡ����״̬ (��Ҫ����ѭ���ж��ڵ���Encoder_KeyScan)
uint8_t Encoder_GetKeyState(void) {
    uint8_t state = key_state;
    if (key_state != KEY_NO_PRESS) {
        BUZZER_1;  // ��������ʱ����������
        HAL_Delay(50); // ���ݷ���
        BUZZER_0;  // �رշ�����
    }		
    key_state = KEY_NO_PRESS; // ��ȡ�����״̬
    return state;
}

// ����ɨ�躯�� (��Ҫ����ѭ���ж��ڵ���)
void Encoder_KeyScan(void) {
    static uint8_t last_key_state = 1;
    uint8_t current_key_state = HAL_GPIO_ReadPin(EC11_KEY_PORT, EC11_KEY_PIN);
    
    // ��������(�½���)
    if(last_key_state == 1 && current_key_state == 0) {
        key_press_time = HAL_GetTick();
        key_pressed = 1;
				BUZZER_1;  // ��������ʱ����������
    }
    // �����ͷ�(������)
    else if(last_key_state == 0 && current_key_state == 1) {
				BUZZER_0;  // �رշ�����
        if(key_pressed) {
            if(HAL_GetTick() - key_press_time < 1000) { // �̰���ֵ1s
                key_state = KEY_SHORT_PRESS;
            }
            key_pressed = 0;
        }
    }
    
    // �������
    if(key_pressed && (HAL_GetTick() - key_press_time >= 1000)) {
        key_state = KEY_LONG_PRESS;
        key_pressed = 0; // ��ֹ�ظ�����
    }
    
    last_key_state = current_key_state;
}

// ��ȡ���������򼰲���
int8_t Encoder_GetDirection(void) {
    static int16_t last_count = 32768;
    static int16_t accumulated_diff = 0;  // �������ۻ���ֵ
    int16_t current_count = Encoder_GetCount();
    int16_t diff = current_count - last_count;
    accumulated_diff += diff/2;  // �ۻ��仯��
    
    if(abs(accumulated_diff) < 2) {  // ��Ҫ����2�������Ŵ���
        return 0;
    }		
//    if(diff == 0) {
//        return 0;
//    }
    
    // �жϷ���
//    int8_t direction = (diff > 0) ? ENCODER_CW : ENCODER_CCW;
    int8_t direction = (accumulated_diff > 0) ? ENCODER_CW : ENCODER_CCW;
    
    // �ж��Ƿ��ǿ�����ת
//    uint8_t speed = (abs(diff) > 5) ? 10 : 1;
    uint8_t speed = (abs(accumulated_diff) > 10) ? 10 : 1;  // ԭ��ֵ��2��
   
    // ���ü�����
    Encoder_ResetCount();
    last_count = 32768;
    accumulated_diff = 0;  // �����ۻ���
		
		BUZZER_1;  // ��������ʱ����������
		HAL_Delay(20); // ���ݷ���
		BUZZER_0;  // �رշ�����    
		
    return direction * speed;
}