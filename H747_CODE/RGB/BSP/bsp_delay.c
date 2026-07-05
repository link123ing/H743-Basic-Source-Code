/**
 ****************************************************************************************************
 * @file        bsp_delay.c
 * @brief       Simple delay helpers for BSP drivers
 ****************************************************************************************************
 */

#include "bsp_delay.h"

void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void delay_us(uint32_t us)
{
    uint32_t ticks = (SystemCoreClock / 1000000U) * us / 5U;

    while (ticks-- > 0U)
    {
        __NOP();
    }
}
