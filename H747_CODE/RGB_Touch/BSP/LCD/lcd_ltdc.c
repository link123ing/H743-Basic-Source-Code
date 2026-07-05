
/**
 ****************************************************************************************************
 * @file        ltdc.c
 * @version     V1.0
 * @brief       LTDC ��������
 ****************************************************************************************************
 * @attention   Waiken-Smart ������Զ
 *
 * ʵ��ƽ̨:    STM32H747XIH6Сϵͳ��
 *
 ****************************************************************************************************
 */

#include "lcd_ltdc.h"
#include "lcd.h"
#include "bsp_delay.h"

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
static void ltdc_fb_cache_sync(void)
{
    if (lcdltdc.pwidth != 0U)
    {
        uint32_t fb_bytes = lcdltdc.pwidth * lcdltdc.pheight * lcdltdc.pixsize;
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)LTDC_FRAME_BUF_ADDR, (int32_t)fb_bytes);
    }
}
#else
static void ltdc_fb_cache_sync(void)
{
}
#endif

void ltdc_framebuffer_sync(void)
{
    ltdc_fb_cache_sync();
}


uint32_t *g_ltdc_framebuf[2];              /* LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ����� */
_ltdc_dev lcdltdc;                         /* ����LCD LTDC����Ҫ���� */


/**
 * @brief       LTDC����
 * @param       sw   : 1,��; 0,�ر�;
 * @retval      ��
 */
void ltdc_switch(uint8_t sw)
{
    if (sw)
    {
        __HAL_LTDC_ENABLE(&hltdc);   /* ��LTDC */
    }
    else
    {
        __HAL_LTDC_DISABLE(&hltdc);  /* �ر�LTDC */
    }
}

/**
 * @brief       LTDC����ָ����
 * @param       layerx      : 0,��һ��; 1,�ڶ���;
 * @param       sw          : 1,��;   0,�ر�;
 * @retval      ��
 */
void ltdc_layer_switch(uint8_t layerx, uint8_t sw)
{
    if (sw) 
    {
        __HAL_LTDC_LAYER_ENABLE(&hltdc, layerx);   /* ����layerx */
    }
    else
    {
        __HAL_LTDC_LAYER_DISABLE(&hltdc, layerx);  /* �ر�layerx */
    }

    __HAL_LTDC_RELOAD_CONFIG(&hltdc);              /* �������¼������� */
}

/**
 * @brief       LTDCѡ���
 * @param       layerx   : ���:0,��һ��; 1,�ڶ���;
 * @retval      ��
 */
void ltdc_select_layer(uint8_t layerx)
{
    lcdltdc.activelayer = layerx;
}

/**
 * @brief       LTDC��ʾ��������
 * @param       dir         : 0,����; 1,����;
 * @retval      ��
 */
void ltdc_display_dir(uint8_t dir)
{
    lcdltdc.dir = dir;     /* ��ʾ���� */

    if (dir == 0)          /* ���� */
    {
        lcdltdc.width = lcdltdc.pheight;
        lcdltdc.height = lcdltdc.pwidth;
    }
    else if (dir == 1)     /* ���� */
    {
        lcdltdc.width = lcdltdc.pwidth;
        lcdltdc.height = lcdltdc.pheight;
    }
}

/**
 * @brief       LTDC���㺯��
 * @param       x,y         : д������
 * @param       color       : ��ɫֵ
 * @retval      ��
 */
void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color)
{ 
    g_ltdc_framebuf[0] = (uint32_t *)LTDC_FRAME_BUF_ADDR;
    lcdltdc.pixsize = 2;
#if LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888
    if (lcdltdc.dir)
    {
        *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
    else
    {
        *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = color;
    }
#elif LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888
    if (lcdltdc.dir)
    {
        *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
        *(uint8_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x) + 2) = color >> 16;
    }
    else
    {
        *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = color;
        *(uint8_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y) + 2) = color >> 16;
    }
#else
    if (lcdltdc.dir)
    {
        *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = (uint16_t)color;
    }
    else
    {
        *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = (uint16_t)color;
    }
#endif
}

/**
 * @brief       LTDC���㺯��
 * @param       x,y       : ��ȡ�������
 * @retval      ��ɫֵ
 */
