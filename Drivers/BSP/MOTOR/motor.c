#include "./BSP/MOTOR/motor.h"
#include <stdlib.h>
#include "./SYSTEM/delay/delay.h"

TIM_HandleTypeDef g_timx_pwm_chy_handle;

void motorInit(void)
{
    TIM_OC_InitTypeDef timx_oc_pwm_chy = {0};

    __HAL_RCC_TIM1_CLK_ENABLE();

    g_timx_pwm_chy_handle.Instance = TIM1;
    g_timx_pwm_chy_handle.Init.Prescaler = 0;
    g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_pwm_chy_handle.Init.Period = 7199;
    if (HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle) != HAL_OK)
    {
        return;
    }

    timx_oc_pwm_chy.OCMode = TIM_OCMODE_PWM1;
    timx_oc_pwm_chy.Pulse = 0;
    timx_oc_pwm_chy.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, TIM_CHANNEL_4);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *timHandle)
{
    if (timHandle->Instance == TIM1)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitTypeDef gpio_init_struct = {0};

        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = PWMA_Pin | PWMB_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        __HAL_RCC_GPIOB_CLK_ENABLE();
        gpio_init_struct.Pin = BIN2_Pin | BIN1_Pin | AIN1_Pin | AIN2_Pin;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);

        motorStop();
    }
}

void motorStop(void)
{
    AIN1(0);
    AIN2(0);
    BIN1(0);
    BIN2(0);
    PWMA = 0;
    PWMB = 0;
}

void motorMove(int motor_left, int motor_right)
{
    if (motor_right > 0)
    {
        BIN1(1);
        BIN2(0);
    }
    else if (motor_right < 0)
    {
        BIN1(0);
        BIN2(1);
    }
    else
    {
        BIN1(0);
        BIN2(0);
    }
    PWMB = (uint32_t)abs(motor_right);

    if (motor_left > 0)
    {
        AIN1(1);
        AIN2(0);
    }
    else if (motor_left < 0)
    {
        AIN1(0);
        AIN2(1);
    }
    else
    {
        AIN1(0);
        AIN2(0);
    }
    PWMA = (uint32_t)abs(motor_left);
}

int PWM_Limit(int IN, int max, int min)
{
    if (IN > max)
    {
        return max;
    }
    else if (IN < min)
    {
        return min;
    }
    else
    {
        return IN;
    }
}
