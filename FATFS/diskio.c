/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "../mylib.h"

// 为每个设备定义一个物理编号
// SD 卡
#define ATA			    0
// SPI FLASH 芯片
#define SPI_FLASH		1

//#define FLASH_ID				0xEF3015	/*W25X16*/
//#define FLASH_ID				0xEF4015	/*W25Q16*/
//#define FLASH_ID				0XEF4017	/*W25Q64*/
#define FLASH_ID				0XEF4018	/*W25Q128*/
#define FLASH_PageSize			256
#define FLASH_SectorSize		4096

// 设备状态，pdrv 物理编号
DSTATUS disk_status(BYTE pdrv)
{
	DSTATUS status = STA_NOINIT;
	switch (pdrv)
	{
	case ATA:
		break;
	case SPI_FLASH:
		// SPI Flash状态检测：读取SPI Flash 设备ID
		if(FLASH_ID == SPI_FLASH_ReadID())
		{
			// 设备ID读取结果正确
			status &= ~STA_NOINIT;
		}
		break;
	}
	return status;
}

// 设备初始化，pdrv 物理编号
DSTATUS disk_initialize(BYTE pdrv)
{
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) {
	case ATA:			// SD CARD
		break;
    case SPI_FLASH:		// SPI Flash
		SPI_FLASH_Init();
	    SPI_FLASH_WakeUp();
		// SPI Flash状态检测：读取SPI Flash 设备ID
		if(FLASH_ID == SPI_FLASH_ReadID())
		{
			// 设备ID读取结果正确
			status &= ~STA_NOINIT;
		}
		break;
	}
	return status;
}

// 读扇区：读取扇区内容到指定存储区
DRESULT disk_read (
	BYTE pdrv,		/* 设备物理编号 */
	BYTE *buff,		/* 数据缓存区 */
	DWORD sector,	/* 扇区索引 */
	UINT count		/* 扇区个数(1..128) */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv)
	{
	case ATA:		// SD CARD
		break;
	case SPI_FLASH:
		SPI_FLASH_BufferRead(buff, sector*FLASH_SectorSize, count*FLASH_SectorSize);
		status = RES_OK;
		break;
	}
	return status;
}

// 写扇区：见数据写入指定扇区空间上
DRESULT disk_write (
	BYTE pdrv,				/* 设备物理编号(0..) */
	const BYTE *buff,		/* 欲写入数据的缓存区 */
	DWORD sector,			/* 扇区首地址 */
	UINT count				/* 扇区个数(1..128) */
)
{
	uint32_t write_addr; 
	DRESULT status = RES_PARERR;
	if (count==0)
	{
		return RES_PARERR;
	}

	switch (pdrv)
	{
	case ATA:	// SD CARD
		break;
	case SPI_FLASH:
		write_addr = sector*FLASH_SectorSize;
		SPI_FLASH_SectorErase(write_addr);
		SPI_FLASH_BufferWrite((uint8_t*)buff, write_addr, count*FLASH_SectorSize);
		status = RES_OK;
		break;
	}
	return status;
}


// 其他控制
DRESULT disk_ioctl (
	BYTE pdrv,		/* 物理编号 */
	BYTE cmd,		/* 控制指令 */
	void *buff		/* 写入或者读取数据地址指针 */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv)
	{
	case ATA:		// SD CARD
		break;
	case SPI_FLASH:
		switch (cmd)
		{
		case GET_SECTOR_COUNT:			// 扇区数量：16*1024*1024/4096
			*(DWORD*)buff = 4096;
			break;
		case GET_SECTOR_SIZE:			// 扇区大小
			*(WORD*)buff = FLASH_SectorSize;
			break;
		case GET_BLOCK_SIZE:
			*(DWORD*)buff = 1;			// 同时擦除扇区个数
			break;
		}
		status = RES_OK;
		break;
	}
	return status;
}

#if _FS_NORTC==0
// 返回当前时间戳
DWORD get_fattime(void) {
	return	((DWORD)(2018 - 1980) << 25)	// Year 25:31
			| ((DWORD)1 << 21)				// Month 21:24
			| ((DWORD)1 << 16)				// Mday 16:20
			| ((DWORD)0 << 11)				// Hour 11:15
			| ((DWORD)0 << 5)				// Min 5:10
			| ((DWORD)0 / 2U);				// Sec 0:4
}
#endif