uint32_t ltdc_read_point(uint16_t x, uint16_t y)
{ 
    g_ltdc_framebuf[0] = (uint32_t *)LTDC_FRAME_BUF_ADDR;
    lcdltdc.pixsize = 2;
#if LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888
    if (lcdltdc.dir)
    {
        return *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
    else
    {
        return *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y));
    }
#elif LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888
    if (lcdltdc.dir)
    {
        return *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) & 0xFFFFFF;
    }
    else
    {
        return *(uint32_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) & 0xFFFFFF;
    }
#else
    if (lcdltdc.dir)
    {
        return *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
    else
    {
        return *(uint16_t *)((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y));
    }
#endif
}

/**
 * @brief       LTDC������, DMA2D���
 * @note       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 *              ע��:sx,ex,���ܴ���lcddev.width - 1; sy,ey,���ܴ���lcddev.height - 1
 * @param       sx,sy       : ��ʼ����
 * @param       ex,ey       : ��������
 * @param       color       : ������ɫ
 * @retval      ��
 */
void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{ 
    uint32_t psx, psy, pex, pey;        /* ��LCD���Ϊ��׼������ϵ,����������仯���仯 */
    uint32_t timeout = 0; 
    uint16_t offline;
    uint32_t addr; 

    /* ����ϵת�� */
    if (lcdltdc.dir)                    /* ���� */
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else                                /* ���� */
    {
        if (ex >= lcdltdc.pheight)
        {
            ex = lcdltdc.pheight - 1;   /* ���Ʒ�Χ */
        }

        if (sx >= lcdltdc.pheight)
        {
            sx = lcdltdc.pheight - 1;   /* ���Ʒ�Χ */
        }
        
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }

    offline = lcdltdc.pwidth - (pex - psx + 1);   /* ��ƫ��:��ǰ�����һ�����غ���һ�е�һ������֮���������Ŀ */
    addr = ((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));

    __HAL_RCC_DMA2D_CLK_ENABLE();                             /* ʹ��DMA2Dʱ�� */

    DMA2D->CR &= ~(DMA2D_CR_START);                           /* ��ֹͣDMA2D */
    DMA2D->CR = DMA2D_R2M;                                    /* �Ĵ������洢��ģʽ */
    DMA2D->OPFCCR = LTDC_PIXFORMAT;                           /* ������ɫ��ʽ */
    DMA2D->OOR = offline;                                     /* ������ƫ��  */

    DMA2D->OMAR = addr;                                       /* ����洢����ַ */
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);   /* �趨�����Ĵ��� */
    DMA2D->OCOLR = color;                                     /* �趨�����ɫ�Ĵ��� */
    DMA2D->CR |= DMA2D_CR_START;                              /* ���DMA2D */

    while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)               /* �ȴ�������� */
    {
        timeout++;
      
        if (timeout > 0X1FFFFF)break;                         /* ��ʱ�˳� */
    } 
    
    DMA2D->IFCR |= DMA2D_FLAG_TC;                             /* ���������ɱ�־ */
}

///* ʹ��DMA2D��ص�HAL�⺯��ʹ��DMA2D����(���Ƽ�) */
///**
// * @brief       ��ָ����������䵥����ɫ
// * @param       (sx,sy),(ex,ey)  : �����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
// * @param       color            : Ҫ������ɫ
// * @retval      ��
// */
//void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
//{
//    uint32_t psx, psy, pex, pey;   /* ��LCD���Ϊ��׼������ϵ,����������仯���仯 */
//    uint32_t timeout = 0; 
//    uint16_t offline;
//    uint32_t addr;  
//
//    if (ex >= lcdltdc.width)
//    {
//        ex = lcdltdc.width - 1;
//    }
//
//    if (ey >= lcdltdc.height)
//    {
//        ey = lcdltdc.height - 1;
//    }
//
//    /* ����ϵת�� */
//    if (lcdltdc.dir)   /* ���� */
//    {
//        psx = sx; 
//        psy = sy;
//        pex = ex;
//        pey = ey;
//    }
//    else               /* ���� */
//    {
//        psx = sy;
//        psy = lcdltdc.pheight - ex - 1;
//        pex = ey;
//        pey = lcdltdc.pheight - sx - 1;
//    }

