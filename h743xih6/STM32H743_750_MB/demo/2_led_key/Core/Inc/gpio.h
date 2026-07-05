/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#define LED_R(n)				(n?HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_RESET))
#define LED_R_TOGGLE(void)		HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin)   /* LED_R电平翻转 */

#define LED_G(n)				(n?HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_RESET))
#define LED_G_TOGGLE(void)   	HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin)	/* LED_G电平翻转 */

#define LED_B(n)				(n?HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,GPIO_PIN_RESET))
#define LED_B_TOGGLE(void)   	HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin)	/* LED_B电平翻转 */


#define KEY0        HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)                     /* 读取KEY0引脚 */
#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)                     /* 读取KEY1引脚 */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)                     /* 读取KEY2引脚 */
#define WKUP       HAL_GPIO_ReadPin(WKUP_GPIO_Port, WKUP_Pin)                      /* 读取WKUP引脚 */

#define KEY0_PRES    1                  /* KEY0按下 */
#define KEY1_PRES    2                  /* KEY1按下 */
#define KEY2_PRES    3                  /* KEY2按下 */
#define WKUP_PRES    4                  /* KEY_UP按下(即WK_UP) */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

uint8_t key_scan(uint8_t mode);         /* 按键扫描函数 */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

