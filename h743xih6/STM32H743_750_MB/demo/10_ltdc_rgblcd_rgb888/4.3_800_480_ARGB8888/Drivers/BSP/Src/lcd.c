#include "lcd.h"
#include "lcdfont.h"
#include "main.h"

/* LCD的画笔颜色和背景色 */
uint32_t g_point_color = 0xFF000000;    /* 画笔颜色 */
uint32_t g_back_color  = 0xFFFFFFFF;    /* 背景色 */


/* 根据不同的颜色格式,定义帧缓存数组 */

uint32_t ltdc_lcd_framebuf[800][480] __attribute__((section(".bss.ARM.__at_0XC0000000")));  /* 定义最大屏分辨率时,LCD所需的帧缓存数组大小 */

_ltdc_dev lcdltdc;                         /* 管理LCD LTDC的重要参数 */


/**
 * @brief       LCD初始化函数
 * @param       无
 * @retval      无
 */
void LCD_Init(void)
{
    lcdltdc.pwidth = 800;           /* 面板宽度,单位:像素 */
    lcdltdc.pheight = 480;          /* 面板高度,单位:像素 */
    lcdltdc.activelayer=0;          /* 使用第一层 */
    lcdltdc.pixsize = 4;            /* ARGB8888每个像素占4个字节 */
    
    lcdltdc.width = lcdltdc.pwidth;
    lcdltdc.height = lcdltdc.pheight;
    
    LCD_Display_Dir(1);             /* 默认横屏 */
    
    LCD_BL(1);                      /* 点亮背光 */
    LCD_Clear(LTDC_BACKLAYERCOLOR); /* 清屏 */
}

/**
 * @brief       画点函数
 * @param       x,y:写入坐标
 * @param       color:颜色值
 * @retval      无
 */
void LCD_Draw_Point(uint16_t x, uint16_t y, uint32_t color)
{ 
    if (lcdltdc.dir)   /* 横屏 */
    {
        *(uint32_t*)((uint32_t)ltdc_lcd_framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
    else             /* 竖屏 */
    {
        *(uint32_t*)((uint32_t)ltdc_lcd_framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y)) = color; 
    }

}

/**
 * @brief       读点函数
 * @param       x,y:读取点的坐标
 * @param       返回值:颜色值
 * @retval      无
 */
uint32_t LCD_Read_Point(uint16_t x, uint16_t y)
{ 
    if (lcdltdc.dir)   /* 横屏 */
    {
        return *(uint32_t*)((uint32_t)ltdc_lcd_framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
    else             /* 竖屏 */
    {
        return *(uint32_t*)((uint32_t)ltdc_lcd_framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y)); 
    }
}

/**
 * @brief       设置LCD显示方向
 * @param       dir:0,竖屏；1,横屏
 * @retval      无
 */
void LCD_Display_Dir(uint8_t dir)
{
    lcdltdc.dir = dir;    /* 显示方向 */
    if (dir == 0)         /* 竖屏 */
    {
        lcdltdc.width = lcdltdc.pheight;
        lcdltdc.height = lcdltdc.pwidth;
    }
    else if (dir == 1)    /* 横屏 */
    {
        lcdltdc.width = lcdltdc.pwidth;
        lcdltdc.height = lcdltdc.pheight;
    }
}

/**
 * @brief       LCD清屏
 * @param       color:颜色值
 * @retval      无
 */
void LCD_Clear(uint32_t color)
{
    LCD_Fill(0, 0, lcdltdc.width - 1, lcdltdc.height - 1, color);
}

/**
 * @brief       LTDC填充矩形,DMA2D填充
 * @note        有时候需要频繁的调用填充函数，所以为了速度，填充函数采用寄存器版本，
 * @param       (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
 * @param       color:要填充的颜色
 * @retval      无
 */
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{ 
    uint32_t addr;
    uint32_t offline = 0;

    uint32_t psx, psy, pex, pey;   /* 以LCD面板为基准的坐标系,不随横竖屏变化而变化 */

    /* 坐标系转换 */
    if (lcdltdc.dir)               /* 横屏 */
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else                          /* 竖屏 */
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }
    
    /* 横屏 */
    addr = (uint32_t)ltdc_lcd_framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx);
    offline = lcdltdc.pwidth - (pex - psx + 1);
    
    /* 停止DMA2D */
    DMA2D->CR &= ~DMA2D_CR_START;
    
    /* 设置DMA2D工作模式 */
    DMA2D->CR = DMA2D_R2M;  /* 寄存器到存储器 */
    
    /* 设置DMA2D的相关参数 */
    /* 颜色格式设置(OPFCCR) */
    DMA2D->OPFCCR = LCD_PIXFORMAT;
    
    /* 输出存储器地址(OMAR) */
    DMA2D->OMAR = addr;
    
    /* 输出窗口(OOR NLR) */
    DMA2D->OOR = offline;
    DMA2D->NLR = ((pex - psx + 1) << 16) | (pey - psy + 1); /* 像素数 */
    
    /* 颜色寄存器(仅R2M时设置) OCOLR */
    DMA2D->OCOLR = color;
    
    /* 启动DMA2D传输 */
    DMA2D->CR |= DMA2D_CR_START;    /* 启动DMA2D */

    /* 等待DMA2D传输完成，清除相关标志 */
    while((DMA2D->ISR & DMA2D_FLAG_TC) == 0);
    DMA2D->IFCR |= DMA2D_FLAG_TC;
    while( LTDC->CDSR != 0X00000001);       /* 等待一帧刷完再刷下一帧 */
    
}

