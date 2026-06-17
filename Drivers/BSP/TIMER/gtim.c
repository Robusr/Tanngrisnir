#include <string.h>
#include "./BSP/TIMER/gtim.h"
#include "./curveShow.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/ENCODER/encoder.h"
#include "./BSP/MOTOR/motor.h"
#include "./BSP/LED/led.h"

TIM_HandleTypeDef g_timx_handle;             /* 定时器x句柄 */

float Bias,Pwm,Integral_bias,Last_Bias;
double Vel_KP=10,Vel_KI=2;   //pid参数
int32_t vel=0,target=25;
extern uint8_t stopFlag;


/**
 * @brief       通用定时器TIM2定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 预分频系数
 * @retval      无
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE(); /* 使能TIMx时钟 */
    g_timx_handle.Instance = GTIM_TIMX_INT;                 /* 通用定时器x */
    g_timx_handle.Init.Prescaler = psc;                     /* 预分频系数 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;    /* 向上计数器 */
    g_timx_handle.Init.Period = arr;                        /* 递增计数模式 */
    HAL_TIM_Base_Init(&g_timx_handle);    
    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 1, 3);         /* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                 /* 开启ITMx中断 */
    HAL_TIM_Base_Start_IT(&g_timx_handle);                  /* 使能定时器x和定时器x更新中断 */
}

void GTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);  /* 定时器回调函数 */
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_INT)
    {
			//中断处理逻辑
			
				if(!stopFlag)
			{
				 
				 vel=(-Read_Encoder(4));//两个轮子向前为正，读编码器值取反方向才与实际一致
				 Pwm=Vel_PI(vel,target);
				 Pwm=PWM_Limit(Pwm,6900,-6900);     //限幅得到的motor要和pwm分开，限幅只影响实际输出而不能改变算出的PWM值
				 motorMove(Pwm,Pwm);         					//赋值给PWM寄存器  
			}else
			{
				//停止电机
				AIN1(0);                                            
				AIN2(0);
				BIN1(0);
				BIN2(0);
				Pwm=0;  //停止后复位pid
				vel=0;
				Integral_bias=0;//停止后复位pid
				Bias=0;
				Last_Bias=0;
			}

			
			
			
    }
}



int Vel_PI(int vel,int Target)
{
	 static int last_output = 0;
    Bias = Target - vel;
    // 比例项
    float P_term = Vel_KP * (Bias - Last_Bias);
    // 积分项
    Integral_bias += Bias;
    float I_term = Vel_KI * Bias;
    int output = last_output + P_term + I_term;
    Last_Bias = Bias;
    last_output = output;
    return output;
}


