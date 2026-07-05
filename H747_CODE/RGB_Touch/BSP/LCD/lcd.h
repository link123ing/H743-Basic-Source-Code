/**
 ****************************************************************************************************
 * @file        lcd.h
 * @version     V1.0
 * @brief       LCD��ʾӦ�ú��� ����
 ****************************************************************************************************
 * @attention   Waiken-Smart ������Զ
 *
 * ʵ��ƽ̨:    STM32H747XIH6Сϵͳ��
 *
 ****************************************************************************************************
 */

#ifndef __LCD_H
#define __LCD_H

#include "stdlib.h"
#include "main.h"
#include "lcd_ltdc.h"


/******************************************************************************************/
/* LCD PWREN ���� ���� 
 * LCD_PWREN��������IO����LCD_5V�Ŀ���ر�, ����LCD_5V��Ҫ��LCD_PWREN�������1.
 */

#define LCD_PWREN_GPIO_PORT               GPIOI
#define LCD_PWREN_GPIO_PIN                GPIO_PIN_11
#define LCD_PWREN_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   /* PI��ʱ��ʹ�� */

/* LCD��Դ�������� */
#define LCD_PWREN(x)      do{ x ? \
                              HAL_GPIO_WritePin(LCD_PWREN_GPIO_PORT, LCD_PWREN_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(LCD_PWREN_GPIO_PORT, LCD_PWREN_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)

/******************************************************************************************/

/* LCD��Ҫ������ */
typedef struct
{
    uint16_t width;     /* LCD ��� */
    uint16_t height;    /* LCD �߶� */
    uint16_t id;        /* LCD ID */
    uint8_t dir;        /* ���������������ƣ�0��������1�������� */
} _lcd_dev;

/* LCD���� */
extern _lcd_dev lcddev; /* ����LCD��Ҫ���� */

/* LCD�Ļ�����ɫ�ͱ���ɫ */
extern uint32_t  g_point_color;     /* Ĭ�Ϻ�ɫ */
extern uint32_t  g_back_color;      /* ������ɫ.Ĭ��Ϊ��ɫ */

/* LCD������� */
#define LCD_BL(x)      do{ x ? \
                           HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_SET) : \
                           HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_RESET); \
                       }while(0)

/* LCD��λ���� */
#define LCD_RST(x)     do{ x ? \
                           HAL_GPIO_WritePin(LTDC_RST_GPIO_PORT, LTDC_RST_GPIO_PIN, GPIO_PIN_SET) : \
                           HAL_GPIO_WritePin(LTDC_RST_GPIO_PORT, LTDC_RST_GPIO_PIN, GPIO_PIN_RESET); \
                       }while(0)

/******************************************************************************************/
/* LCD��ɫ ���� */

#if LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB565

/* ���û�����ɫ */
#define WHITE           0xFFFF          /* ��ɫ */
#define BLACK           0x0000          /* ��ɫ */
#define RED             0xF800          /* ��ɫ */
#define GREEN           0x07E0          /* ��ɫ */
#define BLUE            0x001F          /* ��ɫ */ 
#define MAGENTA         0XF81F          /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0XFFE0          /* ��ɫ = GREEN + RED */
#define CYAN            0X07FF          /* ��ɫ = GREEN + BLUE */  

/* �ǳ�����ɫ */
#define BROWN           0XBC40          /* ��ɫ */
#define BRRED           0XFC07          /* �غ�ɫ */
#define GRAY            0X8430          /* ��ɫ */ 
#define DARKBLUE        0X01CF          /* ����ɫ */
#define LIGHTBLUE       0X7D7C          /* ǳ��ɫ */ 
#define GRAYBLUE        0X5458          /* ����ɫ */ 
#define LIGHTGREEN      0X841F          /* ǳ��ɫ */  
#define LGRAY           0XC618          /* ǳ��ɫ(PANNEL),���屳��ɫ */ 
#define LGRAYBLUE       0XA651          /* ǳ����ɫ(�м����ɫ) */ 
#define LBBLUE          0X2B12          /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */ 

#elif LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888

