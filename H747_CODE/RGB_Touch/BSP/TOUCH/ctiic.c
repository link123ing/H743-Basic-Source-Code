/**
 * @file        ctiic.c
 * @brief       Capacitive touch software I2C (bit-bang) implementation.
 */
#include "ctiic.h"
#include "bsp_delay.h"

static void ct_iic_delay(void)
{
    /* Simple timing; tune if needed for your touch IC. */
    delay_us(2);
}

void ct_iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    CT_IIC_SCL_GPIO_CLK_ENABLE();
    CT_IIC_SDA_GPIO_CLK_ENABLE();

    /* SCL: push-pull output */
    gpio_init_struct.Pin = CT_IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(CT_IIC_SCL_GPIO_PORT, &gpio_init_struct);

    /* SDA: open-drain output (release by writing 1) */
    gpio_init_struct.Pin = CT_IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(CT_IIC_SDA_GPIO_PORT, &gpio_init_struct);

    ct_iic_stop();
}

void ct_iic_start(void)
{
    CT_IIC_SDA(1);
    CT_IIC_SCL(1);
    ct_iic_delay();

    /* START: SDA high -> low while SCL high */
    CT_IIC_SDA(0);
    ct_iic_delay();

    /* Hold SCL low for next transfer */
    CT_IIC_SCL(0);
    ct_iic_delay();
}

void ct_iic_stop(void)
{
    /* STOP: SDA low -> high while SCL high */
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SDA(1);
    ct_iic_delay();
}

uint8_t ct_iic_wait_ack(void)
{
    uint16_t waittime = 0;

    /* Release SDA (open-drain -> high-Z) */
    CT_IIC_SDA(1);
    ct_iic_delay();

    CT_IIC_SCL(1);
    ct_iic_delay();

    /* Wait until SDA is pulled low by slave (ACK) */
    while (CT_READ_SDA != 0)
    {
        waittime++;
        if (waittime > 250)
        {
            ct_iic_stop();
            return 1;
        }
        ct_iic_delay();
    }

    CT_IIC_SCL(0);
    ct_iic_delay();
    return 0;
}

void ct_iic_ack(void)
{
    CT_IIC_SDA(0);
    ct_iic_delay();

    CT_IIC_SCL(1);
    ct_iic_delay();

    CT_IIC_SCL(0);
    ct_iic_delay();
    CT_IIC_SDA(1);
    ct_iic_delay();
}

void ct_iic_nack(void)
{
    CT_IIC_SDA(1);
    ct_iic_delay();

    CT_IIC_SCL(1);
    ct_iic_delay();

    CT_IIC_SCL(0);
    ct_iic_delay();
}

void ct_iic_send_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        /* MSB first */
        CT_IIC_SDA((data & 0x80U) ? 1 : 0);
        ct_iic_delay();

        CT_IIC_SCL(1);
        ct_iic_delay();

        CT_IIC_SCL(0);
        ct_iic_delay();

        data <<= 1;
    }

    /* Release SDA after byte */
    CT_IIC_SDA(1);
}

uint8_t ct_iic_read_byte(uint8_t ack)
{
    uint8_t receive = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        receive <<= 1;

        CT_IIC_SCL(1);
        ct_iic_delay();

        if (CT_READ_SDA != 0)
        {
            receive |= 1;
        }

        CT_IIC_SCL(0);
        ct_iic_delay();
    }

    if (ack)
        ct_iic_ack();
    else
        ct_iic_nack();

    return receive;
}