/**
 * @brief       在指定区域内填充指定颜色块(图片),DMA2D填充
 * @param       (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
 * @param       注意:sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
 * @param       color:要填充的颜色数组首地址
 * @retval      无
 */
void LCD_Fill_Color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t *color)
{
    uint32_t addr;
    uint32_t offline = 0;

    uint32_t psx, psy, pex, pey;   /* 以LCD面板为基准的坐标系,不随横竖屏变化而变化 */

    /* 坐标系转换 */
    if (lcdltdc.dir)               /* 横屏 */
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else                          /* 竖屏 */
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }
    
    /* 横屏 */
    addr = (uint32_t)ltdc_lcd_framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx);
    offline = lcdltdc.pwidth - (pex - psx + 1);
    
    /* 停止DMA2D */
    DMA2D->CR &= ~DMA2D_CR_START;
    
    /* 设置DMA2D工作模式 */
    DMA2D->CR = DMA2D_M2M;  /* 存储器到存储器 */
    
    /* 设置DMA2D的相关参数 */
    /* 颜色格式设置(FGPFCCR) */
    DMA2D->FGPFCCR = LCD_PIXFORMAT;
    
    /* 输入 & 输出存储器地址(FGMAR & OMAR) */
    DMA2D->FGMAR = (uint32_t)color;
    DMA2D->OMAR = addr;
    
    /* 输出窗口(FGOR OOR NLR) */
    DMA2D->FGOR = 0;
    DMA2D->OOR = offline;
    DMA2D->NLR = ((pex - psx + 1) << 16) | (pey - psy + 1); /* 像素数 */
    
    /* 启动DMA2D传输 */
    DMA2D->CR |= DMA2D_CR_START;    /* 启动DMA2D */

    /* 等待DMA2D传输完成，清除相关标志 */
    while((DMA2D->ISR & DMA2D_FLAG_TC) == 0);
    DMA2D->IFCR |= DMA2D_FLAG_TC;    
} 



/**
 * @brief       画水平线
 * @param       x,y   : 起点坐标
 * @param       len   : 线长度
 * @param       color : 颜色
 * @retval      无
 */
void LCD_Draw_Hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{
    if ((len == 0) || (x > lcdltdc.width) || (y > lcdltdc.height))
    {
        return;
    }

    LCD_Fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       画竖直线
 * @param       x,y   : 起点坐标
 * @param       len   : 线长度
 * @param       color : 颜色
 * @retval      无
 */
void LCD_Draw_Vline(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{
    if ((len == 0) || (x > lcdltdc.width) || (y > lcdltdc.height))
    {
        return;
    }

    LCD_Fill(x, y, x , y + len - 1, color);
}

/**
 * @brief       画线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       color: 线的颜色
 * @retval      无
 */
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;      /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1;       /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0;       /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;       /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)
    {
        distance = delta_x;  /* 选取基本增量坐标轴 */
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++ )    /* 画线输出 */
    {
        LCD_Draw_Point(row, col, color);    /* 画点 */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}


/**
 * @brief       画矩形
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       color: 矩形的颜色
 * @retval      无
 */
void LCD_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    LCD_Draw_Line(x1, y1, x2, y1, color);
    LCD_Draw_Line(x1, y1, x1, y2, color);
    LCD_Draw_Line(x1, y2, x2, y2, color);
    LCD_Draw_Line(x2, y1, x2, y2, color);
}

