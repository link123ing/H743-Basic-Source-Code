/**
 * @file        ft5206.h
 * @brief       FT5206/FT5426 capacitive touch controller driver.
 */
#ifndef __FT5206_H
#define __FT5206_H

#include "main.h"
#include <stdint.h>

#define FT5206_RST_GPIO_PORT            GPIOB
#define FT5206_RST_GPIO_PIN             GPIO_PIN_12
#define FT5206_RST_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define FT5206_INT_GPIO_PORT            GPIOB
#define FT5206_INT_GPIO_PIN             GPIO_PIN_5
#define FT5206_INT_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define FT5206_RST(x) do{ \
    if ((x) != 0) HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_SET); \
    else           HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_RESET); \
}while(0)

#define FT5206_INT()  HAL_GPIO_ReadPin(FT5206_INT_GPIO_PORT, FT5206_INT_GPIO_PIN)

/* I2C command bytes */
#define FT5206_CMD_WR       0x70
#define FT5206_CMD_RD       0x71

/* FT5206 registers (16-bit address in this software I2C protocol) */
#define FT5206_DEVIDE_MODE          0x00
#define FT5206_REG_NUM_FINGER       0x02

#define FT5206_TP1_REG              0x03
#define FT5206_TP2_REG              0x09
#define FT5206_TP3_REG              0x0F
#define FT5206_TP4_REG              0x15
#define FT5206_TP5_REG              0x1B

#define FT5206_ID_G_LIB_VERSION     0xA1
#define FT5206_ID_G_MODE            0xA4
#define FT5206_ID_G_THGROUP         0x80
#define FT5206_ID_G_PERIODACTIVE    0x88

uint8_t ft5206_init(void);
uint8_t ft5206_scan(uint8_t mode);

uint8_t ft5206_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len);
void ft5206_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len);

#endif /* __FT5206_H */

