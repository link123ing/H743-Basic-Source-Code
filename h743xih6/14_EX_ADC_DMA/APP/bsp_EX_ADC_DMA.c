#include "bsp_EX_ADC_DMA.h"

int adc_val = 0;
float volt = 0;
#if USE_DMA 
ALIGN_32BYTES (uint16_t adc_dma_buf[1]) __attribute__((section(".ARM.__at_0x38000000")));//BDMA在该区域内有效

void EX_ADC_Init(void)
{
	HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);    //ADC偏移校准
  HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_dma_buf, 1);

}
/**
 * @brief       ADC采集中断回调函数
 * @param       无 
 * @retval      无
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC3 )                                   /* 判断是不是ADC1 */
    {       
				HAL_ADC_Stop_DMA(&hadc3);                                   /* 关闭DMA转换 */
        SCB_InvalidateDCache_by_Addr((uint32_t *)adc_dma_buf,sizeof(adc_dma_buf)); /* 刷新D-cach，非常必要的一句 */
        
        adc_val = adc_dma_buf[0];
				HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_dma_buf, 1); /* 启动DMA转换 */
    }
}

void ADC_proc(void)
{
    volt = adc_val * 3.3f / 65535.0f;

    int volt_int = (int)volt;
    int volt_dec = (int)((volt - volt_int) * 1000);
    Uart_Printf(&huart1,"ADC:%d, volt:%d.%03dV\r\n", adc_val, volt_int, volt_dec);
}


#else


void EX_ADC_Init(void)
{
	HAL_ADC_Start(&hadc3);
}

void ADC_proc(void)
{
	adc_val = HAL_ADC_GetValue(&hadc3);
	volt = adc_val * 3.3f / 65536.0f;

	int volt_int = (int)volt;
	int volt_dec = (int)((volt - volt_int) * 1000);
	Uart_Printf(&huart1,"ADC:%d, volt:%d.%03dV\r\n", adc_val, volt_int, volt_dec);

}
#endif