//    offline = lcdltdc.pwidth - (pex - psx + 1);
//    addr =((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
//
//    if (LTDC_PIXFORMAT == LTDC_PIXEL_FORMAT_RGB565)  /* �����RGB565��ʽ�Ļ���Ҫ������ɫת������16bitת��Ϊ32bit�� */
//    {
//        color = ((color & 0X0000F800) << 8) | ((color & 0X000007E0) << 5 ) | ((color & 0X0000001F) << 3);
//    }
//
//    /* ����DMA2D��ģʽ */
//    g_dma2d_handle.Instance = DMA2D;
//    g_dma2d_handle.Init.Mode = DMA2D_R2M;                                                    /* �Ĵ������洢��ģʽ */
//    g_dma2d_handle.Init.ColorMode = LTDC_PIXFORMAT;                                          /* ������ɫ��ʽ */
//    g_dma2d_handle.Init.OutputOffset = offline;                                              /* ���ƫ��  */
//    HAL_DMA2D_Init(&g_dma2d_handle);                                                         /* ��ʼ��DMA2D */
//
//    HAL_DMA2D_ConfigLayer(&g_dma2d_handle, lcdltdc.activelayer);                             /* ������ */
//    HAL_DMA2D_Start(&g_dma2d_handle, color, (uint32_t)addr, pex - psx + 1, pey - psy + 1);   /* ������� */
//    HAL_DMA2D_PollForTransfer(&g_dma2d_handle, 1000);                                        /* �������� */
//
//    while((__HAL_DMA2D_GET_FLAG( &g_dma2d_handle,DMA2D_FLAG_TC) == 0) && (timeout < 0X5000)) /* �ȴ�DMA2D������� */
//    {
//        timeout++;
//    }
//    __HAL_DMA2D_CLEAR_FLAG(&g_dma2d_handle,DMA2D_FLAG_TC);                                   /* ���������ɱ�־ */
//}

/**
 * @brief       ��ָ�����������ָ����ɫ��, DMA2D���
 * @note        �˺�����֧��uint16_t,RGB565��ʽ����ɫ�������.
 *              (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 *              ע��:sx,ex,���ܴ���lcddev.width - 1; sy,ey,���ܴ���lcddev.height - 1
 * @param       sx,sy       : ��ʼ����
 * @param       ex,ey       : ��������
 * @param       color       : ������ɫ�����׵�ַ
 * @retval      ��
 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint32_t psx, psy, pex, pey;   /* ��LCD���Ϊ��׼������ϵ,����������仯���仯 */
    uint32_t timeout = 0; 
    uint16_t offline;
    uint32_t addr;
  
    /* ����ϵת�� */
    if (lcdltdc.dir)               /* ���� */
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else                           /* ���� */
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }

    offline = lcdltdc.pwidth - (pex - psx + 1);   /* ��ƫ��:��ǰ�����һ�����غ���һ�е�һ������֮���������Ŀ */
    addr = ((uint32_t)g_ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));

    __HAL_RCC_DMA2D_CLK_ENABLE();                             /* ʹ��DMA2Dʱ�� */

    DMA2D->CR &= ~(DMA2D_CR_START);                           /* ��ֹͣDMA2D */
    DMA2D->CR = DMA2D_M2M;                                    /* �洢�����洢��ģʽ */
    DMA2D->FGPFCCR = LTDC_PIXFORMAT;                          /* ����ǰ������ɫ��ʽ */
    DMA2D->FGOR = 0;                                          /* ǰ������ƫ��Ϊ0 */
    DMA2D->OOR = offline;                                     /* ������ƫ�� */

    DMA2D->FGMAR = (uint32_t)color;                           /* Դ��ַ */
    DMA2D->OMAR = addr;                                       /* ����洢����ַ */
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);   /* �趨�����Ĵ��� */
    DMA2D->CR |= DMA2D_CR_START;                              /* ���DMA2D */

    while((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)                /* �ȴ�������� */
    {
        timeout++;
      
        if (timeout > 0X1FFFFF)break;                         /* ��ʱ�˳� */
    } 
    
    DMA2D->IFCR |= DMA2D_FLAG_TC;                             /* ���������ɱ�־ */
}  

/**
 * @brief       LTDC����
 * @param       color       : ��ɫֵ
 * @retval      ��
 */
