#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/MOTOR/motor.h"
#include "./BSP/ENCODER/encoder.h"
#include "./BSP/TIMER/gtim.h"
#include "./curveShow.h"
#include "./utilities.h"
#include <stdio.h>
#include <string.h>
#include "./BSP/ATK_MS6050/atk_ms6050.h"
#include "./BSP/ATK_MS6050/eMPL/inv_mpu.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/OLED/oled.h"
#include "./oledShow.h"

int16_t encoder_left, encoder_right;
extern uint8_t stopFlag;
extern float pit,rol,yaw;
extern short gyro[3];
extern int Voltage;
extern int motor_left;
extern int motor_right;

int Balance(float angle, float gyro);
int Velocity(int encoder_left, int encoder_right);
int PWM_Limit(int IN, int max, int min);


int main(void)
{
	HAL_Init();
	sys_stm32_clock_init(RCC_PLL_MUL9);
	delay_init(72);
	usart_init(115200);
	adc_init();
	atk_ms6050_init();
	atk_ms6050_dmp_init();
	exti_mpu6050_init();
	exti_key0_init();
	oled_init();
    oled_show_string(0,0,"ID:U202410620!",12);
	oled_show_string(0,15,"pitAngle:",12);
	oled_show_string(0,30,"pitVel:",12);
	oled_show_string(0,45,"batVol:",12);
	oled_show_string(110,45,"mV",12);
	oled_refresh_gram();
	motorInit();
	leftEncoderInit();
    rightEncoderInit();

	delay_ms(1000);
    
	

	while(1)
	{
		oledShow();
       if(Voltage < 10000) // 低于10V停止
        {
            stopFlag = 1;
            motorMove(0, 0);
            printf("电压过低! %.2fV\r\n", Voltage/1000.0);
            delay_ms(1000);
            continue;
        }
		delay_ms(10);
	}
	
}

	
