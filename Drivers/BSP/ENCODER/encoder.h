#include "./SYSTEM/sys/sys.h"

void leftEncoderInit(void);
void rightEncoderInit(void);

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim);

int Read_Encoder(uint8_t TIMX);
