#include "sdram.h"
#include "main.h"

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
  __IO uint32_t temp =  (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1|	/* 设置突发长度:2(可以是1/2/4/8) */
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
  /* 所以,COUNT=64*1000*133/4096-20=2168 */    
  HAL_SDRAM_ProgramRefreshRate(&hsdram1,2168);
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