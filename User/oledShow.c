#include "./BSP/OLED/oled.h"
#include "./oledShow.h"

extern float pit, rol, yaw;
extern short gyro[3];
extern int Voltage;
void oledShow()
{
	if(-pit<0) oled_show_string(70,15,"-",12);
	if(-pit>=0) oled_show_string(70,15,"+",12);
	oled_show_num(90,15,abs((int)(-pit)),3,12);
	if(-gyro[0]<0) oled_show_string(70,30,"-",12);
	if(-gyro[0]>=0) oled_show_string(70,30,"+",12);
	oled_show_num(90,30,abs((int)(-gyro[0])),5,12);
	oled_show_num(70,45,Voltage,5,12);
	oled_refresh_gram();
	
}