void ltdc_clear(uint32_t color)
{
    ltdc_fill(0, 0, lcdltdc.width - 1, lcdltdc.height - 1, color);
}

/**
 * @brief       LTDCʱ��(Fdclk)���ú���
 * @param       pll3n     : PLL3 VCO��Ƶϵ��(PLL��Ƶ),        ȡֵ��Χ:4~512.
 * @param       pll3m     : PLL3Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),  ȡֵ��Χ:1~63.
 * @param       pll3r     : PLL3��r��Ƶϵ��(PLL֮��ķ�Ƶ),   ȡֵ��Χ:1~128.
 *
 * @note        Fvco = Fs * (pll3n / pll3m);
 *              Fr = Fvco / pll3r = Fs * (pll3n / (pll3m * pll3r));
 *              Fdclk = Fr;
 *              ����:
 *              Fvco: VCOƵ��
 *              Fr: PLL3��r��Ƶ���ʱ��Ƶ��
 *              Fs: PLL3����ʱ��Ƶ��,������HSI,CSI,HSE��(ϵͳʱ�ӳ�ʼ��ʱѡ��HSE��ΪPLL������ʱ��Դ).
 *
 *              ����:�ⲿ����Ϊ25M, pllm = 25 ��ʱ��, Fs = 25Mhz�� pllm��Ƶ��Ƶ�� Ϊ1Mhz.
 *              ����: Ҫ�õ�33M��LTDCʱ��, ���������: pll3n = 300, pllm = 25, pll3r = 9
 *              Fdclk= ((25 / 25) * 300) / 9 = 33 Mhz
 * @retval      0, �ɹ�;
 *              1, ʧ��;
 */
uint8_t ltdc_clk_set(uint32_t pll3n, uint32_t pll3m, uint32_t pll3r)
{
    RCC_PeriphCLKInitTypeDef periphclk_initure;

    /* LTDC�������ʱ�ӣ���Ҫ�����Լ���ʹ�õ�LCD�����ֲ������ã� */
    periphclk_initure.PeriphClockSelection = RCC_PERIPHCLK_LTDC;     /* LTDCʱ�� */
    periphclk_initure.PLL3.PLL3M = pll3m;
    periphclk_initure.PLL3.PLL3N = pll3n;
    periphclk_initure.PLL3.PLL3P = 2;
    periphclk_initure.PLL3.PLL3Q = 2;
    periphclk_initure.PLL3.PLL3R = pll3r;

    if (HAL_RCCEx_PeriphCLKConfig(&periphclk_initure) == HAL_OK)     /* ��������ʱ�� */
    {
        return 0;                                                    /* �ɹ� */
    }
    else 
    {
        return 1;                                                    /* ʧ�� */
    }
}

/**
 * @brief       LTDC�㴰������, ������LCD�������ϵΪ��׼
 * @note        �˺���������ltdc_layer_parameter_config֮��������.����,�����õĴ���ֵ���������ĳ�
 *              ��ʱ,GRAM�Ĳ���(��/д�㺯��),ҲҪ���ݴ��ڵĿ���������޸�,������ʾ������(�����̾�δ���޸�).
 * @param       layerx      : 0,��һ��; 1,�ڶ���;
 * @param       sx, sy      : ��ʼ����
 * @param       width,height: ��Ⱥ͸߶�
 * @retval      ��
 */
void ltdc_layer_window_config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    HAL_LTDC_SetWindowPosition(&hltdc, sx, sy, layerx);     /* ���ô��ڵ�λ�� */
    HAL_LTDC_SetWindowSize(&hltdc, width, height, layerx);  /* ���ô��ڴ�С */
  
    if (lcdltdc.pheight == 1280 && layerx == 0)
    {
        LTDC_Layer1->CFBLR = (width * 4 << 16) | (width * 4 + 7);   /* ֡�������г����м������(���ֽ�Ϊ��λ) */
    }  
}

