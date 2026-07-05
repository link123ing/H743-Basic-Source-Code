/**
 * @file        ft5206.c
 * @brief       FT5206/FT5426 capacitive touch controller driver.
 *
 * This driver uses the same software I2C backend (ctiic.c).
 */
#include "ft5206.h"
#include "ctiic.h"
#include "touch.h"

#include "lcd.h"
#include "bsp_delay.h"

#include <string.h>

uint8_t ft5206_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t ret = 0;

    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);
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

void ft5206_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);
    ct_iic_wait_ack();
    ct_iic_send_byte((uint8_t)(reg & 0xFF));
    ct_iic_wait_ack();

    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_RD);
    ct_iic_wait_ack();

    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }

    ct_iic_stop();
}

uint8_t ft5206_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    FT5206_RST_GPIO_CLK_ENABLE();
    FT5206_INT_GPIO_CLK_ENABLE();

    /* RST output */
    gpio_init_struct.Pin = FT5206_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(FT5206_RST_GPIO_PORT, &gpio_init_struct);

    /* INT input */
    gpio_init_struct.Pin = FT5206_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(FT5206_INT_GPIO_PORT, &gpio_init_struct);

    ct_iic_init();

    FT5206_RST(0);
    delay_ms(20);
    FT5206_RST(1);
    delay_ms(50);

    uint8_t temp[2] = {0};

    /* Enter normal mode */
    temp[0] = 0;
    ft5206_wr_reg(FT5206_DEVIDE_MODE, temp, 1);
    ft5206_wr_reg(FT5206_ID_G_MODE, temp, 1);

    /* Tune touch sensitivity (from your example) */
    temp[0] = 22;
    ft5206_wr_reg(FT5206_ID_G_THGROUP, temp, 1);
    temp[0] = 12;
    ft5206_wr_reg(FT5206_ID_G_PERIODACTIVE, temp, 1);

    /* Read version bytes */
    ft5206_rd_reg(FT5206_ID_G_LIB_VERSION, temp, 2);

    /* Basic verification: accept known FT5206 version patterns */
    uint8_t b0 = temp[0];
    uint8_t b1 = temp[1];

    if ((b0 == 0x30 && b1 == 0x03) || (b1 == 0x01) || (b1 == 0x02) || (b0 == 0x00 && b1 == 0x00))
        return 0;

    return 1;
}

static const uint16_t FT5206_TPX_TBL[5] = {
    FT5206_TP1_REG, FT5206_TP2_REG, FT5206_TP3_REG, FT5206_TP4_REG, FT5206_TP5_REG
};

uint8_t ft5206_scan(uint8_t mode_unused)
{
    (void)mode_unused;

    uint8_t sta = 0;
    uint8_t buf[4] = {0};
    uint8_t res = 0;

    /* Reduce CPU load: poll every 10 calls */
    static uint8_t poll_div = 0;
    poll_div++;
    if (!((poll_div % 10) == 0 || poll_div < 10))
        return 0;

    ft5206_rd_reg(FT5206_REG_NUM_FINGER, &sta, 1);
    uint8_t point_num = sta & 0x0F;

    if (point_num && point_num < 6)
    {
        uint16_t temp_mask = (uint16_t)(0xFFFFU << point_num);
        tp_dev.sta = (uint16_t)(~temp_mask) | TP_PRES_DOWN | TP_CATH_PRES;
        delay_ms(4);

        for (uint8_t i = 0; i < 5; i++)
        {
            if (tp_dev.sta & (1U << i))
            {
                ft5206_rd_reg(FT5206_TPX_TBL[i], buf, 4);

                if (tp_dev.touchtype & 0x01U)
                {
                    /* Horizontal mapping: follow your example */
                    tp_dev.y[i] = (uint16_t)(((buf[0] & 0x0FU) << 8) | buf[1]);
                    tp_dev.x[i] = (uint16_t)(((buf[2] & 0x0FU) << 8) | buf[3]);
                }
                else
                {
                    tp_dev.x[i] = (uint16_t)(lcddev.width - (((buf[0] & 0x0FU) << 8) | buf[1]));
                    tp_dev.y[i] = (uint16_t)(((buf[2] & 0x0FU) << 8) | buf[3]);
                }

                /* Only accept "contact" events */
                if ((buf[0] & 0xF0U) != 0x80U)
                {
                    tp_dev.x[i] = 0;
                    tp_dev.y[i] = 0;
                }
            }
        }

        if (!(tp_dev.x[0] == 0 && tp_dev.y[0] == 0))
            res = 1;

        poll_div = 0;
    }
    else
    {
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

    return res;
}

