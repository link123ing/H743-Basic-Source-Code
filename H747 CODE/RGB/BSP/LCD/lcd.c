/**
 ****************************************************************************************************
 * @file        lcd.c
 * @version     V1.0
 * @brief       LCD��ʾӦ�ú��� ����
 ****************************************************************************************************
 * @attention   Waiken-Smart ������Զ
 *
 * ʵ��ƽ̨:    STM32H747XIH6Сϵͳ��
 *
 ****************************************************************************************************
 */
 
#include "stdlib.h"
#include "lcd.h"
#include "lcdfont.h"
#include <stdio.h>
#include "bsp_delay.h"
#include "lcd_ltdc.h"


/* LCD�Ļ�����ɫ�ͱ���ɫ */
uint32_t g_point_color = RED;            /* ������ɫ */
uint32_t g_back_color  = 0XFFFFFFFF;     /* ����ɫ */

/* ����LCD��Ҫ���� */
_lcd_dev lcddev;


/**
 * @brief       LCD��ʱ����,�����ڲ�����mdk -O1ʱ���Ż�ʱ��Ҫ���õĵط�
 * @param       i:��ʱ����ֵ
 * @retval      ��
 */
void lcd_opt_delay(uint32_t i)
{
    while (i--);           /* ʹ��AC6ʱ��ѭ�����ܱ��Ż�,��ʹ��while(1) __asm volatile(""); */
}

/**
 * @brief       ׼��дGRAM
 * @param       ��
 * @retval      ��
 */
void lcd_write_ram_prepare(void)
{
}

/**
 * @brief       ��ɫת��
 * @note        ��RGB565��ɫת����RGB888��ɫ
 * @param       rgb565 : RGB565��ɫ
 * @retval      RGB888��ɫֵ
 */
uint32_t lcd_rgb565torgb888(uint16_t rgb565)
{
    uint16_t r, g, b;
    uint32_t rgb888;
  
    r = (rgb565 & 0XF800) >> 8;
    g = (rgb565 & 0X07E0) >> 3;
    b = (rgb565 & 0X001F) << 3;
  
    rgb888 = (r << 16) | (g << 8) | b;
  
    return rgb888;
}

/**
 * @brief       ��ȡĳ�������ɫֵ
 * @param       x,y:����
 * @retval      �˵����ɫ
 */
uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    if (x >= lcddev.width || y >= lcddev.height)return 0;   /* �����˷�Χ,ֱ�ӷ��� */

    return ltdc_read_point(x, y);
}

/**
 * @brief       LCD������ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_on(void)
{
    if (lcdltdc.pwidth != 0)
    {
        ltdc_switch(1);         /* ����LTDC */
    }
}

/**
 * @brief       LCD�ر���ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_off(void)
{
    if (lcdltdc.pwidth != 0)
    {
        ltdc_switch(0);         /* �ر�LTDC */
    }
}

/**
 * @brief       ���ù��λ��(��RGB����Ч)
 * @param       x,y: ����
 * @retval      ��
 */
void lcd_set_cursor(uint16_t x, uint16_t y)
{
}

/**
 * @brief       ����LCD���Զ�ɨ�跽��(��RGB����Ч)
 * @note
 *              ע��:�����������ܻ��ܵ��˺������õ�Ӱ��,
 *              ����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
 *
 * @param       dir:0~7,����8������(���嶨���lcd.h)
 * @retval      ��
 */
void lcd_scan_dir(uint8_t dir)
{
}

/**
 * @brief       ����
 * @param       x,y: ����
 * @param       color: �����ɫ
 * @retval      ��
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{    
    if (lcdltdc.pwidth != 0)     /* �����RGB�� */
    {
        ltdc_draw_point(x, y, color);
    }
}

/**
 * @brief       ����LCD��ʾ����
 * @param       dir:0,����; 1,����
 * @retval      ��
 */
void lcd_display_dir(uint8_t dir)
{
    lcddev.dir = dir;            /* ����/���� */
    
    if (lcdltdc.pwidth != 0)     /* �����RGB�� */
    {
        ltdc_display_dir(dir);
        lcddev.width = lcdltdc.width;
        lcddev.height = lcdltdc.height;
        return;
    }
}

