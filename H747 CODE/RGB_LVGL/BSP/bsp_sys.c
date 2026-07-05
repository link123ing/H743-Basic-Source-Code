/**
 ****************************************************************************************************
 * @file        bsp_sys.c
 * @brief       BSP system helpers
 ****************************************************************************************************
 */

#include "bsp_sys.h"

void bsp_cache_enable(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
    SCB->CACR |= 1U << 2;
}
