#include "sdram.h"
#include "main.h"

__IO uint32_t stimer,etimer;
__IO uint32_t write_timer=0,read_timer=0;

/**
 * @brief       发送SDRAM初始化序列
 * @param       无
 * @retval      无
 */
void SDRAM_Init(void)
{
  FMC_SDRAM_CommandTypeDef  Command;    
  /* 第一步，给SDRAM提供时钟 */ 
  Command.CommandMode           = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget         = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber     = 1;
  Command.ModeRegisterDefinition= 0;
  
  HAL_SDRAM_SendCommand(&hsdram1, &Command,0xFFFF);
  
  /* 第二步，延迟至少100us */ 
  HAL_Delay(1);
  
  /* 第三步，对所有BNAK进行预充电 */ 
  Command.CommandMode           = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget         = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber     = 1;
  Command.ModeRegisterDefinition= 0;
  
  HAL_SDRAM_SendCommand(&hsdram1, &Command,0xFFFF); 
  
  /* 第四步，插入8个自动刷新周期 */ 
  Command.CommandMode           = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget         = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber     = 8;
  Command.ModeRegisterDefinition= 0;
  
  HAL_SDRAM_SendCommand(&hsdram1, &Command,0xFFFF); 
  
  /* 第五步，编程加载模式寄存器 */ 
  __IO uint32_t temp =  (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1|	/* 设置突发长度:1(可以是1/2/4/8) */
                        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	/* 设置突发类型:连续(可以是连续/交错) */
                        SDRAM_MODEREG_CAS_LATENCY_2           |	/* 设置CAS值:2(可以是2/3) */
                        SDRAM_MODEREG_OPERATING_MODE_STANDARD | /* 设置操作模式:0,标准模式 */
                        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;   /* 设置突发写模式:1,单点访问 */
                        
                             
  Command.CommandMode           = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget         = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber     = 1;
  Command.ModeRegisterDefinition= temp;
  
  HAL_SDRAM_SendCommand(&hsdram1, &Command,0xFFFF); 
 
  /* 第六步，配置自动刷新周期 */ 
  
  /* 刷新频率计数器(以SDCLK频率计数),计算方法:  */
  /* COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数 */
  /* 我们使用的SDRAM刷新周期为64ms,SDCLK=266/2=133Mhz,行数为4096(2^12) */
  /* 所以,COUNT=64*1000*133/4096-20=2058 */    
  HAL_SDRAM_ProgramRefreshRate(&hsdram1,2058);
}

/**
 * @brief       在指定地址(WriteAddr+FMC_SDRAM_ADDR)开始,连续写入n个字节
 * @param       pBuffer:字节指针
 * @param       WriteAddr:要写入的地址
 * @param       n:要写入的字节数
 * @retval      无
*/
void FMC_SDRAM_Write_Buffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *(__IO uint8_t*)(FMC_SDRAM_ADDR + WriteAddr) = *pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

/**
 * @brief       在指定地址((WriteAddr+FMC_SDRAM_ADDR))开始,连续读出n个字节.
 * @param       pBuffer:字节指针
 * @param       ReadAddr:要读出的起始地址
 * @param       n:要写入的字节数
 * @retval      无
*/
void FMC_SDRAM_Read_Buffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *pBuffer++ = *(__IO uint8_t*)(FMC_SDRAM_ADDR + ReadAddr);
        ReadAddr++;
    }
}

/**
 * @brief       SDRAM内存测试
 * @param       无
 * @retval      无
 */    
void FMC_SDRAM_Test(void)
{  
	uint32_t i=0,temp=0,sval=0;  	  
	
	for(i=0;i<32*1024*1024;i+=16*1024)      /* 每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节 */
	{
		*(__IO uint32_t*)(FMC_SDRAM_ADDR+i)=temp; 
		temp++;
	}	
    
 	for(i=0;i<32*1024*1024;i+=16*1024)      /* 依次读出之前写入的数据,进行校验 */
	{	
  		temp=*(uint32_t*)(FMC_SDRAM_ADDR+i);
		if(i==0)sval=temp;
 		else if(temp<=sval)                 /* 后面读出的数据一定要比第一次读到的数据大 */	 
        { 
            printf("SDRAM_ERROR\r\n");
            break;  
        }            
		printf("SDRAM Capacity:%dKB\r\n",(uint16_t)(temp-sval+1)*16);   /* 打印SDRAM容量 */
 	}					 
}

/*
*********************************************************************************************************
*	函 数 名: SDRAM_Checkfailed
*	功能说明: SDRAM读写出错时提示
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SDRAM_Checkfailed(void)
{
    printf("SDRAM ERROR\r\n");
    while(1);
}

/*
*********************************************************************************************************
*	函 数 名: SDRAM_WriteSpeed_Test
*	功能说明: 写SDRAM速度测试
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SDRAM_WriteSpeed_Test(void)
{
	uint32_t i, j=0;
	uint32_t *pBuf;
    
	pBuf = (uint32_t *)FMC_SDRAM_ADDR;
 	stimer=HAL_GetTick();
	
	for (i = 0; i < 1024*1024/4 ; i++) /* 以递增的方式写数据到SDRAM所有空间 */
	{
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;	

		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;	

		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;	

		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;
		*pBuf++ = j++;	
        
	}
    etimer=HAL_GetTick();               /* 暂存时间 */
	write_timer=etimer-stimer;               
	j = 0;                              /* 读取写入的是否出错 */
	pBuf = (uint32_t *)FMC_SDRAM_ADDR;
	for (i = 0; i < 1024*1024*8; i++)
	{
		if(*pBuf++ != j++)
		{
			printf("写入出错 j=%d\r\n", j);
			SDRAM_Checkfailed();
		}
	}
		
	printf("32MB数据写耗时: %dms,  写速度: %dMB/s\r\n",write_timer, (EXT_SDRAM_SIZE / 1024 /1024 * 1000) / write_timer);    /* 打印速度 */
}

/*
*********************************************************************************************************
*	函 数 名: SDRAM_ReadSpeed_Test
*	功能说明: 读SDRAM速度测试
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void SDRAM_ReadSpeed_Test(void)
{	
	uint32_t *pBuf,i;
	__IO uint32_t ulTemp; /* 设置为__IO类型，防止被MDK优化 */
    
	/* 设置初始化值并记下开始时间 */
	pBuf = (uint32_t *)FMC_SDRAM_ADDR;
    
    stimer=HAL_GetTick();
    
	/* 读取SDRAM所有空间数据 */	
	for (i = 0; i < 1024*1024/4 ; i++)
	{
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;

		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
		ulTemp = *pBuf++;
	}
    etimer=HAL_GetTick();               /* 暂存时间 */
	read_timer=etimer-stimer; 
    printf("32MB数据读耗时: %dms, 读速度: %dMB/s\r\n\r\n",read_timer, (EXT_SDRAM_SIZE / 1024 /1024 * 1000) / read_timer);  	/* 打印速度 */
}
