/**
 ****************************************************************************************************
 * @file        exti.h
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

#ifndef __EXTI_H
#define __EXTI_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 和 中断编号 & 中断服务函数 定义 */ 



//按键中断
#define KEY0_INT_GPIO_PORT              GPIOA
#define KEY0_INT_GPIO_PIN               GPIO_PIN_5
#define KEY0_INT_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */
#define KEY0_INT_IRQn                   EXTI9_5_IRQn
#define KEY0_INT_IRQHandler             EXTI9_5_IRQHandler
#define KEY0_INT                        HAL_GPIO_ReadPin(KEY0_INT_GPIO_PORT, KEY0_INT_GPIO_PIN)     /* 读取KEY0引脚 */

//MPU6050中断
#define MPU6050_EXTI_Pin GPIO_PIN_12
#define MPU6050_EXTI_GPIO_Port GPIOA
#define MPU6050_EXTI_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */
#define MPU6050_EXTI_IRQn                EXTI15_10_IRQn
#define MPU6050_IRQHandler               EXTI15_10_IRQHandler
/******************************************************************************************/


void exti_key0_init(void);  /* 外部中断初始化 */
void exti_mpu6050_init(void);  /* 外部中断初始化 */
void readVel(void);
void readMpu(void);
void readVolt(uint8_t cnt);

int Balance(float angle,float gyro);
int Velocity(int encoder_left,int encoder_right);
int Turn(float gyro);
void Set_Pwm(int motor_left,int motor_right);
int PWM_Limit(int IN,int max,int min);
uint8_t Turn_Off(void);
#endif




