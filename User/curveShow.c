

#include "./curveShow.h"
#include "./utilities.h"


extern float pit, rol, yaw;
extern short gyro[3];
extern int Encoder_Left,Encoder_Right; 
extern int Voltage;

void curveShow(void)
{   
	uint8_t i;//计数变量
	float Vol;								//电压变量
	unsigned char Send_Count; //串口需要发送的数据个数
	Vol=(float)Voltage/1000;
	DataScope_Get_Channel_Data( -pit, 1 );       //显示角度 单位：度（°）
	DataScope_Get_Channel_Data( Encoder_Right, 2 );         //显示超声波测量的距离 单位：CM 
	DataScope_Get_Channel_Data( Vol, 3 );                 //显示电池电压 单位：V
//		DataScope_Get_Channel_Data( 0 , 4 );   
//		DataScope_Get_Channel_Data(0, 5 ); //用您要显示的数据替换0就行了
//		DataScope_Get_Channel_Data(0 , 6 );//用您要显示的数据替换0就行了
//		DataScope_Get_Channel_Data(0, 7 );
//		DataScope_Get_Channel_Data( 0, 8 ); 
//		DataScope_Get_Channel_Data(0, 9 );  
//		DataScope_Get_Channel_Data( 0 , 10);
	Send_Count = DataScope_Data_Generate(3);
	
	for(i = 0 ; i < Send_Count; i++) 
	{
		while((USART1->SR&0X40)==0);  
		USART1->DR = DataScope_OutPut_Buffer[i]; 
	}
}
