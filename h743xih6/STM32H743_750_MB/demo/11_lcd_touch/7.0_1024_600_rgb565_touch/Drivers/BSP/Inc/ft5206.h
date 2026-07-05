#ifndef __FT5206_H
#define __FT5206_H

#include "main.h"

#define TP_PRES_DOWN    0x8000  /* 触屏被按下 */
#define TP_CATH_PRES    0x4000  /* 有按键按下了 */

#define CT_MAX_TOUCH    5      /* 电容屏支持的点数,固定为5点 */

/* 触摸屏控制器 */
typedef struct
{

    uint16_t x[CT_MAX_TOUCH];   /* 当前坐标 */
    uint16_t y[CT_MAX_TOUCH];   /* 电容屏有最多10组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
                                 * x[9],y[9]存储第一次按下时的坐标.
                                 */

    uint16_t sta;               /* 笔的状态
                                 * b15:按下1/松开0;
                                 * b14:0,没有按键按下;1,有按键按下.
                                 * b13~b10:保留
                                 * b9~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
                                 */
} _m_tp_dev;

extern _m_tp_dev tp_dev;        

                                        
/* 与电容触摸屏连接的芯片引脚(未包含IIC引脚) 
 * IO操作函数 
 */
#define FT5206_RST(x)     do{ x ? \
                              HAL_GPIO_WritePin(CT_RST_GPIO_Port, CT_RST_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(CT_RST_GPIO_Port, CT_RST_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* 复位引脚 */

#define FT5206_INT        HAL_GPIO_ReadPin(CT_INT_GPIO_Port, CT_INT_Pin)     /* 读取引脚电平 */                                        

/* FT5206 IIC读写地址定义 */
#define FT5206_CMD_WR                   0x70    /* 写地址 */
#define FT5206_CMD_RD                   0x71    /* 读地址 */

/* FT5206部分寄存器地址定义 */
#define FT5206_DEVIDE_MODE              0x00    /* FT5206模式控制寄存器 */
#define FT5206_REG_NUM_FINGER           0x02    /* 触摸状态寄存器 */
#define FT5206_TP1_REG                  0x03    /* 第一个触摸点数据地址 */
#define FT5206_TP2_REG                  0x09    /* 第二个触摸点数据地址 */
#define FT5206_TP3_REG                  0x0F    /* 第三个触摸点数据地址 */
#define FT5206_TP4_REG                  0x15    /* 第四个触摸点数据地址 */
#define FT5206_TP5_REG                  0x1B    /* 第五个触摸点数据地址 */ 
#define FT5206_ID_G_LIB_VERSION         0xA1    /* 版本 */
#define FT5206_ID_G_MODE                0xA4    /* FT5206中断模式控制寄存器 */
#define FT5206_ID_G_THGROUP             0x80    /* 触摸有效值设置寄存器 */
#define FT5206_ID_G_PERIODACTIVE        0x88    /* 激活状态周期设置寄存器 */

/* 函数声明 */
uint8_t ft5206_init(void);          /* 初始化FT5206 */
uint8_t ft5206_scan(void);          /* 扫描FT5206读触摸屏 */

#endif
