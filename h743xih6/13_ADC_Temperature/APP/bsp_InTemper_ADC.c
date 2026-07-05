#include "bsp_InTemper_ADC.h"

void Printf_Logo(void)
{
	Uart_Printf(&huart1,"*************************************************************\r\n");
	
    uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;
    
    CPU_Sn0 = *(__IO uint32_t*)(0x1FF1E800);
    CPU_Sn1 = *(__IO uint32_t*)(0x1FF1E800 + 4);
    CPU_Sn2 = *(__IO uint32_t*)(0x1FF1E800 + 8);

    Uart_Printf(&huart1,"CPU : STM32H743XIH6, BGA240, 主频: %dMHz\r\n", SystemCoreClock / 1000000);
    Uart_Printf(&huart1,"UID = %08X %08X %08X\r\n", CPU_Sn2, CPU_Sn1, CPU_Sn0);

		Uart_Printf(&huart1,"*************************************************************\r\n");
		Uart_Printf(&huart1,"* 例程名称 : 单片机内部温度测试\r\n");	/* 打印例程名称 */
    Uart_Printf(&huart1,"* 例程现象 : 串口发送单片机内部温度和温度变化\r\n");	/* 打印例程名称 */

	Uart_Printf(&huart1,"*************************************************************\r\n");
}


/**
 * @brief   获取ADC3结果
 * @param   无
 * @retval  ADC结果
 */
uint32_t adc3_get_result(void)
{
    uint32_t result;
        
    HAL_ADC_Start(&hadc3);                                      /* 开启ADC转换 */
    HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);           /* 等待ADC转换结束 */
    result = HAL_ADC_GetValue(&hadc3);                          /* 获取ADC转换结果 */
    HAL_ADC_Stop(&hadc3);                                       /* 停止ADC转换 */
    
    return result;
}

/**
 * @brief   获取内部温度传感器结果
 * @param   无
 * @retval  温度值（单位：摄氏度）
 */
int32_t adc3_get_temperature(void)
{
    uint32_t result;
    
    result = adc3_get_result();
    return __HAL_ADC_CALC_TEMPERATURE(VDD_VALUE, result, ADC_RESOLUTION_16B);       /* hal库温度转换函数 */
}

/**
 * @brief   均值滤波获取温度的结果
 * @param   times: 均值滤波的原始数据个数
 * @retval  ADC结果
 */
float adc_get_temperature_average(uint8_t times)
{
    float sum_temperature = 0;
    uint8_t index;
    float temperature;
    
    for (index=0; index<times; index++)
    {
        sum_temperature += adc3_get_temperature();
    }
    
    temperature = sum_temperature / times;
    
    return temperature;
}       
