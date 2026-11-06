#include "./BSP/OLED/oled.h"
#include "./oledShow.h"

#define GYRO_LSB_PER_DPS 16.4f

extern volatile float pit, rol, yaw;
extern volatile short gyro[3];
extern volatile int Voltage;

void oledShow(void)
{
    int pitch = (int)(-pit);
    int gyro_dps = (int)(-gyro[0] / GYRO_LSB_PER_DPS);

    if (pitch < 0)
    {
        oled_show_string(70, 15, "-", 12);
    }
    else
    {
        oled_show_string(70, 15, "+", 12);
    }
    oled_show_num(90, 15, (uint32_t)abs(pitch), 3, 12);

    if (gyro_dps < 0)
    {
        oled_show_string(70, 30, "-", 12);
    }
    else
    {
        oled_show_string(70, 30, "+", 12);
    }
    oled_show_num(90, 30, (uint32_t)abs(gyro_dps), 5, 12);

    oled_show_num(70, 45, (uint32_t)Voltage, 5, 12);
    oled_refresh_gram();
}
