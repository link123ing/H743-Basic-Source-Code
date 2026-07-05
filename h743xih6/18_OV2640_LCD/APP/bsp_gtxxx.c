#include "bsp_gtxxx.h"

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
    uint8_t retry = 0;
    uint8_t success = 0;
    
    ct_iic_init();              /* 初始化I2C总线 */
    HAL_Delay(10);
    
    /* 尝试最多3次读取ID */
    for (retry = 0; retry < 3; retry++)
    {
        /* 硬件复位 */
        GT9XXX_RST(0);
        HAL_Delay(10);
        GT9XXX_RST(1);
        HAL_Delay(250);         /* 延长等待时间，确保芯片稳定 */
        
        /* 读取产品ID */
        gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);
        temp[4] = 0;
        
        printf("CTP ID attempt %d: %s\r\n", retry + 1, temp);
        
        /* 校验ID是否有效（常见ID: 911, 914, 915, 916, 917, 9271等） */
        if (strcmp((char *)temp, "9271") == 0 ||
            strcmp((char *)temp, "911")  == 0 ||
            strcmp((char *)temp, "914")  == 0 ||
            strcmp((char *)temp, "915")  == 0 ||
            strcmp((char *)temp, "916")  == 0 ||
            strcmp((char *)temp, "917")  == 0 ||
						strcmp((char *)temp, "9147")  == 0)	
        {
            success = 1;
            break;
        }
        
        /* 读取失败：额外延时，准备重试 */
        HAL_Delay(100);
    }
    
    if (!success)
    {
        printf("CTP init failed: cannot read valid ID\r\n");
        return 1;   /* 初始化失败 */
    }
    
    /* 根据ID设置最大触摸点数 */
    if (strcmp((char *)temp, "9271") == 0)
    {
        g_gt_tnum = 10;
    }
    else
    {
        g_gt_tnum = 5;
    }
    
    /* 软复位GT9xxx（配置生效）*/
    temp[0] = 0x02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);
    HAL_Delay(10);
    temp[0] = 0x00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);
    
    printf("CTP init success, ID: %s, max touch: %d\r\n", temp, g_gt_tnum);
    return 0;
}
/* GT9XXX 10个触摸点(最多) 对应的寄存器表 */
const uint16_t GT9XXX_TPX_TBL[10] =
{
    GT9XXX_TP1_REG, GT9XXX_TP2_REG, GT9XXX_TP3_REG, GT9XXX_TP4_REG, GT9XXX_TP5_REG,
    GT9XXX_TP6_REG, GT9XXX_TP7_REG, GT9XXX_TP8_REG, GT9XXX_TP9_REG, GT9XXX_TP10_REG,
};


/**
 * @brief       扫描触摸屏 (查询方式)
 * @retval      当前触屏状态
 *   @arg       0, 触屏无触摸; 
 *   @arg       1, 触屏有触摸;
 */
uint8_t GT9XX_Scan(void)
{
    uint8_t  mode, i, res = 0;
    uint8_t  buf[4];
    uint16_t tempsta;
    static uint8_t t = 0;           /* 控制查询间隔 */
    uint8_t  touch_num;
    uint8_t  valid_touch = 0;       /* 标记本次是否有合法触摸点 */
    uint8_t  need_clear_gstid = 0;  /* 标记是否需要清除GSTID寄存器 */

    t++;
    if ((t % 10) == 0 || t < 10)    /* 每10次调用只真正读取1次 */
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);
        touch_num = mode & 0x0F;

        /* 有触摸点且点数合理 */
        if ((mode & 0x80) && (touch_num <= g_gt_tnum) && (touch_num > 0))
        {
            tempsta = tp_dev.sta;
            tp_dev.sta = (~(0xFFFF << touch_num)) | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];

            uint16_t x_tmp[CT_MAX_TOUCH];
            uint16_t y_tmp[CT_MAX_TOUCH];
            uint8_t  valid_cnt = 0;

            /* 读取所有触摸点坐标并初步合法性检查 */
            for (i = 0; i < touch_num; i++)
            {
                gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);
                if (lcdltdc.dir)    /* 横屏 */
                {
                    x_tmp[i] = ((uint16_t)buf[1] << 8) + buf[0];
                    y_tmp[i] = ((uint16_t)buf[3] << 8) + buf[2];
                }
                else                /* 竖屏（根据实际坐标变换） */
                {
                    y_tmp[i] = ((uint16_t)buf[1] << 8) + buf[0];
                    x_tmp[i] = lcdltdc.width - (((uint16_t)buf[3] << 8) + buf[2]);
                }

                /* 坐标合法性：必须位于屏幕范围内（含边界，与原逻辑一致） */
                if (x_tmp[i] <= lcdltdc.width && y_tmp[i] <= lcdltdc.height)
                {
                    tp_dev.x[i] = x_tmp[i];
                    tp_dev.y[i] = y_tmp[i];
                    valid_cnt++;
                }
                else
                {
                    tp_dev.sta &= ~(1 << i);   /* 清除非法点的有效标志 */
                }
            }

            /* 如果没有合法触摸点，则恢复旧状态，本次无有效触摸 */
            if (valid_cnt == 0)
            {
                tp_dev.sta = tempsta;
                tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                /* 无合法点，但依然需要清除GSTID，否则芯片会卡死 */
                need_clear_gstid = 1;
                /* 不重置t，保持慢速查询，避免假触摸疯狂打印 */
                valid_touch = 0;
            }
            else
            {
                /* 有合法触摸点：重置t，便于连续检测 */
                t = 0;
                valid_touch = 1;

                /* 调试输出（正式使用时可以注释） */
                for (i = 0; i < touch_num; i++)
                {
                    if (tp_dev.sta & (1 << i))
                    {
                        LCD_Fill_Circle(tp_dev.x[i], tp_dev.y[i], 3, POINT_COLOR_TBL[i]);
                        printf("x[%d]:%d, y[%d]:%d\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);
                    }
                }
                res = 1;    /* 有有效触摸 */

                /* 必须清除GSTID（读完坐标后） */
                need_clear_gstid = 1;
            }
        }
        else    /* 无触摸（mode & 0x8F == 0x80）或点数异常 */
        {
            if ((mode & 0x8F) == 0x80)  /* 确无触摸 */
            {
                if (tp_dev.sta & TP_PRES_DOWN)
                {
                    tp_dev.sta &= ~TP_PRES_DOWN;   /* 清除按下标志 */
                }
                /* 清除所有点有效位，重置坐标 */
                tp_dev.sta &= 0xE000;
                tp_dev.x[0] = 0xFFFF;
                tp_dev.y[0] = 0xFFFF;
                for (i = 1; i < CT_MAX_TOUCH; i++)
                {
                    tp_dev.x[i] = 0;
                    tp_dev.y[i] = 0;
                }
            }
            /* 无论哪种无触摸情形，都清除GSTID，防止状态挂起 */
            need_clear_gstid = 1;
            valid_touch = 0;
        }

        /* 统一清除GSTID（如果前面标记了需要清除） */
        if (need_clear_gstid)
        {
            i = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1);
        }

        /* 如果没有合法触摸点，且之前没有清除GSTID的情况不会发生，但确保安全 */
        if (valid_touch == 0)
        {
            /* 保持t值不变，不做额外操作 */
        }
    }

    /* 限制t的范围，避免溢出 */
    if (t > 240) t = 10;

    return res;
}


