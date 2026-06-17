#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"
#define KEY2_GPIO_PORT GPIOA
#define KEY2_GPIO_PIN GPIO_PIN_5
#define KEY2_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOA_CLK_ENABLE();}while(0)

#define KEY2 HAL_GPIO_ReadPin(KEY2_GPIO_PORT,KEY2_GPIO_PIN)
#define KEY2_PRES 2
void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif
