#include "bsp_TimCounter.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        Uart_Printf(&huart1,"TIM1 Interrupt\r\n");
    }
}
