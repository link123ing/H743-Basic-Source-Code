/**
 * @file lvgl_demo.c
 * @brief LVGL application on FreeRTOS
 */
#include "lvgl_demo.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demo_stress.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#define LVGL_TASK_PRIO      (osPriorityAboveNormal)
#define LVGL_TASK_STACK     (4096U * 4U)

static osThreadId_t lvglTaskHandle;

static void lvgl_task(void *argument);

void lvgl_app_init(void)
{
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
}

void lvgl_start_task(void)
{
    const osThreadAttr_t lvgl_task_attributes = {
        .name = "lvgl_task",
        .stack_size = LVGL_TASK_STACK,
        .priority = LVGL_TASK_PRIO,
    };

    lvglTaskHandle = osThreadNew(lvgl_task, NULL, &lvgl_task_attributes);
}

static void lvgl_task(void *argument)
{
    (void)argument;

    lv_demo_stress();

    for (;;)
    {
        lv_timer_handler();
        osDelay(1);
    }
}

void vApplicationTickHook(void)
{
    lv_tick_inc(1);
}
