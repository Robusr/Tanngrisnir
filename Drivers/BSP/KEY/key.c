#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"

#define KEY_RELEASE 0   // 按键释放状态（值为0）
#define KEY_PRESS 1

static uint8_t key_state = KEY_RELEASE;   // 按键当前状态
static uint8_t trigger_flag = 1;

void key_init(void)
{
	GPIO_InitTypeDef gpio_init_struct={0};
	KEY2_GPIO_CLK_ENABLE();
	gpio_init_struct.Pin =KEY2_GPIO_PIN;
	gpio_init_struct.Mode =GPIO_MODE_AF_INPUT;
	gpio_init_struct.Pull = GPIO_PULLUP;
	gpio_init_struct.Speed =GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEY2_GPIO_PORT,&gpio_init_struct);
	
}
uint8_t key_scan(uint8_t mode) {
    uint8_t current_level = HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN); // 读取当前电平（上拉模式下按下为0）
    uint8_t key_value = 0;                                               // 返回值
    
    // 状态变化时触发消抖
    if (current_level != key_state) {
        delay_ms(10);                     // 10ms消抖延时
        current_level 
= HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN); // 二次检测
    }

    // 非连续检测逻辑：仅在按下瞬间触发一次（触发后锁定，释放后重置）
    if (current_level == GPIO_PIN_RESET) { // 检测到按下（低电平有效）
        if (trigger_flag) {               // 允许触发时返回键值
            key_value 
= KEY2_PRES;
            trigger_flag 
= 0;             // 锁定触发
        }
        key_state 
= GPIO_PIN_RESET;       // 更新状态为按下
    } else {                             // 检测到释放（高电平）
        trigger_flag 
= 1;                 // 重置触发标志
        key_state 
= GPIO_PIN_SET;         // 更新状态为释放
    }

    return key_value;
}