/**
 * @brief       LTDC�������������
 * @note        �˺���,������ltdc_layer_window_config֮ǰ����.
 * @param       layerx      : 0,��һ��; 1,�ڶ���;
 * @param       bufaddr     : ����ɫ֡������ʼ��ַ
 * @param       pixformat   : �����ظ�ʽ. 0,ARGB8888; 1,RGB888; 2,RGB565; 3,ARGB1555; 4,ARGB4444; 5,L8; 6;AL44; 7;AL88
 * @param       alpha       : ��㶨Alphaֵ, 0,ȫ͸��;255,��͸��
 * @param       alpha0      : Ĭ����ɫAlphaֵ, 0,ȫ͸��;255,��͸��
 * @param       bfac1       : ���ϵ��1, 4(100),�㶨Alpha; 6(101),����Alpha*�㶨Alpha
 * @param       bfac2       : ���ϵ��2, 5(101),�㶨Alpha; 7(111),����Alpha*�㶨Alpha
 * @param       bkcolor     : ��Ĭ����ɫ,32λ,��24λ��Ч,RGB888��ʽ
 * @retval      ��
 */
void ltdc_layer_parameter_config(uint8_t layerx, uint32_t bufaddr, uint8_t pixformat, uint8_t alpha, uint8_t alpha0, uint8_t bfac1, uint8_t bfac2, uint32_t bkcolor)
{
    LTDC_LayerCfgTypeDef playercfg;

    playercfg.WindowX0 = 0;                                            /* ������ʼX���� */
    playercfg.WindowY0 = 0;                                            /* ������ʼY���� */
    playercfg.WindowX1 = lcdltdc.pwidth;                               /* ������ֹX���� */
    playercfg.WindowY1 = lcdltdc.pheight;                              /* ������ֹY���� */
    playercfg.PixelFormat = pixformat;                                 /* ���ò����ظ�ʽ */
    playercfg.Alpha = alpha;                                           /* ���ò�㶨Alphaֵ,0~255,Ӳ��255��Ƶ,255Ϊ��ȫ��͸�� */
    playercfg.Alpha0 = alpha0;                                         /* ����Ĭ����ɫAlphaֵ */
    playercfg.BlendingFactor1 = (uint32_t)bfac1 << 8;                  /* ���ò���ϵ��1 */
    playercfg.BlendingFactor2 = (uint32_t)bfac2;                       /* ���ò���ϵ��2 */
    playercfg.FBStartAdress = bufaddr;                                 /* ���ò���ɫ֡������ʼ��ַ */
    playercfg.ImageWidth = lcdltdc.pwidth;                             /* ������ɫ֡�������Ŀ�� */
    playercfg.ImageHeight = lcdltdc.pheight;                           /* ������ɫ֡�������ĸ߶� */
    playercfg.Backcolor.Red = (uint8_t)(bkcolor & 0X00FF0000) >> 16;   /* ������ɫ��ɫ���� */
    playercfg.Backcolor.Green = (uint8_t)(bkcolor & 0X0000FF00) >> 8;  /* ������ɫ��ɫ���� */
    playercfg.Backcolor.Blue = (uint8_t)bkcolor & 0X000000FF;          /* ������ɫ��ɫ���� */
    HAL_LTDC_ConfigLayer(&hltdc, &playercfg, layerx);          /* ������ѡ�еĲ� */
}  

/**
 * @brief       LTDC��ȡ���ID
 * @note        ����LCD RGB�ߵ����λ(R7,G7,B7)��ʶ�����ID
 *              PJ6 = R7(M0); PK2 = G7(M1); PK6 = B7(M2);
 *              M2:M1:M0
 *              0 :0 :0     4.3 ��480*272  RGB��,ID = 0X4342
 *              0 :0 :1     7   ��800*480  RGB��,ID = 0X7084
 *              0 :1 :0     7   ��1024*600 RGB��,ID = 0X7016
 *              0 :1 :1     5.5 ��720*1280 RGB��,ID = 0X5571
 *              1 :0 :0     4.3 ��800*480  RGB��,ID = 0X4384
 *              1 :0 :1     10.1��1280*800 RGB��,ID = 0X1018
 * @param       ��
 * @retval      0, �Ƿ�; 
 *              ����, LCD ID
 */
