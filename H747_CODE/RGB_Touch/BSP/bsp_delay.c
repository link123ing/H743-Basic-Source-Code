/**
 ****************************************************************************************************
 * @file        bsp_delay.c
 * @brief       Delay helpers for BSP drivers (SysTick-based us delay, same idea as 实验24)
 ****************************************************************************************************
 */

#include "bsp_delay.h"

static uint32_t g_fac_us = 0U;

void delay_init(uint16_t sysclk_mhz)
{
    g_fac_us = sysclk_mhz;
}

void delay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0U;
    uint32_t reload;

    if (g_fac_us == 0U)
    {
        g_fac_us = (uint16_t)(SystemCoreClock / 1000000U);
    }

    reload = SysTick->LOAD;
    ticks = us * g_fac_us;
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

void delay_ms(uint32_t ms)
{
    while (ms-- > 0U)
    {
        delay_us(1000U);
    }
}
