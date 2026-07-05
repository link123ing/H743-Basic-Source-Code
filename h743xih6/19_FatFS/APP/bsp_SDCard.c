#include "bsp_SDCard.h"
int sdcard_status = 0;
HAL_SD_CardCIDTypeDef sdcard_cid;
uint8_t write_buf[512] = "2025年12月15日下午16:37分完成该测试";
uint8_t read_buf[512];

void SD_State_Test(void)
{
	Uart_Printf(&huart1,"Micro SD Card Test...\r\n");
	sdcard_status = HAL_SD_GetCardState(&hsd1);
	if(sdcard_status == HAL_SD_CARD_TRANSFER)
	{
			Uart_Printf(&huart1,"SD card init ok!\r\n\r\n");
			
			//打印SD卡基本信息
			Uart_Printf(&huart1,"SD card information!\r\n");
			Uart_Printf(&huart1,"CardCapacity: %llu Mb\r\n",(((unsigned long long)hsd1.SdCard.BlockSize*hsd1.SdCard.BlockNbr)/1024/1024));
			Uart_Printf(&huart1,"CardBlockSize: %d \r\n",hsd1.SdCard.BlockSize);
			Uart_Printf(&huart1,"RCA: %d \r\n",hsd1.SdCard.RelCardAdd);
			Uart_Printf(&huart1,"CardType: %d \r\n",hsd1.SdCard.CardType);
			
			//读取并打印SD卡的CID信息
			HAL_SD_GetCardCID(&hsd1,&sdcard_cid);
			Uart_Printf(&huart1,"ManufacturerID: %d \r\n",sdcard_cid.ManufacturerID);
	}
	else
	{ 
			Uart_Printf(&huart1,"SD card init fail!\r\n" );

	}

}

void SD_Block_Erase(void)
{
	/* 擦除SD卡块 */
	Uart_Printf(&huart1,"------------------- Block Erase -------------------------------\r\n");
	sdcard_status = HAL_SD_Erase(&hsd1, 0, 512);
	if (sdcard_status == 0)
	{
			Uart_Printf(&huart1,"Erase block ok\r\n");
	}
	else
	{
			Uart_Printf(&huart1,"Erase block fail\r\n");
	}

}

void SD_Block_Write(void)
{

	/* 向SD卡块写入数据 */
	Uart_Printf(&huart1,"------------------- Write SD card block data Test ------------------\r\n");
	sdcard_status = HAL_SD_WriteBlocks(&hsd1,(uint8_t *)write_buf,0,1,0xffff);
	if(sdcard_status == 0)
	{ 
			Uart_Printf(&huart1,"Write block data ok \r\n" );
	}
	else
	{
			Uart_Printf(&huart1,"Write block data fail!\r\n " );
	}


}

void SD_Block_Read(void)
{
	/* 读取操作之后的数据 */
	Uart_Printf(&huart1,"------------------- Read SD card block data after Write ------------------\r\n");
	sdcard_status = HAL_SD_ReadBlocks(&hsd1,(uint8_t *)read_buf,0,1,0xffff);
	if(sdcard_status == 0)
	{ 
			Uart_Printf(&huart1,"Read block data ok \r\n" );
			Uart_Printf(&huart1,"%s \r\n", read_buf);

	}

}

// 擦除整张 SD 卡（你要的功能）
void SD_Erase_All_Card(void)
{
    uint32_t erase_start = 0;               // 从第0块开始
    uint32_t erase_end = hsd1.SdCard.BlockNbr - 1;  // 擦到最后一块 = 整张卡

    Uart_Printf(&huart1,"------------------- Erase Entire SD Card -------------------------------\r\n");
    Uart_Printf(&huart1,"Start Block: %lu | End Block: %lu | Total Blocks: %lu\r\n",
           erase_start, erase_end, hsd1.SdCard.BlockNbr);

    // 执行擦除
    sdcard_status = HAL_SD_Erase(&hsd1, erase_start, erase_end);

    if (sdcard_status == HAL_OK)
    {
        Uart_Printf(&huart1,"? Entire SD card erased successfully!\r\n");
    }
    else
    {
        Uart_Printf(&huart1,"? SD card erase fail! Error code: %d\r\n", sdcard_status);
    }
}