uint16_t ltdc_panelid_read(void)
{
    uint8_t idx = 0;

    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_GPIOJ_CLK_ENABLE();                              /* ʹ��GPIOJʱ�� */
    __HAL_RCC_GPIOK_CLK_ENABLE();                              /* ʹ��GPIOKʱ�� */
    
    gpio_init_struct.Pin = GPIO_PIN_6;                         /* R7����PJ6 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                   /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;        /* ���� */
    HAL_GPIO_Init(GPIOJ, &gpio_init_struct);                   /* ��ʼ��PJ6 */
    
    gpio_init_struct.Pin = GPIO_PIN_2 | GPIO_PIN_6;            /* G7,B7����PK2,6 */
    HAL_GPIO_Init(GPIOK, &gpio_init_struct);                   /* ��ʼ��PK2,6 */

    delay_us(10);
    idx  = (uint8_t)HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_6);       /* ��ȡM0 */
    idx |= (uint8_t)HAL_GPIO_ReadPin(GPIOK, GPIO_PIN_2) << 1;  /* ��ȡM1 */
    idx |= (uint8_t)HAL_GPIO_ReadPin(GPIOK, GPIO_PIN_6) << 2;  /* ��ȡM2 */

    switch (idx)
    {
        case 0 :
            return 0X4342;                    /* 4.3����,480*272�ֱ��� */
        
        case 1 :
            return 0X7084;                    /* 7  ����,800*480�ֱ��� */
        
        case 2 :
            return 0X7016;                    /* 7  ����,1024*600�ֱ��� */
        
        case 3 :
            return 0X5571;                    /* 5.5����,720*1280�ֱ��� */
        
        case 4 :
            return 0X4384;                    /* 4.3����,800*480�ֱ��� */
        
        case 5 :
            return 0X1018;                    /* 10.1����,1280*800�ֱ��� */
        
        default :
            return 0;
    }
}

/**
 * @brief       LTDC��ʼ������
 * @param       ��
 * @retval      ��
 */
