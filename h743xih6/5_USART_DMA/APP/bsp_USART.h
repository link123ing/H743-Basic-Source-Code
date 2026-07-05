#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "bsp_system.h"

int Uart_Printf(UART_HandleTypeDef *huart,const char* format,...);

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
void Uart_Init(void);
void uart_proc(void);


#endif
