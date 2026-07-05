#ifndef __BSP_GTXXX_H
#define __BSP_GTXXX_H

#include "bsp_system.h"
#include "bsp_ctiic.h"

#define CT_INT_Pin GPIO_PIN_7
#define CT_INT_GPIO_Port GPIOH
#define CT_RST_Pin GPIO_PIN_8
#define CT_RST_GPIO_Port GPIOI

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

extern _m_tp_dev tp_dev;        /* 触屏控制器在touch.c里面定义 */
  
/* 与电容触摸屏连接的芯片引脚(未包含IIC引脚) 
 * IO操作函数 
 */
#define GT9XXX_RST(x)     do{ x ? \
                              HAL_GPIO_WritePin(CT_RST_GPIO_Port, CT_RST_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(CT_RST_GPIO_Port, CT_RST_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* 复位引脚 */

#define GT9XXX_INT        HAL_GPIO_ReadPin(CT_INT_GPIO_Port, CT_INT_Pin)     /* 读取引脚电平 */

/* IIC读写命令 */
#define GT9XXX_CMD_WR       0XBA//0X28        /* 写命令 */
#define GT9XXX_CMD_RD       0XBB//0X29        /* 读命令 */

/* GT9XXX 部分寄存器定义  */
#define GT9XXX_CTRL_REG     0X8040      /* GT9XXX控制寄存器 */
#define GT9XXX_CFGS_REG     0X8047      /* GT9XXX配置起始地址寄存器 */
#define GT9XXX_CHECK_REG    0X80FF      /* GT9XXX校验和寄存器 */
#define GT9XXX_PID_REG      0X8140      /* GT9XXX产品ID寄存器 */

#define GT9XXX_GSTID_REG    0X814E      /* GT9XXX当前检测到的触摸情况 */
#define GT9XXX_TP1_REG      0X8150      /* 第一个触摸点数据地址 */
#define GT9XXX_TP2_REG      0X8158      /* 第二个触摸点数据地址 */
#define GT9XXX_TP3_REG      0X8160      /* 第三个触摸点数据地址 */
#define GT9XXX_TP4_REG      0X8168      /* 第四个触摸点数据地址 */
#define GT9XXX_TP5_REG      0X8170      /* 第五个触摸点数据地址 */
#define GT9XXX_TP6_REG      0X8178      /* 第六个触摸点数据地址 */
#define GT9XXX_TP7_REG      0X8180      /* 第七个触摸点数据地址 */
#define GT9XXX_TP8_REG      0X8188      /* 第八个触摸点数据地址 */
#define GT9XXX_TP9_REG      0X8190      /* 第九个触摸点数据地址 */
#define GT9XXX_TP10_REG     0X8198      /* 第十个触摸点数据地址 */
 

uint8_t gt9xxx_wr_reg(uint16_t reg,uint8_t *buf,uint8_t len);
void gt9xxx_rd_reg(uint16_t reg,uint8_t *buf,uint8_t len); 
uint8_t GT9XX_Init(void);
uint8_t GT9XX_Scan(void); 






#endif

