#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/MOTOR/motor.h"
#include "./BSP/ENCODER/encoder.h"
#include "./curveShow.h"
#include "./utilities.h"
#include <stdio.h>
#include "./BSP/ATK_MS6050/atk_ms6050.h"
#include "./BSP/ATK_MS6050/eMPL/inv_mpu.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/OLED/oled.h"
#include "./oledShow.h"

extern volatile uint8_t stopFlag;
extern volatile int Voltage;

static void system_fault(const char *msg)
{
    stopFlag = 1;
    motorStop();
    oled_show_string(0, 0, msg, 12);
    oled_refresh_gram();
    printf("%s\r\n", msg);

    while (1)
    {
        LED_ON();
        delay_ms(250);
        LED_OFF();
        delay_ms(250);
    }
}

int main(void)
{
    uint8_t ret;
    uint8_t tries;
    short gyro_buf[3];
    float pitch_buf, roll_buf, yaw_buf;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    usart_init(115200);
    adc_init();
    motorInit();
    leftEncoderInit();
    rightEncoderInit();
    oled_init();

    oled_show_string(0, 0, "ID:U202515287", 12);
    oled_show_string(0, 15, "pitAngle:", 12);
    oled_show_string(0, 30, "pitVel:", 12);
    oled_show_string(0, 45, "batVol:", 12);
    oled_show_string(110, 45, "mV", 12);
    oled_refresh_gram();

    delay_ms(200);
    ret = atk_ms6050_init();
    if (ret != ATK_MS6050_EOK)
    {
        delay_ms(200);
        ret = atk_ms6050_init();
    }
    if (ret != ATK_MS6050_EOK)
    {
        system_fault("IMU INIT FAIL!");
    }

    delay_ms(200);
    ret = atk_ms6050_dmp_init();
    if (ret != 0)
    {
        delay_ms(200);
        ret = atk_ms6050_dmp_init();
    }
    if (ret != 0)
    {
        printf("DMP init warning\r\n");
    }

    tries = 0;
    while (tries < 20 &&
           atk_ms6050_dmp_get_data(gyro_buf, &pitch_buf, &roll_buf, &yaw_buf) != 0)
    {
        delay_ms(10);
        tries++;
    }
    if (tries >= 20)
    {
        system_fault("DMP READ FAIL!");
    }

    exti_key0_init();
    exti_mpu6050_init();

    delay_ms(1000);

    while (1)
    {
        readVolt(10);

        if (mpuFault)
        {
            stopFlag = 1;
            motorStop();
            oled_show_string(0, 0, "IMU FAULT!    ", 12);
        }

        oledShow();
        curveShow();
        usartTest();

        if (stopFlag)
        {
            LED_OFF();
        }
        else
        {
            LED_ON();
        }

        if (Voltage > 0 && Voltage < LOW_VOLT_STOP_MV)
        {
            stopFlag = 1;
            motorStop();
            printf("Low voltage: %.2fV\r\n", Voltage / 1000.0);
            delay_ms(1000);
            continue;
        }

        delay_ms(10);
    }
}
