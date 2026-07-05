#include "bsp_ctiic.h"

/**
 * @brief       IIC延时函数,用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static void ct_iic_delay(void)
{
      __IO  uint8_t i =50;   /* 防止被MDK优化 */
      while(i--) ; 
}

/**
 * @brief       初始化IIC
 * @param       无
 * @retval      无
 */
void ct_iic_init(void)
{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};

//    __HAL_RCC_GPIOH_CLK_ENABLE();
//    __HAL_RCC_GPIOI_CLK_ENABLE();
//    
//    /*Configure GPIO pin : PtPin */
//    GPIO_InitStruct.Pin = CT_SCL_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    HAL_GPIO_Init(CT_SCL_GPIO_Port, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = CT_SDA_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    HAL_GPIO_Init(CT_SDA_GPIO_Port, &GPIO_InitStruct);
    /* SDA引脚模式设置,开漏输出,上拉, 这样就不用再设置IO方向了, 开漏输出的时候(=1), 也可以读取外部信号的高低电平 */
    /* 初始化的过程已经在gpio中完成 */

    ct_iic_stop();     /* 停止总线上所有设备 */
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
void ct_iic_start(void)
{
    CT_IIC_SCL(1);
    CT_IIC_SDA(1);
    ct_iic_delay();
    CT_IIC_SDA(0);     /* START信号: 当SCL为高时, SDA从高变成低, 表示起始信号 */
    ct_iic_delay();
    CT_IIC_SCL(0);     /* 钳住I2C总线，准备发送或接收数据 */
    ct_iic_delay();
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void ct_iic_stop(void)
{
    CT_IIC_SDA(0);     /* STOP信号: 当SCL为高时, SDA从低变成高, 表示停止信号 */
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SDA(1);     /* 发送I2C总线结束信号 */
}

/**
 * @brief       等待应答信号到来
 * @param       无
 * @retval      1，接收应答失败
 *              0，接收应答成功
 */
uint8_t ct_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    CT_IIC_SDA(1);     /* 主机释放SDA线(此时外部器件可以拉低SDA线) */
    ct_iic_delay();
    CT_IIC_SCL(1);     /* SCL=1, 此时从机可以返回ACK */
    ct_iic_delay();

    while (CT_IIC_READ_SDA)    /* 等待应答 */
    {
        waittime++;

        if (waittime > 250)
        {
            ct_iic_stop();
            rack = 1;
            break;
        }
    }

    CT_IIC_SCL(0);     /* SCL=0, 结束ACK检查 */
    ct_iic_delay();
    return rack;
}

/**
 * @brief       产生ACK应答
 * @param       无
 * @retval      无
 */
void ct_iic_ack(void)
{
    CT_IIC_SDA(0);     /* SCL 0 -> 1 时 SDA = 0,表示应答 */
    ct_iic_delay();
    CT_IIC_SCL(1);     /* 产生一个时钟 */
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
    CT_IIC_SDA(1);     /* 主机释放SDA线 */
    ct_iic_delay();
}

/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
void ct_iic_nack(void)
{
    CT_IIC_SDA(1);     /* SCL 0 -> 1  时 SDA = 1,表示不应答 */
    ct_iic_delay();
    CT_IIC_SCL(1);     /* 产生一个时钟 */
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
}

/**
 * @brief       IIC发送一个字节
 * @param       data: 要发送的数据
 * @retval      无
 */
void ct_iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t = 0; t < 8; t++)
    {
        CT_IIC_SDA((data & 0x80) >> 7);    /* 高位先发送 */
        ct_iic_delay();                    /* 等数据稳定 */
        CT_IIC_SCL(1);
        ct_iic_delay();
        CT_IIC_SCL(0);
        data <<= 1;     /* 左移1位,用于下一次发送 */
    }
    CT_IIC_SDA(1);         /* 发送完成, 主机释放SDA线 */
}

/**
 * @brief       IIC读取一个字节
 * @param       ack:  ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t ct_iic_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* 接收1个字节数据 */
    {
        receive <<= 1;  /* 高位先输出,所以先收到的数据位要左移 */
        CT_IIC_SCL(1);
        ct_iic_delay();    /* 等数据稳定 */

        if (CT_IIC_READ_SDA)
        {
            receive++;
        }
        
        CT_IIC_SCL(0);
        ct_iic_delay();
    }

    if (!ack)
    {
        ct_iic_nack();     /* 发送nACK */
    }
    else
    {
        ct_iic_ack();      /* 发送ACK */
    }
    
    return receive;
}












