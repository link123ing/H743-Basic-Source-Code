/**
 ****************************************************************************************************
 * @file        sdram.h
 * @brief       SDRAM driver (STM32H747XIH6 + IS42S32800J, FMC Bank2)
 ****************************************************************************************************
 */

#ifndef BSP_SDRAM_H
#define BSP_SDRAM_H

#include "main.h"
#include "fmc.h"

#define BANK6_SDRAM_ADDR           ((uint32_t)0xD0000000U)

/* SDRAM mode register */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000U)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001U)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002U)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004U)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000U)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008U)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020U)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030U)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000U)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000U)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200U)

/* FMC ker_ck=240MHz, SDCLK=ker_ck/2=120MHz, refresh 64ms, 4096 rows */
#define SDRAM_REFRESH_COUNT          (1855U)

void sdram_init(void);
void sdram_initialization_sequence(void);
uint8_t sdram_send_cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval);
void sdram_test(void);

#endif /* BSP_SDRAM_H */
