/**
 * @file        touch.c
 * @brief       Touch driver frontend: auto-detect and wrap GT9xxx / FT5206.
 */
#include "touch.h"

#include "gt9xxx.h"
#include "ft5206.h"

/* Default tp_dev instance (scan function pointer will be replaced in tp_init()). */
_m_tp_dev tp_dev =
{
    .init = tp_init,
    .scan = 0,
    .adjust = 0,
    .x = {0},
    .y = {0},
    .sta = 0,
    .touchtype = 0
};

static uint8_t tp_scan_dummy(uint8_t mode)
{
    (void)mode;
    return 0;
}

static void tp_adjust_dummy(void)
{
    /* No calibration implemented for capacitive touch in this simplified BSP integration. */
}

uint8_t tp_init(void)
{
    /* Orientation hint comes from LCD direction (same approach as your example). */
    tp_dev.touchtype = (uint8_t)(lcddev.dir & 0x01U);

    /* init scan pointer defaults */
    tp_dev.scan = tp_scan_dummy;
    tp_dev.adjust = tp_adjust_dummy;

    tp_dev.sta = 0;
    for (uint8_t i = 0; i < CT_MAX_TOUCH; i++)
    {
        tp_dev.x[i] = 0xFFFFU;
        tp_dev.y[i] = 0xFFFFU;
    }

    /* Try GT9xxx first */
    if (gt9xxx_init() == 0)
    {
        tp_dev.scan = gt9xxx_scan;
        tp_dev.touchtype |= 0x80U; /* capacitive */
        return 0;
    }

    /* Fallback to FT5206/FT5426 */
    if (ft5206_init() == 0)
    {
        tp_dev.scan = ft5206_scan;
        tp_dev.touchtype |= 0x80U; /* capacitive */
        return 0;
    }

    return 1;
}

uint8_t tp_scan(uint8_t mode)
{
    if (tp_dev.scan)
        return tp_dev.scan(mode);
    return 0;
}

