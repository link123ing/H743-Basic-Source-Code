/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.h
  * @brief   This file contains all the function prototypes for
  *          the quadspi.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __QUADSPI_H__
#define __QUADSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN Private defines */

/* W25Q256JV基本信息 */
#define QSPI_FLASH_SIZE     25                      /* Flash大小，2^25 = 32MB*/
#define QSPI_SECTOR_SIZE    (4 * 1024)              /* 扇区大小，4KB */
#define QSPI_PAGE_SIZE      256        				/* 页大小，256字节 */
#define QSPI_END_ADDR    	(1 << QSPI_FLASH_SIZE)  /* 末尾地址 */
#define QSPI_FLASH_SIZES    32*1024*1024            /* Flash大小，2^25 = 32MB*/

/* W25Q256JV相关命令 */
#define WRITE_ENABLE_CMD      0x06         /* 写使能指令 */  
#define READ_ID_CMD2          0x9F         /* 读取ID命令 */  
#define READ_STATUS_REG_CMD   0x05         /* 读取状态命令 */ 
#define BULK_ERASE_CMD        0xC7         /* 整个芯片擦除命令 */ 
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD      0x21    /* 32bit地址扇区擦除指令, 4KB */
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD    0x34    /* 32bit地址的4线快速写入命令 */
#define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC    /* 32bit地址的4线快速读取命令 */

/* USER CODE END Private defines */

void MX_QUADSPI_Init(void);

/* USER CODE BEGIN Prototypes */

void QSPI_MemoryMapped(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __QUADSPI_H__ */

