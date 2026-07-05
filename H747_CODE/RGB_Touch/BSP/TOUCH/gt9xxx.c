/**
 * @file        gt9xxx.c
 * @brief       GT9xxx capacitive touch controller driver.
 *
 * This driver is written to be self-contained for the current CubeMX project:
 * - Software I2C (GPIO bit-bang) via ctiic.c
 * - Polling scan (no external interrupt dependency)
 * - Coordinate mapping using lcddev.dir and lcddev.id hints (based on your example logic)
 */
#include "gt9xxx.h"
#include "ctiic.h"
#include "touch.h"

#include "lcd.h"
#include "bsp_delay.h"

#include <stdio.h>
#include <string.h>

/* Default point number for most GT9xxx chips in 5-point mode. */
static uint8_t g_gt_tnum = 5;

uint8_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t ret = 0;

    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);
    ret = ct_iic_wait_ack();
    if (ret) { ct_iic_stop(); return ret; }

    ct_iic_send_byte((uint8_t)(reg >> 8));
    ret = ct_iic_wait_ack();
    if (ret) { ct_iic_stop(); return ret; }

    ct_iic_send_byte((uint8_t)(reg & 0xFF));
    ret = ct_iic_wait_ack();
    if (ret) { ct_iic_stop(); return ret; }

    for (uint8_t i = 0; i < len; i++)
    {
        ct_iic_send_byte(buf[i]);
        ret = ct_iic_wait_ack();
        if (ret) break;
    }

    ct_iic_stop();
    return ret;
}

void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);
    ct_iic_wait_ack();
    ct_iic_send_byte((uint8_t)(reg >> 8));
    ct_iic_wait_ack();
    ct_iic_send_byte((uint8_t)(reg & 0xFF));
    ct_iic_wait_ack();

    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_RD);
    ct_iic_wait_ack();

    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }

    ct_iic_stop();
}

uint8_t gt9xxx_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    GT9XXX_RST_GPIO_CLK_ENABLE();
    GT9XXX_INT_GPIO_CLK_ENABLE();

    /* RST as push-pull output */
    gpio_init_struct.Pin = GT9XXX_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GT9XXX_RST_GPIO_PORT, &gpio_init_struct);

    /* INT as input (polling scan does not strictly need it) */
    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);

    ct_iic_init();

    /* Hardware reset (same sequence as 实验24) */
    GT9XXX_RST(0);
    delay_ms(30);
    GT9XXX_RST(1);
    delay_ms(30);

    /* INT: floating input after reset */
    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);

    delay_ms(100);

    uint8_t pid_raw[5] = {0};
    gt9xxx_rd_reg(GT9XXX_PID_REG, pid_raw, 4);
    pid_raw[4] = 0;

    printf("CTP ID:%s\r\n", pid_raw);

    g_gt_tnum = 5;
    if (strcmp((char *)pid_raw, "911") && strcmp((char *)pid_raw, "9147") &&
        strcmp((char *)pid_raw, "1158") && strcmp((char *)pid_raw, "9271") &&
        strcmp((char *)pid_raw, "967"))
    {
        return 1;
    }

    if (strcmp((char *)pid_raw, "9271") == 0)
    {
        g_gt_tnum = 10;
    }

    /* Soft reset: 0x02 then 0x00 */
    uint8_t temp[1];
    temp[0] = 0x02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);
    delay_ms(10);

    temp[0] = 0x00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);

    return 0;
}

static const uint16_t GT9XXX_TPX_TBL[10] = {
    GT9XXX_TP1_REG,  GT9XXX_TP2_REG,  GT9XXX_TP3_REG,  GT9XXX_TP4_REG,  GT9XXX_TP5_REG,
    GT9XXX_TP6_REG,  GT9XXX_TP7_REG,  GT9XXX_TP8_REG,  GT9XXX_TP9_REG,  GT9XXX_TP10_REG
};

uint8_t gt9xxx_scan(uint8_t mode_unused)
{
    (void)mode_unused;

    uint8_t gst = 0;
    uint8_t i = 0;
    uint16_t temp = 0;
    uint16_t tempsta = 0;

    /* Reduce CPU load: poll every 10 calls */
    static uint8_t poll_div = 0;
    poll_div++;

    if (!((poll_div % 10) == 0 || poll_div < 10))
        return 0;

    /* Read touch status */
    gt9xxx_rd_reg(GT9XXX_GSTID_REG, &gst, 1);

    uint8_t point_count = gst & 0x0F;

    if ((gst & 0x80U) && (point_count <= g_gt_tnum))
    {
        i = 0;
        gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1); /* clear flag */

        if (point_count > 0 && point_count <= g_gt_tnum)
        {
            temp = 0xFFFFU << point_count;
            tempsta = tp_dev.sta;
            tp_dev.sta = (uint16_t)(~temp) | TP_PRES_DOWN | TP_CATH_PRES;

            /* keep previous first point at last position like your example */
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];

            for (i = 0; i < g_gt_tnum; i++)
            {
                if (tp_dev.sta & (1U << i))
                {
                    uint8_t buf[4] = {0};
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);

                    /* Raw X/Y from GT9xxx register bytes */
                    uint16_t raw_x = (uint16_t)((buf[3] << 8) | buf[2]);
                    uint16_t raw_y = (uint16_t)((buf[1] << 8) | buf[0]);

                    /* Coordinate mapping:
                     * Use same branching idea as your example code to maximize compatibility.
                     */
                    if (lcddev.id == 0x5510 || lcddev.id == 0x5310 ||
                        lcddev.id == 0x7796 || lcddev.id == 0x7789 ||
                        lcddev.id == 0x9806)
                    {
                        if (tp_dev.touchtype & 0x01U)
                        {
                            /* horizontal: mirror X */
                            tp_dev.x[i] = (uint16_t)(lcddev.width - raw_x);
                            tp_dev.y[i] = raw_y;
                        }
                        else
                        {
                            /* vertical: swap X/Y */
                            tp_dev.x[i] = raw_y;
                            tp_dev.y[i] = raw_x;
                        }
                    }
                    else
                    {
                        if (tp_dev.touchtype & 0x01U)
                        {
                            /* horizontal: swap X/Y */
                            tp_dev.x[i] = raw_y;
                            tp_dev.y[i] = raw_x;
                        }
                        else
                        {
                            /* vertical: mirror X */
                            tp_dev.x[i] = (uint16_t)(lcddev.width - raw_x);
                            tp_dev.y[i] = raw_y;
                        }
                    }
                }
            }

            /* Validate first point */
            if (tp_dev.x[0] > lcddev.width || tp_dev.y[0] > lcddev.height)
            {
                if (point_count > 1)
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    poll_div = 0;
                }
                else
                {
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                    tp_dev.sta = tempsta; /* restore */
                    gst = 0x80;
                }
            }
            else
            {
                poll_div = 0;
            }
        }
    }
    else if ((gst & 0x8FU) == 0x80U)
    {
        /* No touch: handle press/release state */
        if (tp_dev.sta & TP_PRES_DOWN)
        {
            tp_dev.sta &= ~TP_PRES_DOWN;
        }
        else
        {
            tp_dev.x[0] = 0xFFFFU;
            tp_dev.y[0] = 0xFFFFU;
            tp_dev.sta &= 0xE000U;
        }
    }

    if (poll_div > 240)
        poll_div = 10;

    return (tp_dev.sta & TP_PRES_DOWN) ? 1 : 0;
}

