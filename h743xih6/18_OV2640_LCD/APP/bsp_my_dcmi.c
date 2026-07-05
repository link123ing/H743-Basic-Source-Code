#include "bsp_my_dcmi.h"

void DCMI_DMA_Init(uint32_t mem0addr, uint32_t mem1addr, uint32_t memsize, uint32_t memblen, uint32_t meminc)
{
		 __HAL_RCC_DMA1_CLK_ENABLE();                                        /* 使能DMA1时钟 */
    __HAL_LINKDMA(&hdcmi, DMA_Handle, hdma_dcmi);       /* 将DMA与DCMI联系起来 */
    __HAL_DMA_DISABLE_IT(&hdma_dcmi, DMA_IT_TC);                /* 先关闭DMA传输完成中断(否则在使用MCU屏的时候会出现花屏的情况) */

    hdma_dcmi.Instance = DMA1_Stream1;                          /* DMA1数据流1 */
    hdma_dcmi.Init.Request = DMA_REQUEST_DCMI;                  /* DCMI的DMA请求 */
    hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;            /* 外设到存储器 */
    hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;                /* 外设非增量模式 */
    hdma_dcmi.Init.MemInc = meminc;                             /* 存储器增量模式 */
    hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;   /* 外设数据长度:32位 */
    hdma_dcmi.Init.MemDataAlignment = memblen;                  /* 存储器数据长度:8/16/32位 */
    hdma_dcmi.Init.Mode = DMA_CIRCULAR;                         /* 使用循环模式 */
    hdma_dcmi.Init.Priority = DMA_PRIORITY_HIGH;                /* 高优先级 */
    hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;              /* 使能FIFO */
    hdma_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL; /* 使用1/2的FIFO */
    hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;                /* 存储器突发传输 */
    hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;             /* 外设突发单次传输 */
    HAL_DMA_DeInit(&hdma_dcmi);                                 /* 先清除以前的设置 */
    HAL_DMA_Init(&hdma_dcmi);                                   /* 初始化DMA */
    
    /* 在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
     * 这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
     * 锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
     * 程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOCK()先解锁一次DMA。
     */
    __HAL_UNLOCK(&hdma_dcmi);
    
    if (mem1addr == 0)                                                                                    /* 开启DMA，不使用双缓冲 */
    {
        HAL_DMA_Start(&hdma_dcmi, (uint32_t)&DCMI->DR, mem0addr, memsize);
    }
    else                                                                                                  /* 使用双缓冲 */
    {
        HAL_DMAEx_MultiBufferStart(&hdma_dcmi, (uint32_t)&DCMI->DR, mem0addr, mem1addr, memsize); /* 开启双缓冲 */
        __HAL_DMA_ENABLE_IT(&hdma_dcmi, DMA_IT_TC);                                               /* 开启传输完成中断 */
        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);                                            /* DMA中断优先级 */
        HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    }
	
}


/**
 * @brief       DCMI,启动传输
 * @param       无
 * @retval      无
 *针对 LTDC 驱动的 LCD 屏幕
 *(无直接写 GRAM 操作，帧缓冲区直接映射到 0XC0000000)
 */
void dcmi_start(void)
{
    __HAL_DMA_ENABLE(&hdma_dcmi);    /* 使能DMA传输 */
    DCMI->CR |= DCMI_CR_CAPTURE;     /* 启动DCMI捕获（开始接收摄像头数据） */
}

/**
 * @brief       DCMI停止图像采集传输
 * @param       无
 * @retval      无
 */
void dcmi_stop(void)
{
    DCMI->CR &= ~DCMI_CR_CAPTURE;                /* 停止DCMI捕获 */
    while (DCMI->CR & DCMI_CR_CAPTURE);         /* 等待捕获停止 */
    __HAL_DMA_DISABLE(&hdma_dcmi);              /* 关闭DMA传输 */
}

///**
// * @brief       DMA1_Stream1中断服务函数（双缓冲模式用）
// * @note        单缓冲模式无需此函数；双缓冲时切换缓冲区并刷新LTDC
// * @param       无
// * @retval      无
// */
//void DMA1_Stream1_IRQHandler(void)
//{
//    if (__HAL_DMA_GET_FLAG(&hdma_dcmi, DMA_FLAG_TCIF1_5) != RESET)
//    {
//        __HAL_DMA_CLEAR_FLAG(&hdma_dcmi, DMA_FLAG_TCIF1_5);  /* 清除传输完成标志 */
//        
//        /* [可选]双缓冲模式下：切换LTDC帧缓冲区、刷新显示等操作 */
//        // 示例：LCD_Fill(0,0,lcdltdc.width-1,lcdltdc.height-1,0); // 测试用清屏
//    }
//    HAL_DMA_IRQHandler(&hdma_dcmi); /* 调用HAL库DMA中断处理函数 */
//}

/**
 * @brief       DCMI初始化（基础配置，需结合摄像头初始化）
 * @param       无
 * @retval      无
 */
void DCMI_Init(void)
{
//    hdcmi.Instance = DCMI;
//    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;     /* 硬件同步(VSYNC/HSYNC) */
//    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;   /* PCLK上升沿有效 */
//    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;        /* VSYNC低电平有效（匹配摄像头） */
//    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;        /* HSYNC低电平有效（匹配摄像头） */
//    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;         /* 捕获所有帧 */
//    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;  /* 8位数据模式（匹配OV5640等摄像头） */
//    
//    HAL_DCMI_Init(&hdcmi);  /* 初始化DCMI（底层GPIO在HAL_DCMI_MspInit中配置） */
    __HAL_DCMI_ENABLE(&hdcmi); /* 使能DCMI外设 */
}

