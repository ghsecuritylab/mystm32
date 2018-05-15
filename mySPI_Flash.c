#include "mylib.h"
// CS(NSS)引脚 片选选普通GPIO即可
#define FLASH_SPI_CS_APBxClock_FUN	RCC_APB2PeriphClockCmd
#define FLASH_SPI_CS_CLK			RCC_APB2Periph_GPIOC
#define	FLASH_SPI_CS_PORT			GPIOC
#define FLASH_SPI_CS_PIN			GPIO_Pin_0
// SCK引脚
#define FLASH_SPI_SCK_APBxClock_FUN	RCC_APB2PeriphClockCmd
#define FLASH_SPI_SCK_CLK			RCC_APB2Periph_GPIOA
#define FLASH_SPI_SCK_PORT			GPIOA
#define FLASH_SPI_SCK_PIN			GPIO_Pin_5
// MISO引脚
#define FLASH_SPI_MISO_APBxClock_FUN	RCC_APB2PeriphClockCmd
#define FLASH_SPI_MISO_CLK			RCC_APB2Periph_GPIOA
#define	FLASH_SPI_MISO_PORT			GPIOA
#define FLASH_SPI_MISO_PIN			GPIO_Pin_6
// MOSI引脚
#define FLASH_SPI_MOSI_APBxClock_FUN	RCC_APB2PeriphClockCmd
#define FLASH_SPI_MOSI_CLK			RCC_APB2Periph_GPIOA
#define	FLASH_SPI_MOSI_PORT			GPIOA
#define FLASH_SPI_MOSI_PIN			GPIO_Pin_7
// SPI
#define FLASH_SPIx					SPI1
#define FLASH_SPI_APBxClock_FUN		RCC_APB2PeriphClockCmd
#define FLASH_SPI_CLK				RCC_APB2Periph_SPI1

//#define FLASH_ID				0xEF3015	/*W25X16*/
//#define FLASH_ID				0xEF4015	/*W25Q16*/
//#define FLASH_ID				0XEF4017	/*W25Q64*/
#define FLASH_ID				0XEF4018	/*W25Q128*/
#define FLASH_PageSize			256

#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg		0x05
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F

// WIP(busy)标志，FLASH内部正在写入
#define WIP_Flag				0x01
#define Dummy_Byte				0xFF

#define SPI_FLASH_CS_LOW()		GPIO_ResetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define SPI_FLASH_CS_HIGH()		GPIO_SetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)

void SPI_FLASH_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能SPI时钟 
	FLASH_SPI_APBxClock_FUN(FLASH_SPI_CLK, ENABLE);
	
	// 使能SPI引脚相关的时钟 
	FLASH_SPI_CS_APBxClock_FUN(FLASH_SPI_CS_CLK, ENABLE);
	FLASH_SPI_SCK_APBxClock_FUN(FLASH_SPI_SCK_CLK, ENABLE);
	FLASH_SPI_MISO_APBxClock_FUN(FLASH_SPI_MISO_PIN, ENABLE);
	FLASH_SPI_MOSI_APBxClock_FUN(FLASH_SPI_MOSI_PIN, ENABLE);
	
	// 配置SPI的CS引脚，普通IO即可 
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);

	// 停止信号 FLASH: CS引脚高电平
	SPI_FLASH_CS_HIGH();
	
	// 配置SPI的 SCK引脚
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure);

	// 配置SPI的 MISO引脚
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);

	// 配置SPI的 MOSI引脚
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);

	// SPI 模式配置。FLASH 芯片支持SPI模式0及模式3，据此设置CPOL CPHA
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		// CPOL
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	// CPHA
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;		// Flash 并不需要 CRC
	SPI_Init(FLASH_SPIx , &SPI_InitStructure);
	
	// 使能 SPI  
	SPI_Cmd(FLASH_SPIx , ENABLE);
}


// 使用SPI发送一个字节数据，返回接收到的数据
uint8_t SPI_FLASH_SendByte(uint8_t c)
{
	uint32_t SPITimeout = 0x1000;

	// 等待发送缓冲区为空，TXE事件
	while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0)
			return WIP_Flag;
	}
	
	// 写入数据寄存器，把要写入的数据写入发送缓冲区 
	SPI_I2S_SendData(FLASH_SPIx, c);

	SPITimeout = 0x1000;
	
	// 等待接收缓冲区非空，RXNE事件 
	while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0)
			return WIP_Flag;
	}
	
	// 读取数据寄存器，获取接收缓冲区数据
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}


