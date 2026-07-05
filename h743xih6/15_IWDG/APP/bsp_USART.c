#include "bsp_USART.h"

//创建串口接收数组
uint8_t USART1_Receive_Buffer[BUFFER_SIZE];

//创建环形缓冲区句柄
ringbuffer_t usart1_rb;


//创建环形缓冲区处理数组
uint8_t USART1_Ring_Buff[BUFFER_SIZE];

volatile uint8_t g_feed_watchdog_enable = 1;  // 1=喂狗  0=不喂狗


int Uart_Printf(UART_HandleTypeDef *huart,const char* format,...)
{
	char buf[256];
	va_list args;
	int len;
	va_start(args,format);
	len = vsnprintf(buf,sizeof(buf),format,args);
	va_end(args);
	HAL_UART_Transmit(huart,(uint8_t*)buf,(uint16_t)len,500);
	return 0;
}


void Uart_Init(void)
{
	//环形缓冲区初始化
	ringbuffer_init(&usart1_rb);
	//开启DMA空闲中断
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,USART1_Receive_Buffer,BUFFER_SIZE);
	//关闭DMA半中断
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	
}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		SCB_InvalidateDCache_by_Addr((uint32_t*)USART1_Receive_Buffer, Size);
		//如果环形缓冲区非空
		if(!ringbuffer_is_full(&usart1_rb))
		{
			//写入环形缓冲区数组
			ringbuffer_write(&usart1_rb,USART1_Receive_Buffer,Size);
		}
		//清空接收数组
		memset(USART1_Receive_Buffer,0,sizeof(USART1_Receive_Buffer));
		//重新开启DMA空闲中断和关闭半中断
		SCB_CleanDCache_by_Addr((uint32_t*)USART1_Receive_Buffer, BUFFER_SIZE);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,USART1_Receive_Buffer,BUFFER_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	}
}


void uart_proc(void)
{
	//如果环形缓冲区非空
	if(!ringbuffer_is_empty(&usart1_rb))
	{
		ringbuffer_read(&usart1_rb,USART1_Receive_Buffer,usart1_rb.itemCount);
		if(strstr((char*)USART1_Receive_Buffer, "switch") != NULL)
		{
			// 切换喂狗状态
			g_feed_watchdog_enable ^= 1;     
			Uart_Printf(&huart1, "state:%d",g_feed_watchdog_enable);
		}
//		Uart_Printf(&huart1,"%s\n",USART1_Receive_Buffer);
		memset(USART1_Receive_Buffer,0,sizeof(uint8_t)*BUFFER_SIZE);
		// 打印提示
	}
	if(g_feed_watchdog_enable)
		{
			Uart_Printf(&huart1, "→ 已开启喂狗\r\n");
			HAL_IWDG_Refresh(&hiwdg1);  // 喂狗
		}
		else{
			Uart_Printf(&huart1, "→ 已关闭喂狗，2秒后系统复位...\r\n");
		}
}

//在某些情况下用不了微库，且Uart_Printf内的数组造成堆栈溢出，必须使用fputc重定向时
//保留下列代码段可以不需要微库依然做到串口的重定向
//1.关闭半主机模式
#pragma import(__use_no_semihosting)  
//2.库函数需要的底层接口，给个空实现
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}   
//3.定义标准文件结构体，让printf不依赖电脑
struct __FILE 
{ 
	int handle; 

}; 
FILE __stdout;       
//4.关闭系统退出函数，避免程序跑飞
void _sys_exit(int x) 
{ 
	x = x; 
} 

/*
	讲一下微库干了什么：
	正常情况下要用printf函数会加上标准C库
	可是标准C库默认走半主机模式
	底层会调用 BKPT 断点指令 → 让调试器转发给电脑显示
	但是调试器如果不响应这个指令就会导致它卡死

	微库其实是精简C库
	默认直接禁用半主机，不走BKPT

	而上述代码走的仍然是标准C库
	只是我手动禁止了半主机模式而已

*/

