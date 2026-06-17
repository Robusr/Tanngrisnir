/**
 ****************************************************************************************************
 * @file        exti.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       外部中断 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211014
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/EXTI/exti.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/ATK_MS6050/atk_ms6050.h"
#include "./BSP/ATK_MS6050/eMPL/inv_mpu.h"
#include "./BSP/ENCODER/encoder.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/MOTOR/motor.h"
#include <cmath>
/**


 */
 
 
 
uint8_t ret=0;
float pit, rol, yaw;
short gyro[3];
int Encoder_Left,Encoder_Right; 
int Voltage,Voltage_Temp,Voltage_Count,Voltage_All;		//电压测量相关变量
uint8_t stopFlag;
uint8_t tenMsFlag;
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;		  					//平衡环PWM变量，速度环PWM变量，转向环PWM变
int Motor_Left,Motor_Right;                 //电机PWM变量 
float Balance_Kp=400,Balance_Kd=2,Velocity_Kp=160,Velocity_Ki=0.8;
//400,1.8，130，0.65


//中断处理函数
void KEY0_INT_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY0_INT_GPIO_PIN);        /* 调用中断处理公用函数 清除KEY_UP所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_INT_GPIO_PIN);        /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
} 

void MPU6050_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(MPU6050_EXTI_Pin);        /* 调用中断处理公用函数 清除KEY_UP所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(MPU6050_EXTI_Pin);        /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{ 
    switch(GPIO_Pin)
    {
        case KEY0_INT_GPIO_PIN:
            if (KEY0_INT == 0)  //最高速
            {
							stopFlag=!stopFlag;
							
							if(LEDSta)
							{
								LED0(0);
							}else
							{
								LED0(1);
							}
						
					  }
    
          break;
						case MPU6050_EXTI_Pin:  //每5ms一次中断
						
					    tenMsFlag=!tenMsFlag;//10ms到达标记
							readVel();   //测量左右轮编码器速度cnt/5ms
					    readMpu();   //测量pit，pitVel，yawVel，初始化时需要直立
					    if(tenMsFlag)
							{
								readVolt(10);//测量电池电压 ,每10ms更新一次
								return;
							}
							
						  
							/**
							************************************************
							以下为用户控制代码，每10ms一次控制周期
							
							***************************************************
							*/
							
							
							Balance_Pwm=Balance(-pit,-gyro[0]);    //平衡PID控制 Gyro_Balance平衡角速度极性：前倾为正，后倾为负
							Velocity_Pwm=Velocity(Encoder_Left,Encoder_Right);  //速度环PID控制	记住，速度反馈是正反馈，就是小车快的时候要慢下来就需要再跑快一点
							Motor_Left=Balance_Pwm+Velocity_Pwm;       //计算左轮电机最终PWM
							Motor_Right=Balance_Pwm+Velocity_Pwm;      //计算右轮电机最终PWM
							
							//PWM值正数使小车前进，负数使小车后退
							Motor_Left=PWM_Limit(Motor_Left,6900,-6900);
							Motor_Right=PWM_Limit(Motor_Right,6900,-6900);			//PWM限幅
							if(!Turn_Off())     					//如果不存在异常
							motorMove(Motor_Left,Motor_Right);         					//赋值给PWM寄存器  
							
			
							
							
							/**
							************************************************
							以上为用户控制代码，每10ms一次控制周期
							
							***************************************************
							*/
							
							
							
           break;
					
         default : break;
    }
}

/**
 * @brief       外部中断初始化程序
 * @param       无
 * @retval      无
 */
void exti_key0_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    //KEY0_INT按键中断初始化;
	  KEY0_INT_GPIO_CLK_ENABLE();                                     /* KEY0时钟使能 */  
    gpio_init_struct.Pin = KEY0_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* 下降沿触发 */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY0_INT_GPIO_PORT, &gpio_init_struct);    /* KEY0配置为下降沿触发中断 */
    HAL_NVIC_SetPriority(KEY0_INT_IRQn, 1, 2);               /* 抢占0，子优先级2 */
    HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);                       /* 使能中断线4 */

}

void exti_mpu6050_init(void)
{
	  GPIO_InitTypeDef gpio_init_struct;
    
    //KEY0_INT按键中断初始化;
	  MPU6050_EXTI_GPIO_CLK_ENABLE();                                     /* KEY0时钟使能 */  
    gpio_init_struct.Pin = MPU6050_EXTI_Pin;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* 下降沿触发 */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(MPU6050_EXTI_GPIO_Port, &gpio_init_struct);    /* KEY0配置为下降沿触发中断 */
    HAL_NVIC_SetPriority(MPU6050_EXTI_IRQn, 1, 2);               /* 抢占0，子优先级2 */
    HAL_NVIC_EnableIRQ(MPU6050_EXTI_IRQn);                       /* 使能中断线4 */
	
}





void readVel(void)
{
	
		Encoder_Left=-Read_Encoder(3);            					//读取左轮编码器的值，前进为正，后退为负
		Encoder_Right=-Read_Encoder(4);           					//读取右轮编码器的值，前进为正，后退为负
	
}

void readMpu(void)
{
		do{ret=atk_ms6050_dmp_get_data(gyro,&pit, &rol, &yaw);}
		while(ret!=0);
}

void readVolt(uint8_t cnt)
{
	
		Voltage_Temp=Get_battery_volt();		    					//读取电池电压		
		Voltage_Count++;                       						//平均值计数器
		Voltage_All+=Voltage_Temp;              					//多次采样累积
		if(Voltage_Count==cnt) Voltage=Voltage_All/cnt,Voltage_All=0,Voltage_Count=0;//求平均值单位mv
}



int Balance(float Angle,float Gyro)
{  
  
		 int balance=Balance_Kp*Angle+Gyro*Balance_Kd; //计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
		 return balance;

}


int Velocity(int encoder_left,int encoder_right)  //转速环PI控制
{   
      static float velocity,Encoder_bias,Encoder_Integral;
      Encoder_bias =0-(encoder_left+encoder_right);    
	    Encoder_Integral +=Encoder_bias;                                  //积分出位移 积分时间：10ms
			if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //积分限幅
			if(Encoder_Integral<-10000)	  Encoder_Integral=-10000;            //积分限幅	
			velocity=-Encoder_bias*Velocity_Kp-Encoder_Integral*Velocity_Ki;     //速度控制	
			if(stopFlag==1) Encoder_Integral=0;//电机关闭后清除积分
			return velocity;                                            //这里利用串级PID进行了数学化简得到一个速度环，本质输出的其实是倾角，倾角给到直立环作为输入从而输出a，a就可以调节速度了。比如目标速度是0，当前反馈速度为正，则需要a为负来减速，那么要输出倾角为负。
}

uint8_t Turn_Off(void)
{
    // 1. 检查电池电压是否过低（假设低于7V为异常）
    if(Voltage < 7000)  // Voltage单位是mV
    {
        return 1;  // 需要关闭电机
    }
    
    // 2. 检查姿态角度是否过大（假设超过30度为异常）
    if(fabs(pit) > 30 || fabs(rol) > 30)
    {
        return 1;  // 需要关闭电机
    }
    
    // 3. 检查是否手动停止标志
    if(stopFlag == 1)
    {
        return 1;  // 需要关闭电机
    }
    
    // 4. 其他可能的异常检查...
    
    return 0;  // 正常运行，不需要关闭电机
}