void ltdc_init(void)
{
    uint16_t lcdid = lcddev.id;

#if RGB_80_8001280
    if (lcdid != 0U)
    {
        lcdid = 0X8081;
    }
#endif

    ltdc_gpio_init();    /* ��ȡLCD���ID */
 
    if (lcdid == 0X4342)
    {
        lcdltdc.pwidth = 480;       /* �����,��λ:���� */
        lcdltdc.pheight = 272;      /* ���߶�,��λ:���� */
        lcdltdc.hsw = 1;            /* ˮƽͬ����� */
        lcdltdc.hbp = 40;           /* ˮƽ���� */
        lcdltdc.hfp = 5;            /* ˮƽǰ�� */      
        lcdltdc.vsw = 1;            /* ��ֱͬ����� */
        lcdltdc.vbp = 8;            /* ��ֱ���� */      
        lcdltdc.vfp = 8;            /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 33);  /* ��������ʱ��  9Mhz */
    }
    else if (lcdid == 0X7084)
    {
        lcdltdc.pwidth = 800;       /* �����,��λ:���� */
        lcdltdc.pheight = 480;      /* ���߶�,��λ:���� */
        lcdltdc.hsw = 1;            /* ˮƽͬ����� */
        lcdltdc.hbp = 46;           /* ˮƽ���� */
        lcdltdc.hfp = 210;          /* ˮƽǰ�� */
        lcdltdc.vsw = 1;            /* ��ֱͬ����� */
        lcdltdc.vbp = 23;           /* ��ֱ���� */
        lcdltdc.vfp = 22;           /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 9);   /* ��������ʱ�� 33Mhz(�����˫��,��Ҫ����DCLK��18.75Mhz,�Ż�ȽϺ�) */
    }
    else if (lcdid == 0X7016)
    {
        lcdltdc.pwidth = 1024;      /* �����,��λ:���� */
        lcdltdc.pheight = 600;      /* ���߶�,��λ:���� */
        lcdltdc.hsw = 20;           /* ˮƽͬ����� */
        lcdltdc.hbp = 140;          /* ˮƽ���� */
        lcdltdc.hfp = 160;          /* ˮƽǰ�� */
        lcdltdc.vsw = 3;            /* ��ֱͬ����� */
        lcdltdc.vbp = 20;           /* ��ֱ���� */
        lcdltdc.vfp = 12;           /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 6);   /* ��������ʱ��  50Mhz */
    }
    else if (lcdid == 0X5571)
    {
        lcdltdc.pwidth = 720;       /* �����,��λ:���� */
        lcdltdc.pheight = 1280;     /* ���߶�,��λ:���� */
        lcdltdc.hsw = 10;           /* ˮƽͬ����� */
        lcdltdc.hbp = 36;           /* ˮƽ���� */
        lcdltdc.hfp = 46;           /* ˮƽǰ�� */
        lcdltdc.vsw = 5;            /* ��ֱͬ����� */
        lcdltdc.vbp = 5;            /* ��ֱ���� */
        lcdltdc.vfp = 16;           /* ��ֱǰ�� */
        ltdc_clk_set(330, 25, 6);   /* ��������ʱ��  55Mhz */
    }
    else if (lcdid == 0X4384)
    {
        lcdltdc.pwidth = 800;       /* �����,��λ:���� */
        lcdltdc.pheight = 480;      /* ���߶�,��λ:���� */
        lcdltdc.hsw = 48;           /* ˮƽͬ����� */      
        lcdltdc.hbp = 88;           /* ˮƽ���� */
        lcdltdc.hfp = 40;           /* ˮƽǰ�� */
        lcdltdc.vsw = 3;            /* ��ֱͬ����� */
        lcdltdc.vbp = 32;           /* ��ֱ���� */
        lcdltdc.vfp = 13;           /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 9);   /* ��������ʱ�� 33Mhz */ 
    }
    else if (lcdid == 0X8081)       /* 8��800*1280 RGB�� */
    {
        lcdltdc.pwidth = 800;       /* �����,��λ:���� */
        lcdltdc.pheight = 1280;     /* ���߶�,��λ:���� */
        lcdltdc.hsw = 5;            /* ˮƽͬ����� */
        lcdltdc.hbp = 20;           /* ˮƽ���� */
        lcdltdc.hfp = 40;           /* ˮƽǰ�� */
        lcdltdc.vsw = 3;            /* ��ֱͬ����� */
        lcdltdc.vbp = 20;           /* ��ֱ���� */
        lcdltdc.vfp = 30;           /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 5);   /* ��������ʱ��  60Mhz */
    }
    else if (lcdid == 0X1018)       /* 10.1��1280*800 RGB�� */
    {
        lcdltdc.pwidth = 1280;      /* �����,��λ:���� */
        lcdltdc.pheight = 800;      /* ���߶�,��λ:���� */
        lcdltdc.hsw = 10;           /* ˮƽͬ����� */
        lcdltdc.hbp = 140;          /* ˮƽ���� */
        lcdltdc.hfp = 10;           /* ˮƽǰ�� */
        lcdltdc.vsw = 3;            /* ��ֱͬ����� */
        lcdltdc.vbp = 10;           /* ��ֱ���� */
        lcdltdc.vfp = 10;           /* ��ֱǰ�� */
        ltdc_clk_set(300, 25, 5);   /* ��������ʱ��  60Mhz */
    } 

    lcddev.width = lcdltdc.pwidth;      /* ����lcddev�Ŀ�Ȳ��� */
    lcddev.height = lcdltdc.pheight;    /* ����lcddev�ĸ߶Ȳ��� */
    lcdltdc.pixformat = LTDC_PIXFORMAT; /* ��ɫ���ظ�ʽ */

    g_ltdc_framebuf[0] = (uint32_t *)LTDC_FRAME_BUF_ADDR;
    lcdltdc.pixsize = 2;

    
    /* LTDC���� */
