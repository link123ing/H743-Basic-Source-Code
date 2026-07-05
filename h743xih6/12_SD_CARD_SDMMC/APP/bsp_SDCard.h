#ifndef __BSP_SDCARD_H
#define __BSP_SDCARD_H

#include "bsp_system.h"



void SD_State_Test(void);
void SD_Block_Erase(void);
void SD_Block_Write(void);
void SD_Block_Read(void);
void SD_Erase_All_Card(void);



#endif
