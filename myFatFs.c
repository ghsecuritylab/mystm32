#include "mylib.h"
#include "FATFS/ff.h"
#include <stdio.h>
#include <string.h>

// 递归扫描 path 路径下的文件
FRESULT scan_files (char* path)
{ 
	FRESULT result;
	FILINFO fno;
	DIR dir;
	uint16_t i;
	
	printf("OPENING_DIR\n");
	// 打开目录
	result = f_opendir(&dir, path); 
	if (result != FR_OK)
		return result;

	i = strlen(path);
	for (;;)
	{
		printf("READING_DIR\n");
		// 读取目录下的下一个文件
		result = f_readdir(&dir, &fno);

		// 为空时表示所有项目读取完毕，跳出
		if (result != FR_OK || fno.fname[0] == 0)
			break;
		// 点表示当前目录，跳过
		if (*fno.fname == '.') continue;

		// 目录，递归读取
		if (fno.fattrib & AM_DIR)
		{
			// 合成完整目录名
			sprintf(path + i, "/%s", fno.fname);
			printf("%s\n",path);

			// 递归遍历
			result = scan_files(path);
			if (result != FR_OK)
				break; 

			path[i] = '\0';
		}
		else
		{
			// 输出文件名
			printf("%s/%s\n", path, fno.fname);
			// 可以在这里提取特定格式的文件路径

		}
	}
	return result;
}


// 这两个对象占用较大的空间，因此作为全局变量以防栈溢出
FATFS fs;				// FatFs文件系统对象
FIL fnew;				// 文件对象
BYTE work[FF_MAX_SS];	// 格式化时使用的工作区，越大处理越快

void FATFS_EXAMPLE(void)
{
	FRESULT result;			// 文件操作结果
	UINT num;				// 实际读写字节量
	BYTE ReadBuffer[100]={0};
	char WriteBuffer[] = "Hello,World!\r\n"; 
	DIR dir;
	FILINFO fno;
	FATFS *pfs;
	DWORD free_clust, free_sect, total_sect;
	
	// 在外部SPI Flash挂载文件系统，最终对SPI设备初始化，流程：
	// f_mount()->find_volume()->disk_initialize()->SPI_FLASH_Init()
	result = f_mount(&fs, "1:", 1);	// 0：不立即执行，1：立即执行

	// 如果没有文件系统就格式化创建创建文件系统
	if (result == FR_NO_FILESYSTEM)
	{
		// 格式化 FM_ANY 使用 FM_FAT, FM_FAT32, FM_EXFAT
		result = f_mkfs("1:", FM_ANY, 0, work, sizeof work);

		if (result == FR_OK)
		{
			printf("SUCCEED_TO_FORMAT\n");
			// 格式化后，先取消挂载
			result = f_mount(NULL, "1:", 1);	// 0：不立即执行，1：立即执行
			// 重新挂载
			result = f_mount(&fs, "1:", 1);		// 0：不立即执行，1：立即执行
			if (result == FR_OK)
			{
				printf("DEVICE_MOUNTED\n");
			}
			else
			{
				printf("DEVICE_NOT_MOUNTED\n");
				return;
			}
		}
		else
		{
			printf("FAILED_TO_FORMAT\n");
			return;
		}
	}
	else if (result != FR_OK)
	{
		// SPI Flash初始化不成功
		printf("FAILED_TO_INIT\n");
		return;
	}

	// 文件系统测试：写测试
	// 打开文件，每次都以新建的形式打开，属性为可写
	result = f_open(&fnew, "1:first_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if (result == FR_OK)
	{
		// 将指定存储区内容写入到文件内
		result = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &num);
		if (result == FR_OK)
		{
			printf("WRITE_BYTE_NUM=%d\n", num);
			printf("WRITE=%s\n", WriteBuffer);
		}
		else
		{
			printf("FAILED_TO_WRITE_FILE=%d\n", result);
		}

		// 不再读写，关闭文件
		f_close(&fnew);
	}
	else
	{
		printf("FAILED_TO_OPEN_FILE\n");
	}
	
	// 文件系统测试：读测试
	result = f_open(&fnew, "1:first_file.txt", FA_OPEN_EXISTING | FA_READ);
	if (result == FR_OK)
	{
		result = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &num); 
		if (result == FR_OK)
		{
		  printf("BYTE_READ_NUM=%d\n",num);
		  printf("READ=%s\n", ReadBuffer);
		}
		else
		{
			printf("FAILED_TO_READ_FILE=%d\n", result);
		}
	}
	else
	{
		printf("FAILED_TO_OPEN_FILE\n");
	}
	// 不再读写，关闭文件
	f_close(&fnew);

	// 获取设备信息和空簇大小
	result = f_getfree("1:", &free_clust, &pfs);

	// 计算得到总的扇区个数和空扇区个数
	total_sect = (pfs->n_fatent - 2) * pfs->csize;
	free_sect = free_clust * pfs->csize;

	// 打印信息(4096 字节/扇区)
	printf("TOTAL_SIZE=%10lu KB\n", total_sect*4);
	printf("AVAILABLE=%10lu KB\n", free_sect*4);

	result = f_open(&fnew, "1:first_file.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
	if (result == FR_OK)
	{
		// 文件定位
		result = f_lseek(&fnew, f_size(&fnew));
		if (result == FR_OK)
		{
			// 格式化写入，参数格式类似printf函数
			f_printf(&fnew, "this is a new line\r\n");

			// 文件定位到文件起始位置
			result = f_lseek(&fnew, 0);

			// 读取文件所有内容到缓存区
			result = f_read(&fnew, ReadBuffer, f_size(&fnew), &num);

			if (result == FR_OK)
			{
				printf("READ=%s\n", ReadBuffer);
			}
		}
		f_close(&fnew);    

		// 尝试打开目录
		result = f_opendir(&dir, "1:test");
		if (result != FR_OK)
		{
			// 打开目录失败，就创建目录
			result = f_mkdir("1:test");
			if (result == FR_OK)
			{
				// 重命名并移动文件
				result = f_rename("1:first_file.txt", "1:test/test_in_dir.txt");
			}
		}
		else
		{
			printf("DIR_ALREADY_EXISTS\n");
			// 目录已经存在则打开成功，关闭目录
			result = f_closedir(&dir);
			// 删除文件
			// f_unlink("1:test/test_in_dir.txt");
		}
	}
	
	// 获取文件信息
	result = f_stat("1:test/test_in_dir.txt", &fno);
	if (result == FR_OK)
	{
		printf("FILE_SIZE=%ld Bytes\n", fno.fsize);
		printf("TimeStamp=%u/%02u/%02u, %02u:%02u\n",
				(fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
				fno.ftime >> 11, fno.ftime >> 5 & 63);
		printf("Attributes=%c%c%c%c%c\n",
				(fno.fattrib & AM_DIR) ? 'D' : '-',		// 是一个目录
				(fno.fattrib & AM_RDO) ? 'R' : '-',		// 只读文件
				(fno.fattrib & AM_HID) ? 'H' : '-',		// 隐藏文件
				(fno.fattrib & AM_SYS) ? 'S' : '-',		// 系统文件
				(fno.fattrib & AM_ARC) ? 'A' : '-');	// 档案文件
	}
	else
	{
		printf("FAILED_TO_GET_FILE_STAT\n");
	}

	// 遍历目录
	if (scan_files("1:") != FR_OK)
		printf("FAILED_TO_LIST_FILES\n");
	
	// 不再使用文件系统，取消挂载文件系统
	f_mount(NULL, "1:", 1);	// 0：不立即执行，1：立即执行
}
