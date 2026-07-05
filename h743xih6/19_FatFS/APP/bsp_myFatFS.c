#include "bsp_myFatFS.h"

FATFS SDFatFS;

void FatFS_Init(void)
{
	FRESULT	res = f_mount(&SDFatFS, "SD:", 1);
	Uart_Printf(&huart1,"[SD] re-f_mount = %d\r\n", res);
	/*==========读写测试===============*/
//    FIL file;
//    UINT bw;
	//		res = f_open(&file, "0:/test.txt", FA_CREATE_ALWAYS | FA_WRITE);	//0:也可改为SD:
			//在开启 FF_STR_VOLUME_ID = 1下，会根据FF_VOLUME_STRS查表对应的下表匹配数字盘符，二者等价
//    if (res == FR_OK) {
//        f_write(&file, "Hello FatFs on FreeRTOS!\r\n", 27, &bw);
//        f_close(&file);
//        Uart_Printf(&huart1,"[SD] write %u bytes -> test.txt\r\n", bw);
//    } else {
//        Uart_Printf(&huart1,"[SD] f_open error: %d\r\n", res);
//    }
}