// 等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
void SPI_FLASH_WaitForWriteEnd(void)
{
	// 选择 FLASH: CS 低 
	SPI_FLASH_CS_LOW();

	// 发送 读状态寄存器 命令 
	SPI_FLASH_SendByte(W25X_ReadStatusReg);

	// 读取FLASH芯片的状态寄存器，检查正在写入标志 
	while ((SPI_FLASH_SendByte(Dummy_Byte) & WIP_Flag) == SET);

	// 停止信号 FLASH: CS 高
	SPI_FLASH_CS_HIGH();
}

// 向FLASH发送 写使能 命令
void SPI_FLASH_WriteEnable(void)
{
	// 通讯开始：CS低 
	SPI_FLASH_CS_LOW();

	// 发送写使能命令
	SPI_FLASH_SendByte(W25X_WriteEnable);

	// 通讯结束：CS高 
	SPI_FLASH_CS_HIGH();
}
// 擦除指定FLASH扇区
void SPI_FLASH_SectorErase(uint32_t	SectorAddr)
{
	// 发送FLASH写使能命令 
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteEnd();
	
	// 选择FLASH: CS低电平
	SPI_FLASH_CS_LOW();
	
	// 发送扇区擦除指令
	SPI_FLASH_SendByte(W25X_SectorErase);
	// 发送擦除扇区地址的高位
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	// 发送擦除扇区地址的中位 
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	// 发送擦除扇区地址的低位 
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
	
	// 停止信号 FLASH: CS 高电平 
	SPI_FLASH_CS_HIGH();
	// 等待擦除完毕
	SPI_FLASH_WaitForWriteEnd();
}

// 擦除FLASH扇区，整片擦除
void SPI_FLASH_BulkErase(void)
{
	//发送FLASH写使能命令 
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteEnd();
	
	//选择FLASH: CS低电平
	SPI_FLASH_CS_LOW();
	
	//发送整块擦除指令
	SPI_FLASH_SendByte(W25X_ChipErase);
	
	//停止信号 FLASH: CS 高电平 
	SPI_FLASH_CS_HIGH();
	//等待擦除完毕
	SPI_FLASH_WaitForWriteEnd();
}

// 对FLASH按页写入数据，需要先擦除扇区并选中，且写入长度不超过 FLASH_PageSize
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	if (NumByteToWrite > FLASH_PageSize || NumByteToWrite == 0)
		return;
	
	// 发送FLASH写使能命令 
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteEnd();

	// 选择FLASH: CS低电平 
	SPI_FLASH_CS_LOW();
	
	// 写页写指令
	SPI_FLASH_SendByte(W25X_PageProgram);
	
	// 发送写地址的高位
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	// 发送写地址的中位
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	// 发送写地址的低位
	SPI_FLASH_SendByte(WriteAddr & 0xFF);

	while (NumByteToWrite--)
	{
		// 发送当前要写入的字节数据 
		SPI_FLASH_SendByte(*pBuffer++);
	}

	// 停止信号 FLASH: CS 高电平
	SPI_FLASH_CS_HIGH();

	//等待写入完毕
	SPI_FLASH_WaitForWriteEnd();
}


// 对FLASH写入数据，需要先擦除扇区并选中
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage, BytesInFirstPage;
	
	// 若 WriteAddr 是 FLASH_PageSize 整数倍，运算结果 Addr 值为0
	uint8_t Addr = WriteAddr % FLASH_PageSize;
	
	// WriteAddr 刚好按页对齐
	if (Addr == 0)
	{
		// 计算要写多少整数页
		NumOfPage =  NumByteToWrite / FLASH_PageSize;
		// 刚好按页对齐且不满一页
		if (NumOfPage == 0) 
		{
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else // 超过了一页
		{
			// 先把整数页都写了
			while (NumOfPage--)
			{
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, FLASH_PageSize);
				WriteAddr +=  FLASH_PageSize;
				pBuffer += FLASH_PageSize;
			}
			// 剩下不满一页的写完
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite % FLASH_PageSize);
		}
	}
	// 若地址与 SPI_FLASH_PageSize 不对齐  
	else 
	{
		BytesInFirstPage = FLASH_PageSize - Addr;
		// 当前页写不下
		if (NumByteToWrite > BytesInFirstPage)
		{
			// 先把这页补满
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, BytesInFirstPage);
			// 计算剩下的字节数
			NumByteToWrite -= BytesInFirstPage;
			WriteAddr += BytesInFirstPage;
			// 计算要写多少整数页
			NumOfPage =  NumByteToWrite / FLASH_PageSize;
			while(NumOfPage--)
			{
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, FLASH_PageSize);
				NumByteToWrite -= FLASH_PageSize;
				WriteAddr += FLASH_PageSize;
			}
			// 最后剩下不到一页
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else	// 第一页能写下，直接写
		{
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
	}
}