/* ���û�����ɫ */
#define WHITE           0xFFFFFF        /* ��ɫ */
#define BLACK           0x000000        /* ��ɫ */
#define RED             0xFF0000        /* ��ɫ */
#define GREEN           0x00FF00        /* ��ɫ */
#define BLUE            0x0000FF        /* ��ɫ */ 
#define MAGENTA         0XFF00FF        /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0XFFFF00        /* ��ɫ = GREEN + RED */
#define CYAN            0X00FFFF        /* ��ɫ = GREEN + BLUE */  

/* �ǳ�����ɫ */
#define BROWN           0xB88800        /* ��ɫ */
#define BRRED           0XF88038        /* �غ�ɫ */
#define GRAY            0X808480        /* ��ɫ */ 
#define DARKBLUE        0X003878        /* ����ɫ */
#define LIGHTBLUE       0X78ACE0        /* ǳ��ɫ */ 
#define GRAYBLUE        0X5088C0        /* ����ɫ */ 
#define LIGHTGREEN      0X8080F8        /* ǳ��ɫ */  
#define LGRAY           0XC0C0C0        /* ǳ��ɫ(PANNEL),���屳��ɫ */ 
#define LGRAYBLUE       0XA0C888        /* ǳ����ɫ(�м����ɫ) */ 
#define LBBLUE          0x286090        /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */

#elif LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888

#define WHITE           0xFFFFFFFF      /* ��ɫ */
#define BLACK           0xFF000000      /* ��ɫ */
#define RED             0xFFFF0000      /* ��ɫ */
#define GREEN           0xFF00FF00      /* ��ɫ */
#define BLUE            0xFF0000FF      /* ��ɫ */ 
#define MAGENTA         0XFFFF00FF      /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0XFFFFFF00      /* ��ɫ = GREEN + RED */
#define CYAN            0XFF00FFFF      /* ��ɫ = GREEN + BLUE */  

#define BROWN           0xFFB88800      /* ��ɫ */
#define BRRED           0XFFF88038      /* �غ�ɫ */
#define GRAY            0XFF808480      /* ��ɫ */ 
#define DARKBLUE        0XFF003878      /* ����ɫ */
#define LIGHTBLUE       0XFF78ACE0      /* ǳ��ɫ */ 
#define GRAYBLUE        0XFF5088C0      /* ����ɫ */ 
#define LIGHTGREEN      0XFF8080F8      /* ǳ��ɫ */  
#define LGRAY           0XFFC0C0C0      /* ǳ��ɫ(PANNEL),���屳��ɫ */ 
#define LGRAYBLUE       0XFFA0C888      /* ǳ����ɫ(�м����ɫ) */ 
#define LBBLUE          0xFF286090      /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */

#endif

/******************************************************************************************/
/* �������� */

void lcd_init(void);                        /* ��ʼ��LCD */ 
void lcd_display_on(void);                  /* ����ʾ */ 
void lcd_display_off(void);                 /* ����ʾ */
void lcd_scan_dir(uint8_t dir);             /* ������Ļɨ�跽�� */ 
void lcd_display_dir(uint8_t dir);          /* ������Ļ��ʾ���� */ 

void lcd_write_ram_prepare(void);                              /* ׼��дGRAM */ 
void lcd_set_cursor(uint16_t x, uint16_t y);                   /* ���ù�� */ 
uint32_t lcd_rgb565torgb888(uint16_t rgb565);                  /* ��RGB565ת��ΪRGB888 */
uint32_t lcd_read_point(uint16_t x, uint16_t y);               /* ���� */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);   /* ���� */

void lcd_clear(uint32_t color);                                                                /* LCD���� */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint32_t color);                      /* ���ʵ��Բ */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint32_t color);                     /* ��Բ */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color);                     /* ��ˮƽ�� */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);                /* ���ô��� */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);             /* ��ɫ������ */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);      /* ��ɫ������ */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);        /* ��ֱ�� */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);   /* ������ */

void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint32_t color);                       /* ��ʾһ���ַ� */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint32_t color);                     /* ��ʾ���� */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint32_t color);      /* ��չ��ʾ���� */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint32_t color);   /* ��ʾ�ַ��� */


#endif






