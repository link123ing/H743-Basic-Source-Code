#ifndef __QSPI_H
#define __QSPI_H

#include "main.h"


uint8_t qspi_wait_flag(uint32_t flag, uint8_t sta, uint32_t wtime);                   /* QSPI等待某个状态 */
uint8_t qspi_init(void);                                                              /* 初始化QSPI */
void qspi_send_cmd(uint8_t cmd, uint32_t addr, uint8_t mode, uint8_t dmcycle);        /* QSPI发送命令 */
uint8_t qspi_receive(uint8_t *buf, uint32_t datalen);                                 /* QSPI接收数据 */
uint8_t qspi_transmit(uint8_t *buf, uint32_t datalen);                                /* QSPI发送数据 */

#endif
