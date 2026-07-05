#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include "bsp_system.h"

#define LCD_BL_Pin GPIO_PIN_5
#define LCD_BL_GPIO_Port GPIOB

extern LTDC_HandleTypeDef hltdc;

/* LCD LTDC重要参数集 */
typedef struct  
{
    uint32_t pwidth;      /* LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入 */
    uint32_t pheight;     /* LCD面板的高度,固定参数,不随显示方向改变 */
    uint8_t  activelayer; /* 当前层编号:0/1 */
    uint8_t  dir;         /* 0,竖屏;1,横屏; */
    uint16_t width;       /* LCD宽度 */
    uint16_t height;      /* LCD高度 */
    uint32_t pixsize;     /* 每个像素所占字节数 */
}_ltdc_dev; 

extern _ltdc_dev lcdltdc;


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
#define LCD_PIXFORMAT       LCD_PIXEL_FORMAT_RGB565
                      
/* 常用画笔颜色 */
#define WHITE           0xFFFF      /* 白色 */
#define BLACK           0x0000      /* 黑色 */
#define RED             0xF800      /* 红色 */
#define GREEN           0x07E0      /* 绿色 */
#define BLUE            0x001F      /* 蓝色 */ 
#define MAGENTA         0XF81F      /* 品红色/紫红色 = BLUE + RED */
#define YELLOW          0XFFE0      /* 黄色 = GREEN + RED */
#define CYAN            0X07FF      /* 青色 = GREEN + BLUE */  

/* 非常用颜色 */
#define BROWN           0XBC40      /* 棕色 */
#define BRRED           0XFC07      /* 棕红色 */
#define GRAY            0X8430      /* 灰色 */ 
#define DARKBLUE        0X01CF      /* 深蓝色 */
#define LIGHTBLUE       0X7D7C      /* 浅蓝色 */ 
#define GRAYBLUE        0X5458      /* 灰蓝色 */ 
#define LIGHTGREEN      0X841F      /* 浅绿色 */  
#define LGRAY           0XC618      /* 浅灰色(PANNEL),窗体背景色 */ 
#define LGRAYBLUE       0XA651      /* 浅灰蓝色(中间层颜色) */ 
#define LBBLUE          0X2B12      /* 浅棕蓝色(选择条目的反色) */ 

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
void LCD_Fill_Color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
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
