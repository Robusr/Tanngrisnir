#include "./BSP/ENCODER/encoder.h"

void leftEncoderInit(void)
{
	TIM_HandleTypeDef hTimEncoder={0}; 
  TIM_Encoder_InitTypeDef hEncoder={0};
	TIM_MasterConfigTypeDef sMasterConfig={0};
	
	//初始化左编码器
	hTimEncoder.Instance = TIM3;
	hTimEncoder.Init.Prescaler = 0;
	hTimEncoder.Init.CounterMode = TIM_COUNTERMODE_UP;
	hTimEncoder.Init.Period = 0xFFFF;
	hTimEncoder.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	hTimEncoder.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	hEncoder.EncoderMode = TIM_ENCODERMODE_TI12;
	hEncoder.IC1Polarity = TIM_ICPOLARITY_RISING;
	hEncoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	hEncoder.IC1Prescaler = TIM_ICPSC_DIV1;
	hEncoder.IC1Filter = 0;
	hEncoder.IC2Polarity = TIM_ICPOLARITY_RISING;
	hEncoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	hEncoder.IC2Prescaler = TIM_ICPSC_DIV1;
	hEncoder.IC2Filter = 0;
	
	HAL_TIM_Encoder_Init(&hTimEncoder,&hEncoder);
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&hTimEncoder, &sMasterConfig);
	
	HAL_TIM_Encoder_Start(&hTimEncoder,TIM_CHANNEL_ALL);
	
}
void rightEncoderInit(void)
{
	TIM_HandleTypeDef hTimEncoder={0}; ; 
  TIM_Encoder_InitTypeDef hEncoder={0};
	TIM_MasterConfigTypeDef sMasterConfig={0};
	
	//初始化右编码器
	hTimEncoder.Instance = TIM4;
	hTimEncoder.Init.Prescaler = 0;
	hTimEncoder.Init.CounterMode = TIM_COUNTERMODE_UP;
	hTimEncoder.Init.Period = 0xFFFF;
	hTimEncoder.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	hTimEncoder.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	hEncoder.EncoderMode = TIM_ENCODERMODE_TI12;
	hEncoder.IC1Polarity = TIM_ICPOLARITY_RISING;
	hEncoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	hEncoder.IC1Prescaler = TIM_ICPSC_DIV1;
	hEncoder.IC1Filter = 0;
	hEncoder.IC2Polarity = TIM_ICPOLARITY_RISING;
	hEncoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	hEncoder.IC2Prescaler = TIM_ICPSC_DIV1;
	hEncoder.IC2Filter = 0;
	
	HAL_TIM_Encoder_Init(&hTimEncoder,&hEncoder);
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&hTimEncoder, &sMasterConfig);
  
	HAL_TIM_Encoder_Start(&hTimEncoder,TIM_CHANNEL_ALL);
	

	
	



}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(htim->Instance==TIM3)
  {
    
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;       //无上下拉
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
	if(htim->Instance==TIM4)
	{
		
		__HAL_RCC_GPIOB_CLK_ENABLE();//使能TIM3时钟
		__HAL_RCC_TIM4_CLK_ENABLE();//开启GPIOA时钟
		GPIO_InitStruct.Pin=GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;//无上下拉
		HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
		
	}
	

}

int Read_Encoder(uint8_t TIMX)
{
   int Encoder_TIM;    
   switch(TIMX)
	 {
	   case 3:  Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0;break;
		 case 4:  Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0;break;	
		 default: Encoder_TIM=0;
	 }
		return Encoder_TIM;
}
