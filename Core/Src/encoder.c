#include "encoder.h"
#include "main.h"
#include "gpio.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim4;

static uint8_t key_state = KEY_NO_PRESS;
static uint32_t key_press_time = 0;
static uint8_t key_pressed = 0;
static int8_t last_direction = 0;

// 编码器初始化
void Encoder_Init(void) {
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    __HAL_TIM_SET_COUNTER(&htim4, 32768); // 设置中间值
}

// 获取编码器计数值
int16_t Encoder_GetCount(void) {
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim4);
}

// 重置编码器计数值
void Encoder_ResetCount(void) {
    __HAL_TIM_SET_COUNTER(&htim4, 32768);
}

// 获取按键状态 (需要在主循环中定期调用Encoder_KeyScan)
uint8_t Encoder_GetKeyState(void) {
    uint8_t state = key_state;
    if (key_state != KEY_NO_PRESS) {
        BUZZER_1;  // 按键触发时开启蜂鸣器
        HAL_Delay(10); // 短暂蜂鸣
        BUZZER_0;  // 关闭蜂鸣器
    }		
    key_state = KEY_NO_PRESS; // 读取后清除状态
    return state;
}

// 按键扫描函数 (需要在主循环中定期调用)
void Encoder_KeyScan(void) {
    static uint8_t last_key_state = 1;
    uint8_t current_key_state = HAL_GPIO_ReadPin(EC11_KEY_PORT, EC11_KEY_PIN);
    
    // 按键按下(下降沿)
    if(last_key_state == 1 && current_key_state == 0) {
        key_press_time = HAL_GetTick();
        key_pressed = 1;
//				BUZZER_1;  // 按键触发时开启蜂鸣器
//				HAL_Delay(10); // 短暂蜂鸣
//				BUZZER_0;  // 关闭蜂鸣器

    }
    // 按键释放(上升沿)
    else if(last_key_state == 0 && current_key_state == 1) {
        if(key_pressed) {
            if(HAL_GetTick() - key_press_time < 1000) { // 短按阈值1s
                key_state = KEY_SHORT_PRESS;
            }
            key_pressed = 0;
        }
    }
    
    // 长按检测
    if(key_pressed && (HAL_GetTick() - key_press_time >= 1000)) {
        key_state = KEY_LONG_PRESS;
        key_pressed = 0; // 防止重复触发
    }
    
    last_key_state = current_key_state;
}

// 获取编码器方向及步数
int8_t Encoder_GetDirection(void) {
    static int16_t last_count = 32768;
    static int16_t accumulated_diff = 0;  // 新增：累积差值
    int16_t current_count = Encoder_GetCount();
    int16_t diff = current_count - last_count;
    accumulated_diff += diff/2;  // 累积变化量
    
    if(abs(accumulated_diff) < 2) {  // 需要至少2个物理步才触发
        return 0;
    }		
//    if(diff == 0) {
//        return 0;
//    }
    
    // 判断方向
//    int8_t direction = (diff > 0) ? ENCODER_CW : ENCODER_CCW;
    int8_t direction = (accumulated_diff > 0) ? ENCODER_CW : ENCODER_CCW;
    
    // 判断是否是快速旋转
//    uint8_t speed = (abs(diff) > 5) ? 10 : 1;
    uint8_t speed = (abs(accumulated_diff) > 10) ? 10 : 1;  // 原阈值的2倍
   
    // 重置计数器
    Encoder_ResetCount();
    last_count = 32768;
    accumulated_diff = 0;  // 重置累积量
		
		BUZZER_1;  // 按键触发时开启蜂鸣器
		HAL_Delay(10); // 短暂蜂鸣
		BUZZER_0;  // 关闭蜂鸣器    
		
    return direction * speed;
}