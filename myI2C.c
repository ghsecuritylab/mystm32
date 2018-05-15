#include "mylib.h"

#define EEPROM_G_I2CClockCmd	RCC_APB2PeriphClockCmd
#define EEPROM_G_I2C 			RCC_APB2Periph_GPIOB
#define EEPROM_I2CClockCmd		RCC_APB1PeriphClockCmd
#define EEPROM_I2C				RCC_APB1Periph_I2C1
#define I2CX					I2C1
#define EEPROM_GPIO_CLK			GPIOB
#define EEPROM_GPIO_CLK_Pin		GPIO_Pin_6
#define EEPROM_GPIO_SDA			GPIOB
#define EEPROM_GPIO_SDA_Pin		GPIO_Pin_7

#define EEPROM_WORD_ADDRESS

#ifndef EEPROM_WORD_ADDRESS
	// AT24C01/02 每页8字节
	#define EEPROM_PAGE_SIZE	8
#else
	// AT24C04/08A/16A每页16字节
	#define EEPROM_PAGE_SIZE	16
#endif

/*
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0xA0
 * 1 0 1 0 0  0  0  1 = 0xA1 
 *
 * 1Kbit A2,A1,A0 用作设备地址，使用 7 位内部数据地址，8bytes*16pages
 * 2Kbit A2,A1,A0 用作设备地址，使用 8 位内部数据地址，8bytes*32pages
 * 4Kbit 不使用 A0，此位用作内部数据地址（共9位），16bytes*16pages*2blocks
 * 8Kbit 不使用 A1,A0，这两位用作内部数据地址（共10位），16bytes*16pages*4blocks
 * 16Kbit 不适用 A2,A1,A0，全部悬空，全部用作内部数据地址（共11位），16bytes*16pages*8blocks
 */
uint16_t EEPROM_ADDRESS = 0xA0;

void I2C_CONFIG()
{
	EEPROM_G_I2CClockCmd(EEPROM_G_I2C, ENABLE);
	EEPROM_I2CClockCmd(EEPROM_I2C, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_AF_OD;	// I2C必须用复用开漏输出
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	
	gpio_init_t.GPIO_Pin = EEPROM_GPIO_CLK_Pin;
	GPIO_Init(EEPROM_GPIO_CLK, &gpio_init_t);	// CLK
	
	gpio_init_t.GPIO_Pin = EEPROM_GPIO_SDA_Pin;
	GPIO_Init(EEPROM_GPIO_SDA, &gpio_init_t);	// SDA
	
	I2C_InitTypeDef i2c_init_t;
	i2c_init_t.I2C_Mode = I2C_Mode_I2C;
	i2c_init_t.I2C_ClockSpeed = 400000;	// 400kbps
	i2c_init_t.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c_init_t.I2C_Ack = I2C_Ack_Enable;
	i2c_init_t.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_init_t.I2C_OwnAddress1 = 0x0A;	// 和外设的I2C器件地址不一致即可
	I2C_Init(I2CX, &i2c_init_t);
	I2C_Cmd(I2CX, ENABLE);
}

// 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
uint8_t I2C_EE_CheckDevice()
{
	uint32_t timeout;
	
	I2C_CONFIG();

	I2C_GenerateSTART(I2CX, ENABLE);
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}
	
	I2C_Send7bitAddress(I2CX, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}

	I2C_GenerateSTOP(I2CX, ENABLE);
	return 1;
}

uint8_t I2C_EE_ByteWrite(uint8_t c, uint8_t addr)
{
	uint32_t timeout;
	
	// 第0步：发停止信号，启动内部写操作
	I2C_GenerateSTOP(I2CX, ENABLE);

	// 第1步：发起I2C总线启动信号
	I2C_GenerateSTART(I2CX, ENABLE);
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}

	// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
	I2C_Send7bitAddress(I2CX, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	// Test on EV6 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}
	
	// 第3步：发送字节地址
	I2C_SendData(I2CX, addr);
	// Test on EV8 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}
	
	// Send the current byte
	I2C_SendData(I2CX, c);
	// Test on EV8 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}
	
	I2C_GenerateSTOP(I2CX, ENABLE);
		
	// write cycle time (t_WR) max 5ms
	delay_ms(5);
	
	return 1;
}

