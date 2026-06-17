#include "./SYSTEM/sys/sys.h"
#define LED0_GPIO_PORT GPIOA
#define LED0_GPIO_PIN  GPIO_PIN_4
#define LED0_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOA_CLK_ENABLE();}while(0)

#define LEDSta HAL_GPIO_ReadPin(LED0_GPIO_PORT,LED0_GPIO_PIN)

#define LED0(x) do{x ? \
	HAL_GPIO_WritePin(LED0_GPIO_PORT,LED0_GPIO_PIN,GPIO_PIN_SET): \
	               HAL_GPIO_WritePin(LED0_GPIO_PORT,LED0_GPIO_PIN,GPIO_PIN_RESET); \
}while(0)

#define LED0_TOGGLE() do{HAL_GPIO_TogglePin(LED0_GPIO_PORT,LED0_GPIO_PIN); } while(0)

void led_init(void);