/**
 * @brief       画圆
 * @param       x0,y0 : 圆中心坐标
 * @param       r     : 半径
 * @param       color : 圆的颜色
 * @retval      无
 */
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint32_t color)
{
    int a, b;
    int di;

    a = 0;
    b = r;
    di = 3 - (r << 1);       /* 判断下个点位置的标志 */

    while (a <= b)
    {
        LCD_Draw_Point(x0 + a, y0 - b, color);  /* 5 */
        LCD_Draw_Point(x0 + b, y0 - a, color);  /* 0 */
        LCD_Draw_Point(x0 + b, y0 + a, color);  /* 4 */
        LCD_Draw_Point(x0 + a, y0 + b, color);  /* 6 */
        LCD_Draw_Point(x0 - a, y0 + b, color);  /* 1 */
        LCD_Draw_Point(x0 - b, y0 + a, color);
        LCD_Draw_Point(x0 - a, y0 - b, color);  /* 2 */
        LCD_Draw_Point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* 使用Bresenham算法画圆 */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief       填充实心圆
 * @param       x,y  : 圆中心坐标
 * @param       r    : 半径
 * @param       color: 圆的颜色
 * @retval      无
 */
void LCD_Fill_Circle(uint16_t x, uint16_t y, uint16_t r, uint32_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    LCD_Draw_Hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                LCD_Draw_Hline (x - i + 1, y + xr, 2 * (i - 1), color);
                LCD_Draw_Hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        LCD_Draw_Hline(x - xr, y + i, 2 * xr, color);
        LCD_Draw_Hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * @brief       在指定位置显示一个字符
 * @param       x,y  : 坐标
 * @param       chr  : 要显示的字符:" "--->"~"
 * @param       size : 字体大小 12/16/24/32
 * @param       mode : 叠加方式(1); 非叠加方式(0);
 * @retval      无
 */
void LCD_Show_Char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint32_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';    /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* 调用1206字体 */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* 调用1608字体 */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];  /* 调用2412字体 */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];  /* 调用3216字体 */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];                            /* 获取字符的点阵数据 */

        for (t1 = 0; t1 < 8; t1++)                  /* 一个字节8个点 */
        {
            if (temp & 0x80)                        /* 有效点,需要显示 */
            {
                LCD_Draw_Point(x, y, color);        /* 画点出来,要显示这个点 */
            }
            else if (mode == 0)                     /* 无效点,不显示 */
            {
                LCD_Draw_Point(x, y, g_back_color); /* 画背景色,相当于这个点不显示(注意背景色由全局变量控制) */
            }

            temp <<= 1;                             /* 移位, 以便获取下一个位的状态 */
            y++;

            if (y >= lcdltdc.height)return;          /* 超区域了 */

            if ((y - y0) == size)                   /* 显示完一列了? */
            {
                y = y0; /* y坐标复位 */
                x++;    /* x坐标递增 */

                if (x >= lcdltdc.width)
                {
                    return;       /* x坐标超区域了 */
                }

                break;
            }
        }
    }
}

/**
 * @brief       平方函数, m^n
 * @param       m: 底数
 * @param       n: 指数
 * @retval      m的n次方
 */
static uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       显示len个数字
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
 * @retval      无
 */
void LCD_Show_Num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint32_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;   /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))               /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                LCD_Show_Char(x + (size / 2)*t, y, ' ', size, 0, color);    /* 显示空格,占位 */
                continue;       /* 继续下个一位 */
            }
            else
            {
                enshow = 1;     /* 使能显示 */
            }
        }

        LCD_Show_Char(x + (size / 2)*t, y, temp + '0', size, 0, color);     /* 显示字符 */
    }
}

/**
 * @brief       扩展显示len个数字(高位是0也显示)
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
 * @param       mode: 显示模式
 *              [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 *
 * @retval      无
 */
void LCD_Show_Xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint32_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)       /* 按总显示位数循环 */
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;    /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                if (mode & 0x80)    /* 高位需要填充0 */
                {
                    LCD_Show_Char(x + (size / 2)*t, y, '0', size, mode & 0x01, color);  /* 用0占位 */
                }
                else
                {
                    LCD_Show_Char(x + (size / 2)*t, y, ' ', size, mode & 0x01, color);  /* 用空格占位 */
                }

                continue;
            }
            else
            {
                enshow = 1;     /* 使能显示 */
            }

        }

        LCD_Show_Char(x + (size / 2)*t, y, temp + '0', size, mode & 0x01, color);
    }
}

/**
 * @brief       显示字符串
 * @param       x,y         : 起始坐标
 * @param       width,height: 区域大小
 * @param       size        : 选择字体 12/16/24/32
 * @param       p           : 字符串首地址
 * @retval      无
 */
void LCD_Show_String(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint32_t color)
{
    uint8_t x0 = x;
    
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* 判断是不是非法字符! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
        {
            break;      /* 退出 */
        }

        LCD_Show_Char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}