void EEPROM_Wait()
{
	// 每调用一次 EEPROM_ByteWrite 都要进行忙状态检测
	do {
		I2C_GenerateSTART(I2CX, ENABLE);
		I2C_ReadRegister(I2CX, I2C_Register_SR1);		//I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT);
		I2C_Send7bitAddress(I2CX, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	} while(!(I2C_ReadRegister(I2CX, I2C_Register_SR1) & 0x0002));
	//} while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_ClearFlag(I2CX, I2C_FLAG_AF);
	I2C_GenerateSTOP(I2CX, ENABLE);
}

uint8_t I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
	uint32_t timeout;
	uint16_t i, addr = WriteAddr;
	/* 
		写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
		简单的处理方法为：按字节写操作模式，没写1个字节，都发送地址。
		但为了提高连续写的效率，本函数采用按页写。
	*/
	for (i = 0; i < NumByteToWrite; i++)
	{
		// 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址
		if ((i == 0) || (addr & (EEPROM_PAGE_SIZE - 1)) == 0)
		{
			// 第0步：发停止信号，启动内部写操作
			I2C_GenerateSTOP(I2CX, ENABLE);
			
			// write cycle time (t_WR) max 5ms
			delay_ms(5);
			
			// 第1步：发起I2C总线启动信号
			I2C_GenerateSTART(I2CX, ENABLE);
			timeout = 0x1000;
			while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT))
				if (timeout-- == 0) {
					I2C_GenerateSTOP(I2CX, ENABLE);
					return 0;
				}
			
			// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
			I2C_Send7bitAddress(I2CX, EEPROM_ADDRESS, I2C_Direction_Transmitter);
			// Test on EV6 and clear it
			timeout = 0x1000;
			while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
				if (timeout-- == 0) {
					I2C_GenerateSTOP(I2CX, ENABLE);
					return 0;
				}

			// 第3步：发送字节地址
			I2C_SendData(I2CX, addr);
			// Test on EV8 and clear it
			timeout = 0x1000;
			while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
				if (timeout-- == 0) {
					I2C_GenerateSTOP(I2CX, ENABLE);
					return 0;
				}
		}
		
		// Send the current byte
		I2C_SendData(I2CX, *pBuffer++);
		// Test on EV8 and clear it
		timeout = 0x1000;
		while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			if (timeout-- == 0) {
				I2C_GenerateSTOP(I2CX, ENABLE);
				return 0;
			}

		addr++;	// 地址增1
	}
	
	// 命令执行成功，发送I2C总线停止信号
	// Send STOP condition
	I2C_GenerateSTOP(I2CX, ENABLE);
	
	// write cycle time (t_WR) max 5ms
	delay_ms(5);
	return 1;
}

uint8_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	uint32_t timeout=0x1000;
	
	//*((u8 *)0x4001080c) |=0x80;

	while(I2C_GetFlagStatus(I2CX, I2C_FLAG_BUSY))
		if((timeout--) == 0)
			return 0;

	// Send START condition
	I2C_GenerateSTART(I2CX, ENABLE);

	//*((u8 *)0x4001080c) &=~0x80;
	
	// Test on EV5 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}
	
	// Send EEPROM address for write
	I2C_Send7bitAddress(I2CX, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	// Test on EV6 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}

	// Clear EV6 by setting again the PE bit
	I2C_Cmd(I2CX, ENABLE);

	// 发送字节地址
	I2C_SendData(I2CX, ReadAddr);
	// Test on EV8 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}

	// Send STRAT condition a second time
	I2C_GenerateSTART(I2CX, ENABLE);
	// Test on EV5 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}

	// Send EEPROM address for read
	I2C_Send7bitAddress(I2CX, EEPROM_ADDRESS, I2C_Direction_Receiver);
	// Test on EV6 and clear it
	timeout = 0x1000;
	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		if (timeout-- == 0) {
			I2C_GenerateSTOP(I2CX, ENABLE);
			return 0;
		}

	// While there is data to be read
	while(NumByteToRead)
	{
		if(NumByteToRead == 1)
		{
			// Disable Acknowledgement
			I2C_AcknowledgeConfig(I2CX, DISABLE);

			// Send STOP Condition
			I2C_GenerateSTOP(I2CX, ENABLE);
		}

		// Test on EV7 and clear it
		timeout = 0x10000;
		while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_RECEIVED))
			if (timeout-- == 0) {
				I2C_GenerateSTOP(I2CX, ENABLE);
				return 0;
			}
		
		// Read a byte from the EEPROM
		*pBuffer = I2C_ReceiveData(I2CX);

		// Point to the next location where the byte read will be saved
		pBuffer++; 

		// Decrement the read bytes counter
		NumByteToRead--;
	}

	// Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(I2CX, ENABLE);
	return 1;
}

#include <stdio.h>
#include <stdlib.h>
void I2C_EE_EXAMPLE(void)
{	
	#define BufferSize		20
	uint8_t Tx_Buffer[BufferSize] = "Hello,EveryOne~!";
	uint8_t Rx_Buffer[BufferSize] = {0};

	if (!I2C_EE_CheckDevice())
	{
		printf("Cannot find specified EEPROM!\n");
		return;
	}

	// 将发送缓冲区的数据写到flash中，写一页，一页的大小为BufferSize个字节
	if (I2C_EE_BufferWrite(Tx_Buffer, 0x50, BufferSize-1))
	{
		// 将刚刚写入的数据读出来放到接收缓冲区中
		I2C_EE_BufferRead(Rx_Buffer, 0x50, BufferSize-1);

		// 检查写入的数据与读出的数据是否相等
		printf("\nDATA=%s\n", Rx_Buffer);
	}
	else
	{
		printf("WRITE_FAILED\n");
	}
}

