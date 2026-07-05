/**
 * @file        touch.h
 * @brief       Unified touch interface (GT9xxx / FT5206).
 */
#ifndef __TOUCH_H
#define __TOUCH_H

#include "main.h"
#include "lcd.h"

#define CT_MAX_TOUCH  10

/* Touch status bits (compatible with your example code style) */
#define TP_PRES_DOWN    0x8000U   /* pressed */
#define TP_CATH_PRES    0x4000U   /* key/valid point pressed flag (capacitive) */

typedef struct
{
    uint8_t (*init)(void);
    uint8_t (*scan)(uint8_t mode);
    void (*adjust)(void);

    uint16_t x[CT_MAX_TOUCH];
    uint16_t y[CT_MAX_TOUCH];
    uint16_t sta;

    /* bit0: 0=portrait mapping, 1=landscape mapping (your example uses lcddev.dir&0x01) */
    /* bit7: 1=capacitive mode */
    uint8_t touchtype;
} _m_tp_dev;

extern _m_tp_dev tp_dev;

/* Public API */
uint8_t tp_init(void);
uint8_t tp_scan(uint8_t mode);

#endif /* __TOUCH_H */

