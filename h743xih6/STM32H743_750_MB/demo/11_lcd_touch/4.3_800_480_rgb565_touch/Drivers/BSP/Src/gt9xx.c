#include "string.h"
#include "lcd.h"
#include "ctiic.h"
#include "gt9xx.h"
#include "usart.h"

/* 注意: 除了GT9271支持10点触摸之外, 其他触摸芯片只支持 5点触摸 */
uint8_t g_gt_tnum = 5;      /* 默认支持的触摸屏点数(5点触摸) */

/* 10个触控点的颜色(电容触摸屏用) */
const uint16_t POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, YELLOW, MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY};

_m_tp_dev tp_dev =
{
    0,
    0,
    0,
};     /* 和触摸相关的结构体 */

/**
 * @brief       向gt9xxx写入一次数据
 * @param       reg : 起始寄存器地址
 * @param       buf : 数据缓缓存区
 * @param       len : 写数据长度
 * @retval      0, 成功; 1, 失败;
 */
uint8_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);    /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);         /* 发送高8位地址 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0XFF);       /* 发送低8位地址 */
    ct_iic_wait_ack();

    for (i = 0; i < len; i++)
    {
        ct_iic_send_byte(buf[i]);       /* 发数据 */
        ret = ct_iic_wait_ack();
        if (ret)break;
    }

    ct_iic_stop();  /* 产生一个停止条件 */
    return ret;
}

/**
 * @brief       从gt9xxx读出一次数据
 * @param       reg : 起始寄存器地址
 * @param       buf : 数据缓缓存区
 * @param       len : 读数据长度
 * @retval      无
 */
void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);    /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);         /* 发送高8位地址 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0XFF);       /* 发送低8位地址 */
    ct_iic_wait_ack();
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_RD);    /* 发送读命令 */
    ct_iic_wait_ack();

    for (i = 0; i < len; i++)
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);  /* 发数据 */
    }

    ct_iic_stop();  /* 产生一个停止条件 */
}

/**
 * @brief       初始化gt9xxx触摸屏
 * @param       无
 * @retval      0, 初始化成功; 1, 初始化失败;
 */
uint8_t GT9XX_Init(void)
{
    uint8_t temp[5];
    
    /* 引脚初始化在gpio.c中已经完成 */
    
    ct_iic_init();      /* 初始化电容屏的I2C总线 */
    
    /* 通信地址:0x28/0x29 */
    GT9XXX_RST(0);      /* 复位 */
    HAL_Delay(10);
    GT9XXX_RST(1);      /* 释放复位 */
    
    HAL_Delay(100);
    
    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4); /* 读取产品ID */
    
    temp[4] = 0;
    
//    printf("CTP ID:%s\r\n", temp);          /* 打印ID */
    
    if (strcmp((char *)temp, "9271") == 0)  /* ID==9271, 支持10点触摸 */
    {
         g_gt_tnum = 10;    /* 支持10点触摸屏 */
    }
    
    
    temp[0] = 0X02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);    /* 软复位GT9XXX */
    
    HAL_Delay(10);
    
    temp[0] = 0X00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);    /* 结束复位, 进入读坐标状态 */

    return 0;
}

/* GT9XXX 10个触摸点(最多) 对应的寄存器表 */
const uint16_t GT9XXX_TPX_TBL[10] =
{
    GT9XXX_TP1_REG, GT9XXX_TP2_REG, GT9XXX_TP3_REG, GT9XXX_TP4_REG, GT9XXX_TP5_REG,
    GT9XXX_TP6_REG, GT9XXX_TP7_REG, GT9XXX_TP8_REG, GT9XXX_TP9_REG, GT9XXX_TP10_REG,
};


/**
 * @brief       扫描触摸屏(采用查询方式)
 * @retval      当前触屏状态
 *   @arg       0, 触屏无触摸; 
 *   @arg       1, 触屏有触摸;
 */
uint8_t GT9XX_Scan(void)
{
    uint8_t  mode,i,res=0;
    uint8_t buf[4];
    uint16_t temp,tempsta;
    
    static uint8_t t = 0;   /* 控制查询间隔,从而降低CPU占用率 */
    t++; 
    
    if ((t % 10) == 0 || t < 10)    /* 空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率 */
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);          /* 读取触摸点的状态 */ 
        if ((mode & 0X80) && ((mode & 0XF) <= g_gt_tnum))   /* 有触摸并且触摸点数小于最大值 */
        {
            i = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1);         /* 清标志 */
        }
            
        if ((mode & 0XF) && ((mode & 0XF) <= g_gt_tnum))
        {
            temp = 0XFFFF << (mode & 0XF);  /* 将点的个数转换为1的位数,匹配tp_dev.sta定义 */
            tempsta = tp_dev.sta;           /* 保存当前的tp_dev.sta值 */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];  /* 保存触点0的数据,保存在最后一个上 */
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];

            for (i = 0; i < g_gt_tnum; i++)
            {
                if (tp_dev.sta & (1 << i))          /* 触摸有效? */
                {
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);   /* 读取XY坐标值 */

                    if (lcdltdc.dir)                /* 横屏 */
                    {
                        tp_dev.x[i] = (((uint16_t)buf[1] << 8) + buf[0]);
                        tp_dev.y[i] = (((uint16_t)buf[3] << 8) + buf[2]);
                    }
                    else
                    {
                        tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        tp_dev.x[i] = lcdltdc.width - (((uint16_t)buf[3] << 8) + buf[2]);
                    }
                    
                    /* 测试用 */
                    LCD_Fill_Circle(tp_dev.x[i],tp_dev.y[i],3,POINT_COLOR_TBL[i]);
                    printf("x[%d]:%d,y[%d]:%d\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);  
                    /* 实际使用中，这里可以放操作代码 */
                    
                }
            }
            res = 1;        /* 标志有触摸 */
            if (tp_dev.x[0] > lcdltdc.width || tp_dev.y[0] > lcdltdc.height)  /* 非法数据(坐标超出了) */
            {
                if ((mode & 0XF) > 1)   /* 有其他点有数据,则复第二个触点的数据到第一个触点. */
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;  /* 触发一次,则会最少连续监测10次,从而提高命中率 */
                }
                else        /* 非法数据,则忽略此次数据(还原原来的) */
                {
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                    mode = 0X80;
                    tp_dev.sta = tempsta;   /* 恢复tp_dev.sta */
                }
            }
            else 
            {
                t = 0;      /* 触发一次,则会最少连续监测10次,从而提高命中率 */
            }    
        }
        
    }
    if ((mode & 0X8F) == 0X80)  /* 无触摸点按下 */
    {
        if (tp_dev.sta & TP_PRES_DOWN)      /* 之前是被按下的 */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;    /* 标记按键松开 */
        }
        else    /* 之前就没有被按下 */
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE000;           /* 清除点有效标记 */
        }
    }

    if (t > 240)t = 10; /* 重新从10开始计数 */

    return res;
}



