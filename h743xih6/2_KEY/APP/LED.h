#ifndef __LED_H
#define __LED_H

#include "bsp_system.h"

/*------------------------------------------ LED配置宏 ----------------------------------*/

#define LED_R_PIN            			 GPIO_PIN_0        				 	// LED1 引脚      
#define LED_R_PORT           			 GPIOB                 			 	// LED1 GPIO端口     
#define __HAL_RCC_LED_RG_CLK_ENABLE    __HAL_RCC_GPIOB_CLK_ENABLE() 	// LED1 GPIO端口时钟

#define LED_G_PIN            			 GPIO_PIN_1        				 	// LED1 引脚      
#define LED_G_PORT           			 GPIOB                 			 	// LED1 GPIO端口     

#define LED_B_PIN            			 GPIO_PIN_3        				 	// LED1 引脚      
#define LED_B_PORT           			 GPIOE                 			 	// LED1 GPIO端口     
#define __HAL_RCC_LED_B_CLK_ENABLE    __HAL_RCC_GPIOE_CLK_ENABLE() 	// LED1 GPIO端口时钟

/*----------------------------------------- LED控制宏 ----------------------------------*/
						
#define LEDR_ON 	  	HAL_GPIO_WritePin(LED_R_PORT, LED_R_PIN, GPIO_PIN_RESET)		// 输出低电平，点亮LED1	
#define LEDR_OFF 	  	HAL_GPIO_WritePin(LED_R_PORT, LED_R_PIN, GPIO_PIN_SET)			// 输出高电平，关闭LED1	
#define LEDR_Toggle	HAL_GPIO_TogglePin(LED_R_PORT,LED_R_PIN);							// 翻转IO口状态

#define LEDG_ON 	  	HAL_GPIO_WritePin(LED_G_PORT, LED_G_PIN, GPIO_PIN_RESET)		// 输出低电平，点亮LED1	
#define LEDG_OFF 	  	HAL_GPIO_WritePin(LED_G_PORT, LED_G_PIN, GPIO_PIN_SET)			// 输出高电平，关闭LED1	
#define LEDG_Toggle	HAL_GPIO_TogglePin(LED_G_PORT,LED_G_PIN);							// 翻转IO口状态

#define LEDB_ON 	  	HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, GPIO_PIN_RESET)		// 输出低电平，点亮LED1	
#define LEDB_OFF 	  	HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, GPIO_PIN_SET)			// 输出高电平，关闭LED1	
#define LEDB_Toggle	HAL_GPIO_TogglePin(LED_B_PORT,LED_B_PIN);							// 翻转IO口状态

/*---------------------------------------- 函数声明 ------------------------------------*/

void LED_Init(void);




#endif
