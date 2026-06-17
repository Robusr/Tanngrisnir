/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2021-10-18
 * @brief       ADC 驱动代码
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
 * V1.0 20211018
 * 第一次发布
 * V1.1 20211018
 * 1,支持ADC单通道DMA采集 
 * 2,新增adc_dma_init和adc_dma_enable函数
 * V1.2 20211018
 * 1,支持ADC多通道DMA采集
 * 2,新增adc_nch_dma_init和adc_nch_dma_gpio_init函数
 * 
 ****************************************************************************************************
 */

#include "./SYSTEM/delay/delay.h"
#include "./BSP/ADC/adc.h"


ADC_HandleTypeDef g_adc_handle;   /* ADC句柄 */

/* 单通道ADC采集 DMA读取 */
DMA_HandleTypeDef g_dma_adc_handle;     /* 与ADC关联的DMA句柄 */



/**
 * @brief       ADC初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道, 但是不支持ADC3
 *              我们使用12位精度, ADC采样时钟=21M, 转换时间为: 采样周期 + 12个ADC周期
 *              设置最大采样周期: 480, 则转换时间 = 492 个ADC周期 = 23.42us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
	  
    ADC_ChannelConfTypeDef sConfig = {0};
		
    g_adc_handle.Instance = ADC_ADCX;

    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;;                               /* 非扫描模式 */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;                          /* 开启连续转换 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* 不连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_adc_handle.Init.NbrOfConversion = 1;                                   /* 1个转换在规则序列中 也就是只转换规则序列1 */
    HAL_ADC_Init(&g_adc_handle);                                            /* 初始化 */
		
		sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
		HAL_ADC_ConfigChannel(&g_adc_handle, &sConfig);
		HAL_ADCEx_Calibration_Start(&g_adc_handle);//校准ADC，不校准会导致ADC测量不准确
}

/**
 * @brief       ADC底层驱动，引脚配置，时钟使能
                此函数会被HAL_ADC_Init()调用
 * @param       hadc:ADC句柄
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC_ADCX)
    {
        GPIO_InitTypeDef gpio_init_struct;
        ADC_ADCX_CHY_CLK_ENABLE();      /* 使能ADCx时钟 */
        ADC_ADCX_CHY_GPIO_CLK_ENABLE(); /* 开启GPIO时钟 */

			
        /* AD采集引脚模式设置,模拟输入 */
        gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_3CYCLES,  3个ADC时钟周期        ADC_SAMPLETIME_15CYCLES, 15个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_28CYCLES, 28个ADC时钟周期       ADC_SAMPLETIME_56CYCLES, 56个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_84CYCLES, 84个ADC时钟周期       ADC_SAMPLETIME_112CYCLES,112个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_144CYCLES,144个ADC时钟周期      ADC_SAMPLETIME_480CYCLES,480个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channel1的rank=1，channel2 的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channel1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道DMA设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */


/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道值 0~17，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      无
 */
int Get_battery_volt(void)  
{
	  int Volt;
    HAL_ADC_Start(&g_adc_handle);                                       /* 开启ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, 10);                       /* 轮询转换 */
    Volt = (uint16_t)HAL_ADC_GetValue(&g_adc_handle)*3.3*11*1000/4096;	//电阻分压，具体根据原理图简单分析可以得到	单位mv
	  return Volt;	
}

/**
 * @brief       获取通道ch的转换值, 取times次, 然后平均
 * @param       ch      : 通道号, 0~17
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
