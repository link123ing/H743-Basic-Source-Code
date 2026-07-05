#ifndef __BSP_MY_DCMI_H
#define __BSP_MY_DCMI_H


#include "bsp_system.h"
extern DMA_HandleTypeDef hdma_dcmi;
extern DCMI_HandleTypeDef hdcmi;

void DCMI_DMA_Init(uint32_t mem0addr, uint32_t mem1addr, uint32_t memsize, uint32_t memblen, uint32_t meminc);
void dcmi_start(void);
void dcmi_stop(void);
void DCMI_Init(void);




#endif

