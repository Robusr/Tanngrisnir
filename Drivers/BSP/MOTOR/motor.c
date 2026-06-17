#include "./BSP/MOTOR/MOTOR.h"
#include "./utilities.h"
#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"

extern uint8_t stopFlag;
TIM_HandleTypeDef g_timx_pwm_chy_handle;     /* 定时器x句柄 */
void motorInit()
{
	
	  
	  /* 电机PWM定时器配置移植正点原子探索者，容易受干扰PWM不输出 */
	    __HAL_RCC_TIM1_CLK_ENABLE();                 //开启TIM1时钟
			TIM_OC_InitTypeDef timx_oc_pwm_chy  = {0};                          /* 定时器PWM输出配置 */
			g_timx_pwm_chy_handle.Instance = TIM1;                     /* 定时器x */
			g_timx_pwm_chy_handle.Init.Prescaler = 0;                         /* 定时器分频 */
			g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* 递增计数模式 */
			g_timx_pwm_chy_handle.Init.Period = 7199;                            /* 自动重装载值 */
			HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle);                           /* 初始化PWM */

			timx_oc_pwm_chy.OCMode = TIM_OCMODE_PWM1;                           /* 模式选择PWM1 */
			timx_oc_pwm_chy.Pulse = 0;                                    /* 设置比较值,此值用来确定占空比 */
																																					/* 默认比较值为自动重装载值的一半,即占空比为50% */
			timx_oc_pwm_chy.OCPolarity = TIM_OCPOLARITY_HIGH;                    /* 输出比较极性为低 */
			HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, TIM_CHANNEL_1); /* 配置TIMx通道y */
			HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, TIM_CHANNEL_4); /* 配置TIMx通道y */
			HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, TIM_CHANNEL_1);       /* 开启对应PWM通道 */
			HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, TIM_CHANNEL_4);       /* 开启对应PWM通道 */
    	HAL_TIM_PWM_MspInit(&g_timx_pwm_chy_handle);


	  
}


void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* timHandle)
{

 
  if(timHandle->Instance==TIM1)
  {
		/* 电机PWM引脚 */
		 GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = PWMA_Pin|PWMB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		
		/* 电机方向引脚 */
		 GPIO_InitTypeDef gpio_init_struct;
		__HAL_RCC_GPIOB_CLK_ENABLE();
    gpio_init_struct.Pin = BIN2_Pin|BIN1_Pin|AIN1_Pin|AIN2_Pin;                  
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;                   /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);       /* 初始化LED0引脚 */
		
	}

}





void motorMove(int motor_left,int motor_right)
{

	if(motor_right>0)	  //前进   
	{ BIN1(1);
		BIN2(0); 
	}
	else                //后退     			  
	{ BIN1(0);
		BIN2(1); 
	}
	PWMB=abs(motor_right);	//前进
  if(motor_left>0)			
	{
			AIN1(1);	
		  AIN2(0);	
	}
	else 	    //后退    			 
	{		AIN1(0);
  		AIN2(1); 
	}
	PWMA=abs(motor_left);
			
}



int PWM_Limit(int IN,int max,int min)
{
	if (IN > max) {
        return max;  // 超过最大值，返回最大值
    } else if (IN < min) {
        return min;  // 低于最小值，返回最小值
    } else {
        return IN;   // 正常范围，直接返回原值
    }	
}
 





