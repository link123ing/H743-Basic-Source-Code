#include "bsp_delay_us.h"


void Delay_us(uint16_t nus)
{
	__HAL_TIM_SetCounter(&htim14,0);
	__HAL_TIM_ENABLE(&htim14);
	while (__HAL_TIM_GET_COUNTER(&htim14) < nus)
	{
	}
	__HAL_TIM_DISABLE(&htim14);

}