/**
 * @brief       ���ô���(��RGB����Ч),���Զ����û������굽�������Ͻ�(sx,sy).
 * @param       sx,sy:������ʼ����(���Ͻ�)
 * @param       width,height:���ڿ�Ⱥ͸߶�,�������0!!
 * @note        �����С:width*height.
 *
 * @retval      ��
 */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    if (lcdltdc.pwidth != 0)     /* �����RGB�� */
    {
        return;                  /* RGB����֧�ָú��� */
    }
}

/**
 * @brief       ��ʼ��LCD
 * @note        �ó�ʼ���������Գ�ʼ�������ͺŵ�LCD(RGB��)
 *
 * @param       ��
 * @retval      ��
 */
void lcd_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    LCD_PWREN_GPIO_CLK_ENABLE();        /* LCD_PWREN����ʱ��ʹ�� */

    gpio_init_struct.Pin = LCD_PWREN_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;               /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                       /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;        /* ���� */
    HAL_GPIO_Init(LCD_PWREN_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��LCD_PWREN���� */
   
    LCD_PWREN(1);                       /* ����LCD_5V */
    delay_ms(100);                      /* �ȴ���ѹ�ȶ� */
  
    lcddev.id = ltdc_panelid_read();    /* ����Ƿ���RGB������ */

#if RGB_80_8001280         
    lcddev.id = 0X8081;
#endif
   
    if (lcddev.id != 0)                 /* ID����,˵����RGB������ */
    {
        ltdc_init();                    /* ��ʼ��LTDC */      
    }

    /* �ر�ע��, �����main�����������δ���1��ʼ��, ��Ῠ����printf
     * ����(������f_putc����), ����, �����ʼ������1, �������ε�����
     * ���� printf ��� !!!!!!!
     */
    printf("LCD ID:%x\r\n", lcddev.id); /* ��ӡLCD ID */

    if (lcdltdc.pwidth != 0)            /* �����RGB�� */
    {
        lcd_display_dir(1);             /* Ĭ��Ϊ�M�� */
    }
    
    LCD_BL(1);                          /* �������� */
    lcd_clear(WHITE);                   /* ���� */
}

/**
 * @brief       ��������
 * @param       color: Ҫ��������ɫ
 * @retval      ��
 */
void lcd_clear(uint32_t color)
{
    if (lcdltdc.pwidth != 0)            /* �����RGB�� */
    {
        ltdc_clear(color);
    }
}

/**
 * @brief       ��ָ����������䵥����ɫ
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color: Ҫ������ɫ
 * @retval      ��
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    if (lcdltdc.pwidth != 0)            /* �����RGB�� */
    {
        ltdc_fill(sx, sy, ex, ey, color);
    }
}

/**
 * @brief       ��ָ�����������ָ����ɫ��
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color: Ҫ������ɫ�����׵�ַ
 * @retval      ��
 */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    if (lcdltdc.pwidth != 0)            /* �����RGB�� */
    {
        ltdc_color_fill(sx, sy, ex, ey, color);
    }
}

