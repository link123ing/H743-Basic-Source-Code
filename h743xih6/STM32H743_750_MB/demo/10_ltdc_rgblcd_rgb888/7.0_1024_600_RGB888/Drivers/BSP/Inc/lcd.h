#ifndef __LCD_H
#define __LCD_H

#include "main.h"

extern LTDC_HandleTypeDef hltdc;

/* LCD LTDC重要参数集 */
typedef struct  
{
    uint32_t pwidth;      /* LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入 */
    uint32_t pheight;     /* LCD面板的高度,固定参数,不随显示方向改变 */
    uint8_t activelayer;  /* 当前层编号:0/1 */
    uint8_t dir;          /* 0,竖屏;1,横屏; */
    uint16_t width;       /* LCD宽度 */
    uint16_t height;      /* LCD高度 */
    uint32_t pixsize;     /* 每个像素所占字节数 */
}_ltdc_dev; 


/* 定义默认背景层颜色 */
#define LTDC_BACKLAYERCOLOR           0X00000000
/* LCD帧缓冲区首地址,这里定义在SDRAM里面. */
#define LCD_FRAME_BUF_ADDR            0XC0000000  

#define LCD_PIXEL_FORMAT_ARGB8888       0X00
#define LCD_PIXEL_FORMAT_RGB888         0X01
#define LCD_PIXEL_FORMAT_RGB565         0X02
#define LCD_PIXEL_FORMAT_ARGB1555       0X03
#define LCD_PIXEL_FORMAT_ARGB4444       0X04
#define LCD_PIXEL_FORMAT_L8             0X05
#define LCD_PIXEL_FORMAT_AL44           0X06
#define LCD_PIXEL_FORMAT_AL88           0X07

/* 定义颜色像素格式 */
#define LCD_PIXFORMAT       LCD_PIXEL_FORMAT_RGB888

/* 画笔颜色 */
/* R G B */
#define WHITE     0xFFFFFF
#define BLACK     0x000000  
#define BLUE      0x0000FF
#define BRED      0XFF8080
#define GRED      0X8B0000
#define GBLUE     0X66CDAA
#define RED       0xFF0000
#define MAGENTA   0x000080
#define GREEN     0x00FF00
#define CYAN      0x00FFFF
#define YELLOW    0xFFFF00
#define BROWN     0X804040 
#define BRRED     0X800000 
#define GRAY      0X808080 
#define LGRAY     0XC0C0C0                     
  

/* LCD背光控制 */
#define LCD_BL(x)   do{ x ? \
                      HAL_GPIO_WritePin(GPIOB, LCD_BL_Pin, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(GPIOB, LCD_BL_Pin, GPIO_PIN_RESET); \
                     }while(0)

void LCD_Init(void);     /* LCD初始化 */
void LCD_Display_Dir(uint8_t dir);         /* 显示方向控制 */
void LCD_Draw_Point(uint16_t x, uint16_t y, uint32_t color);/* 画点函数 */
uint32_t LCD_Read_Point(uint16_t x, uint16_t y);            /* 读点函数 */
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);           /* 矩形单色填充函数 */
void LCD_Fill_Color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t *color);
void LCD_Clear(uint32_t color);            /* 清屏函数 */
void LCD_Layer_Window_Config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);/* LTDC层窗口设置 */
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void LCD_Draw_Vline(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
void LCD_Draw_Hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
void LCD_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint32_t color);
void LCD_Fill_Circle(uint16_t x, uint16_t y, uint16_t r, uint32_t color);
void LCD_Show_Num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint32_t color);
void LCD_Show_Xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint32_t color);
void LCD_Show_String(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint32_t color);


#endif