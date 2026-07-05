#ifndef __BSP_CTIIC_H
#define __BSP_CTIIC_H

#include "bsp_system.h"


#define CT_SDA_Pin GPIO_PIN_7
#define CT_SDA_GPIO_Port GPIOG
#define CT_SCL_Pin GPIO_PIN_6
#define CT_SCL_GPIO_Port GPIOH



/* IO操作 */
#define CT_IIC_SCL(x)        do{ x ? \
                              HAL_GPIO_WritePin(CT_SCL_GPIO_Port, CT_SCL_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(CT_SCL_GPIO_Port, CT_SCL_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define CT_IIC_SDA(x)        do{ x ? \
                              HAL_GPIO_WritePin(CT_SDA_GPIO_Port, CT_SDA_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(CT_SDA_GPIO_Port, CT_SDA_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define CT_IIC_READ_SDA     HAL_GPIO_ReadPin(CT_SDA_GPIO_Port, CT_SDA_Pin) /* 读取SDA */


/* IIC所有操作函数 */
void ct_iic_init(void);            /* 初始化IIC的IO口 */
void ct_iic_start(void);           /* 发送IIC开始信号 */
void ct_iic_stop(void);            /* 发送IIC停止信号 */
void ct_iic_ack(void);             /* IIC发送ACK信号 */
void ct_iic_nack(void);            /* IIC不发送ACK信号 */
uint8_t ct_iic_wait_ack(void);     /* IIC等待ACK信号 */
void ct_iic_send_byte(uint8_t txd);/* IIC发送一个字节 */
uint8_t ct_iic_read_byte(unsigned char ack);/* IIC读取一个字节 */




#endif

