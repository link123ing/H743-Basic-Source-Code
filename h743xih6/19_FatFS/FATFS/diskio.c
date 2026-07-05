/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */

/* Example: Mapping of physical drive number for each drive */
//#define DEV_FLASH	0	/* Map FTL to physical drive 0 */
//#define DEV_MMC		1	/* Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Map USB MSD to physical drive 2 */

#include "bsp_system.h"
#define DEV_SD 0
#define BLOCK_SIZE 512

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{

	switch (pdrv) {
//	case DEV_RAM :
//		result = RAM_disk_status();

//		// translate the reslut code here

//		return stat;

//	case DEV_MMC :
//		result = MMC_disk_status();

//		// translate the reslut code here

//		return stat;

//	case DEV_USB :
//		result = USB_disk_status();

//		// translate the reslut code here

//		return stat;
		case DEV_SD:
		{
			if(hsd1.State == HAL_SD_STATE_READY && 
				HAL_SD_GetCardState(&hsd1)== HAL_SD_CARD_TRANSFER)
			{
				return 0;
			}
		}
		break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{


	switch (pdrv) {
//	case DEV_RAM :
//		result = RAM_disk_initialize();

//		// translate the reslut code here

//		return stat;

//	case DEV_MMC :
//		result = MMC_disk_initialize();

//		// translate the reslut code here

//		return stat;

//	case DEV_USB :
//		result = USB_disk_initialize();

//		// translate the reslut code here

//		return stat;
		case DEV_SD:
		{
			if (HAL_SD_Init(&hsd1) == HAL_OK &&
        HAL_SD_GetCardState(&hsd1) == HAL_SD_CARD_TRANSFER)
			{
				return 0;
			}
		}
		break;
		
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
//	DRESULT res;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :
//		// translate the arguments here

//		result = RAM_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_MMC :
//		// translate the arguments here

//		result = MMC_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;
//	}

//	return RES_PARERR;
	if (pdrv != DEV_SD || disk_status(DEV_SD) & STA_NOINIT)
        return RES_NOTRDY;

    if (HAL_SD_ReadBlocks(&hsd1, buff, sector, count, 1000) != HAL_OK)
        return RES_ERROR;

    while (HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER);
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
//	DRESULT res;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :
//		// translate the arguments here

//		result = RAM_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_MMC :
//		// translate the arguments here

//		result = MMC_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;
//	}

//	return RES_PARERR;
if (pdrv != DEV_SD || disk_status(DEV_SD) & STA_NOINIT)
        return RES_NOTRDY;

    if (HAL_SD_WriteBlocks(&hsd1, (uint8_t *)buff, sector, count, 1000) != HAL_OK)
        return RES_ERROR;

    while (HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER);
    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
//	DRESULT res;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :

//		// Process of the command for the RAM drive

//		return res;

//	case DEV_MMC :

//		// Process of the command for the MMC/SD card

//		return res;

//	case DEV_USB :

//		// Process of the command the USB drive

//		return res;
//	}

//	return RES_PARERR;
if (pdrv != DEV_SD || disk_status(DEV_SD) & STA_NOINIT)
        return RES_NOTRDY;

    HAL_SD_CardInfoTypeDef ci;
    HAL_SD_GetCardInfo(&hsd1, &ci);

    switch (cmd)
    {
    case CTRL_SYNC:            /* 无缓存，直接 OK */
        return RES_OK;

    case GET_SECTOR_COUNT:     /* 总扇区数 */
        *(LBA_t *)buff = ci.LogBlockNbr;
        return RES_OK;

    case GET_SECTOR_SIZE:      /* 扇区字节数 */
        *(WORD *)buff = BLOCK_SIZE;
        return RES_OK;

    case GET_BLOCK_SIZE:       /* 擦除块大小（单位：扇区） */
        *(DWORD *)buff = 1;    /* 简化成 1 */
        return RES_OK;

    default:
        return RES_PARERR;
    }
}