/**
 * @brief       ����
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       color: �ߵ���ɫ
 * @retval      ��
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;                 /* ������������ */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0) incx = 1;         /* ���õ������� */
    else if (delta_x == 0) incx = 0;   /* ��ֱ�� */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0) incy = 1;
    else if (delta_y == 0) incy = 0;   /* ˮƽ�� */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y) distance = delta_x; /* ѡȡ�������������� */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )        /* ������� */
    {
        lcd_draw_point(row, col, color);        /* ���� */
        xerr += delta_x ;
        yerr += delta_y ;

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
 * @brief       ��ˮƽ��
 * @param       x,y  : �������
 * @param       len  : �߳���
 * @param       color: ���ε���ɫ
 * @retval      ��
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{
    if ((len == 0) || (x > lcddev.width) || (y > lcddev.height))return;

    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       ������
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       color: ���ε���ɫ
 * @retval      ��
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief       ��Բ
 * @param       x0,y0: Բ��������
 * @param       r    : �뾶
 * @param       color: Բ����ɫ
 * @retval      ��
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint32_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* �ж��¸���λ�õı�־ */

    while (a <= b)
    {
        lcd_draw_point(x0 + a, y0 - b, color);  /* 5 */
        lcd_draw_point(x0 + b, y0 - a, color);  /* 0 */
        lcd_draw_point(x0 + b, y0 + a, color);  /* 4 */
        lcd_draw_point(x0 + a, y0 + b, color);  /* 6 */
        lcd_draw_point(x0 - a, y0 + b, color);  /* 1 */
        lcd_draw_point(x0 - b, y0 + a, color);
        lcd_draw_point(x0 - a, y0 - b, color);  /* 2 */
        lcd_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* ʹ��Bresenham�㷨��Բ */
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
 * @brief       ���ʵ��Բ
 * @param       x,y  : Բ��������
 * @param       r    : �뾶
 * @param       color: Բ����ɫ
 * @retval      ��
 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint32_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * @brief       ��ָ��λ����ʾһ���ַ�
 * @param       x,y   : ����
 * @param       chr   : Ҫ��ʾ���ַ�:' '--->'~'
 * @param       size  : �����С 12/16/24/32
 * @param       mode  : ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * @param       color : �ַ�����ɫ;
 * @retval      ��
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint32_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';    /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* ����1206���� */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* ����1608���� */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];  /* ����2412���� */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];  /* ����3216���� */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];                            /* ��ȡ�ַ��ĵ������� */

        for (t1 = 0; t1 < 8; t1++)                  /* һ���ֽ�8���� */
        {
            if (temp & 0x80)                        /* ��Ч��,��Ҫ��ʾ */
            {
                lcd_draw_point(x, y, color);        /* �������,Ҫ��ʾ����� */
            }
            else if (mode == 0)                     /* ��Ч�㲢��ѡ��ǵ��ӷ�ʽ */
            {
                lcd_draw_point(x, y, g_back_color); /* ������ɫ,�൱������㲻��ʾ(ע�ⱳ��ɫ��ȫ�ֱ�������) */
            }

            temp <<= 1;                             /* ��λ, �Ա��ȡ��һ��λ��״̬ */
            y++;

            if (y >= lcddev.height)return;          /* �������� */

            if ((y - y0) == size)                   /* ��ʾ��һ����? */
            {
                y = y0;                             /* y���긴λ */
                x++;                                /* x������� */

                if (x >= lcddev.width)return;       /* x���곬������ */

                break;
            }
        }
    }
}

/**
 * @brief       ƽ������, m^n
 * @param       m: ����
 * @param       n: ָ��
 * @retval      m��n�η�
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       ��ʾlen������(��λΪ0����ʾ)
 * @param       x,y   : ��ʼ����
 * @param       num   : ��ֵ(0 ~ 2^32)
 * @param       len   : ��ʾ���ֵ�λ��
 * @param       size  : ѡ������ 12/16/24/32
 * @param       color : ���ֵ���ɫ;
 * @retval      ��
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint32_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                              /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                      /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))                                  /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2) * t, y, ' ', size, 0, color); /* ��ʾ�ո�,ռλ */
                continue;                                                  /* �����¸�һλ */
            }
            else
            {
                enshow = 1;                                                /* ʹ����ʾ */
            }
        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);  /* ��ʾ�ַ� */
    }
}

/**
 * @brief       ��չ��ʾlen������(��λ��0Ҳ��ʾ)
 * @param       x,y   : ��ʼ����
 * @param       num   : ��ֵ(0 ~ 2^32)
 * @param       len   : ��ʾ���ֵ�λ��
 * @param       size  : ѡ������ 12/16/24/32
 * @param       mode  : ��ʾģʽ
 *              [7]:0,�����;1,���0.
 *              [6:1]:����
 *              [0]:0,�ǵ�����ʾ;1,������ʾ.
 * @param       color : ���ֵ���ɫ;
 * @retval      ��
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint32_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                                            /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                                    /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))                                                /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                if (mode & 0X80)                                                         /* ��λ��Ҫ���0 */
                {
                    lcd_show_char(x + (size / 2) * t, y, '0', size, mode & 0X01, color); /* ��0ռλ */
                }
                else
                {
                    lcd_show_char(x + (size / 2) * t, y, ' ', size, mode & 0X01, color); /* �ÿո�ռλ */
                }

                continue;
            }
            else
            {
                enshow = 1;                                                              /* ʹ����ʾ */
            }
        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0X01, color);
    }
}

/**
 * @brief       ��ʾ�ַ���
 * @param       x,y         : ��ʼ����
 * @param       width,height: �����С
 * @param       size        : ѡ������ 12/16/24/32
 * @param       p           : �ַ����׵�ַ
 * @param       color       : �ַ�������ɫ
 * @retval      ��
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint32_t color)
{
    uint8_t x0 = x;
  
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
        {
            break;                       /* �˳� */
        }

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}








