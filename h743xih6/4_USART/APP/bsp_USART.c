#include "bsp_USART.h"

uint16_t uart_rx_index;//接收缓存索引
uint32_t uart_rx_ticks;//接收缓存时间戳
uint8_t uart_rx_buffer[256];//接收缓存

/*
	这边提供了两种串口打印的方法
	1.fputc进行串口重定向，后续可以直接使用printf进行打印
	但是有个缺点就是它重定向后只能使用固定的串口，不够灵活
	
	2.手动构造类printf函数：Uart_Printf
	优点是足够灵活，但是缺点是内部数组如果开的太大容易造成栈溢出
	后续的串口例程统一使用Uart_Printf演示
	
	注意：串口重定向需要开启微库或者添加本页面最下方的代码
*/
typedef struct __FILE FILE;
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);	
	return (ch);
}


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

//开启串口接收中断，并绑定接收数组
void Uart_Init(void)
{

	HAL_UART_Receive_IT(&huart1,uart_rx_buffer,1);
	
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  //接收中断回调函数
{
	if (huart -> Instance == USART1)  //确认接收的串口
	{
		uart_rx_ticks = uwTick;          //开启串口接收定时计数器
		uart_rx_index ++ ;		   //接收数据指针自增，每接收到一个数据指针都自增一次
		HAL_UART_Receive_IT(&huart1, &uart_rx_buffer[uart_rx_index], 1); //重新开启接收中断
	}
}


void uart_proc(void)
{
	//如果接收索引为0，说明没有数据需要处理，直接返回
	if(uart_rx_index == 0) return;
	//如果从最后一次接收到数据到现在已经超过100ms
	if(uwTick - uart_rx_ticks > 10) // 100ms内没有收到新的数据
	{
		printf("uart data: %s\r\n", uart_rx_buffer);
		//清空接收缓冲区，将接收索引置零
		memset(uart_rx_buffer, 0, uart_rx_index);
		uart_rx_index = 0;
		//将UART接收缓冲区指针重置为接收缓冲区的起始位置
		huart1.pRxBuffPtr = uart_rx_buffer;
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