// 读取FLASH数据
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPI_FLASH_WaitForWriteEnd();
	
	// 选择FLASH: CS低电平
	SPI_FLASH_CS_LOW();

	// 发送 读 指令
	SPI_FLASH_SendByte(W25X_ReadData);

	// 发送 读 地址高位 
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	// 发送 读 地址中位 
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	// 发送 读 地址低位 
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
	// 读取数据 
	while (NumByteToRead--)
	{
		// 读取一个字节
		*pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
		// 指向下一个字节缓冲区
		++pBuffer;
	}

	// 停止信号 FLASH: CS 高电平 
	SPI_FLASH_CS_HIGH();
}

// 读取FLASH ID
uint32_t SPI_FLASH_ReadID(void)
{
	uint32_t b0, b1, b2;

	
	SPI_FLASH_WaitForWriteEnd();
	
	// 开始通讯：CS低电平 
	SPI_FLASH_CS_LOW();

	// 发送 JEDEC 指令，读取ID 
	SPI_FLASH_SendByte(W25X_JedecDeviceID);
	b0 = SPI_FLASH_SendByte(Dummy_Byte);
	b1 = SPI_FLASH_SendByte(Dummy_Byte);
	b2 = SPI_FLASH_SendByte(Dummy_Byte);

	// 停止通讯：CS高电平 
	SPI_FLASH_CS_HIGH();

	// 把数据组合起来，作为函数的返回值
	return ((b0 << 16) | (b1 << 8) | b2);
}

// 读取FLASH Device ID
uint32_t SPI_FLASH_ReadDeviceID(void)
{
	uint32_t Temp = 0;

	SPI_FLASH_WaitForWriteEnd();
	
	// Select the FLASH: Chip Select low 
	SPI_FLASH_CS_LOW();

	// Send RDID instruction 
	SPI_FLASH_SendByte(W25X_DeviceID);
	SPI_FLASH_SendByte(Dummy_Byte);
	SPI_FLASH_SendByte(Dummy_Byte);
	SPI_FLASH_SendByte(Dummy_Byte);
	
	// Read a byte from the FLASH
	Temp = SPI_FLASH_SendByte(Dummy_Byte);

	// Deselect the FLASH: Chip Select high
	SPI_FLASH_CS_HIGH();

	return Temp;
}

// 进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
	// 通讯开始：CS低 
	SPI_FLASH_CS_LOW();

	// 发送 掉电 命令 
	SPI_FLASH_SendByte(W25X_PowerDown);

	// 通讯结束：CS高 
	SPI_FLASH_CS_HIGH();
}

// 唤醒
void SPI_Flash_WAKEUP(void)
{
	// 选择 FLASH: CS 低 
	SPI_FLASH_CS_LOW();

	// 发送 上电 命令 
	SPI_FLASH_SendByte(W25X_ReleasePowerDown);

	// 停止信号 FLASH: CS 高 
	SPI_FLASH_CS_HIGH();
}

#include <stdio.h>
#include <stdlib.h>
void SPI_FLASH_EXAMPLE(void)
{	
	#define BufferSize		30
	uint8_t Tx_Buffer[BufferSize] = "Hello,EveryOne";
	uint8_t Rx_Buffer[BufferSize] = {0};
	
	SPI_FLASH_Init();
	
	// 获取 Flash Device ID
	printf("DeviceID=%x\n", SPI_FLASH_ReadDeviceID());
	
	// 检验 SPI Flash ID
	if (FLASH_ID == SPI_FLASH_ReadID())
	{
		// 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除
		// 这里擦除4K，即一个扇区，擦除的最小单位是扇区
		SPI_FLASH_SectorErase(0x00000);
		
		// 将发送缓冲区的数据写到flash中，写一页，一页的大小为BufferSize个字节
		SPI_FLASH_BufferWrite(Tx_Buffer, 0x00000, BufferSize);
		
		// 将刚刚写入的数据读出来放到接收缓冲区中
		SPI_FLASH_BufferRead(Rx_Buffer, 0x00000, BufferSize);
		
		// 检查写入的数据与读出的数据是否相等
		printf("Data=%s\n", Rx_Buffer);
	}
	else
		printf("Cannot find specified Flash!\n");
}

