/**
 * @file lv_port_indev.c
 * @brief LVGL input port stub (no touch until TOUCH BSP is added)
 */
#include "lv_port_indev.h"
#include "lvgl.h"

void lv_port_indev_init(void)
{
    /* Touch is optional; add GT9xxx driver and register pointer indev here. */
}
