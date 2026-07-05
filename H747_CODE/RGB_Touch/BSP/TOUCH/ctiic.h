/**
 * @file        ctiic.h
 * @brief       Capacitive touch software I2C (bit-bang) interface
 * @details     This implementation uses GPIO bit-banging instead of hardware I2C.
 */
#ifndef __CTIIC_H
#define __CTIIC_H

#include "main.h"

/* Touch controller I2C pins (from your wiring diagram) */
#define CT_IIC_SCL_GPIO_PORT            GPIOB
#define CT_IIC_SCL_GPIO_PIN             GPIO_PIN_10
#define CT_IIC_SCL_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define CT_IIC_SDA_GPIO_PORT            GPIOB
#define CT_IIC_SDA_GPIO_PIN             GPIO_PIN_11
#define CT_IIC_SDA_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

/* I2C line operations.
 * SDA is configured as open-drain, so writing 1 releases the line.
 */
#define CT_IIC_SCL(x) do{ \
    if ((x) != 0) HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_SET); \
    else           HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET); \
}while(0)

#define CT_IIC_SDA(x) do{ \
    if ((x) != 0) HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_SET); \
    else           HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET); \
}while(0)

#define CT_READ_SDA  HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN)

void ct_iic_init(void);
void ct_iic_start(void);
void ct_iic_stop(void);
uint8_t ct_iic_wait_ack(void);
void ct_iic_ack(void);
void ct_iic_nack(void);
void ct_iic_send_byte(uint8_t data);
uint8_t ct_iic_read_byte(uint8_t ack);

#endif /* __CTIIC_H */