#if !BSP_USE_CUBEMX_LTDC
    hltdc.Instance = LTDC;
    
    if (lcdid == 0X8081)
    {
        hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AH;     /* ˮƽͬ������:�ߵ�ƽ��Ч */
    }
    else
    {
        hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;     /* ˮƽͬ������:�͵�ƽ��Ч */
    }
    
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;         /* ��ֱͬ������:�͵�ƽ��Ч */
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;         /* ����ʹ�ܼ���:�͵�ƽ��Ч */
    hltdc.State = HAL_LTDC_STATE_RESET;
    
    if (lcdid == 0X1018 || lcdid == 0X8081)
    {
        hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IIPC;   /* ����ʱ�Ӽ���:�ߵ�ƽ��Ч */
    }
    else 
    {
        hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;    /* ����ʱ�Ӽ���:�͵�ƽ��Ч */
    }

    hltdc.Init.HorizontalSync = lcdltdc.hsw - 1;                                            /* ˮƽͬ����� */
    hltdc.Init.VerticalSync = lcdltdc.vsw - 1;                                              /* ��ֱͬ���߶� */
    hltdc.Init.AccumulatedHBP = lcdltdc.hsw + lcdltdc.hbp - 1;                              /* �ۼ�ˮƽ���ؿ�� */
    hltdc.Init.AccumulatedVBP = lcdltdc.vsw + lcdltdc.vbp - 1;                              /* �ۼӴ�ֱ���ظ߶� */
    hltdc.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth - 1;         /* �ۼ���Ч��� */
    hltdc.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight - 1;        /* �ۼ���Ч�߶� */
    hltdc.Init.TotalWidth = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth + lcdltdc.hfp - 1;   /* �ܿ�� */
    hltdc.Init.TotalHeigh = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp - 1;  /* �ܸ߶� */
    hltdc.Init.Backcolor.Red = 0;                                                           /* ����ɫ��ɫֵ */
    hltdc.Init.Backcolor.Green = 0;                                                         /* ����ɫ��ɫֵ */
    hltdc.Init.Backcolor.Blue = 0;                                                          /* ����ɫ��ɫֵ */
    HAL_LTDC_Init(&hltdc);
#endif

    /* ������ */
    ltdc_layer_parameter_config(0, (uint32_t)g_ltdc_framebuf[0], LTDC_PIXFORMAT, 255, 0, 6, 7, 0X000000);   /* ��������� */
    ltdc_layer_window_config(0, 0, 0, lcdltdc.pwidth, lcdltdc.pheight);                                     /* �㴰������,��LCD�������ϵΪ��׼,��Ҫ����޸�! */

    //ltdc_layer_parameter_config(1, (uint32_t)g_ltdc_framebuf[1], LTDC_PIXFORMAT, 127, 0, 6, 7, 0X000000); /* ��������� */
    //ltdc_layer_window_config(1, 0, 0, lcdltdc.pwidth, lcdltdc.pheight);                                   /* �㴰������,��LCD�������ϵΪ��׼,��Ҫ����޸�! */

    ltdc_display_dir(1);                   /* Ĭ�ϙM������lcd_init������������ */
    ltdc_select_layer(0);                  /* ѡ���1�� */
    LTDC_RST(1);
    delay_ms(10);
    LTDC_RST(0);
    delay_ms(50);
    LTDC_RST(1);
    delay_ms(200);

    
    LTDC_BL(1);                            /* �������� */
    ltdc_clear(WHITE);
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    ltdc_fb_cache_sync();
#endif                /* ���� */
}

/**
 * @brief       LTDC�ײ�IO��ʼ����ʱ��ʹ��
 * @note        �˺����ᱻHAL_LTDC_Init()����
 * @param       hltdc:LTDC���
 * @retval      ��
 */








void ltdc_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    LTDC_DE_GPIO_CLK_ENABLE();
    LTDC_VSYNC_GPIO_CLK_ENABLE();
    LTDC_HSYNC_GPIO_CLK_ENABLE();
    LTDC_CLK_GPIO_CLK_ENABLE();
    LTDC_BL_GPIO_CLK_ENABLE();
    LTDC_RST_GPIO_CLK_ENABLE();
    LCD_PWREN_GPIO_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    gpio_init_struct.Pin = LTDC_BL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LTDC_RST_GPIO_PIN;
    HAL_GPIO_Init(LTDC_RST_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_PWREN_GPIO_PIN;
    HAL_GPIO_Init(LCD_PWREN_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_struct.Alternate = GPIO_AF14_LTDC;

    gpio_init_struct.Pin = LTDC_DE_GPIO_PIN;
    HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &gpio_init_struct);
    gpio_init_struct.Pin = LTDC_VSYNC_GPIO_PIN;
    HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &gpio_init_struct);
    gpio_init_struct.Pin = LTDC_HSYNC_GPIO_PIN;
    HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &gpio_init_struct);
    gpio_init_struct.Pin = LTDC_CLK_GPIO_PIN;
    HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOI, &gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_All;
    HAL_GPIO_Init(GPIOJ, &gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOK, &gpio_init_struct);

    LCD_PWREN(1);
}
