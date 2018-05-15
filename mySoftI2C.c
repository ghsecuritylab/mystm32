#include "mylib.h"

#define EEPROM_I2C_WR	0	/* 写控制bit */
#define EEPROM_I2C_RD	1	/* 读控制bit */

// 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚

#define I2C_G_ClockCmd	RCC_APB2PeriphClockCmd	/* GPIO端口时钟 */
#define I2C_G	 		RCC_APB2Periph_GPIOB	/* GPIO端口时钟 */
#define I2C_GPIO		GPIOB					/* GPIO端口 */
#define I2C_SCL			GPIO_Pin_6				/* 连接到SCL时钟线的GPIO */
#define I2C_SDA			GPIO_Pin_7				/* 连接到SDA数据线的GPIO */

#define	fastSet(p,i)	{p->BSRR=i;}
#define fastReset(p,i)	{p->BRR=i;}
#define fastToggle(p,i)	{p->ODR^=i;}

#define I2C_SDA_1()		fastSet(I2C_GPIO,I2C_SDA)
#define I2C_SDA_0()		fastReset(I2C_GPIO,I2C_SDA)
#define I2C_SCL_1()		fastSet(I2C_GPIO,I2C_SCL)
#define I2C_SCL_0()		fastReset(I2C_GPIO,I2C_SCL)

// 读SDA状态
#define I2C_SDA_READ()  ((I2C_GPIO->IDR & I2C_SDA) != 0)

#define EEPROM_WORD_ADDRESS

#ifndef EEPROM_WORD_ADDRESS
	// AT24C01/02 每页8字节
	#define EEPROM_PAGE_SIZE	8
#else
	// AT24C04/08A/16A每页16字节
	#define EEPROM_PAGE_SIZE	16
#endif

#include <stdio.h>
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
extern uint16_t EEPROM_ADDRESS;

void i2c_Delay(void)
{
	/*　下面的时间是通过逻辑分析仪测试得到的。（工作条件：CPU主频72MHz ，MDK编译环境，1级优化）
		循环次数为10时，SCL频率 = 205KHz
		循环次数为7时，SCL频率 = 347KHz，SCL高电平时间1.5us，SCL低电平时间2.87us
	 	循环次数为5时，SCL频率 = 421KHz，SCL高电平时间1.25us，SCL低电平时间2.375us	*/
	for (uint8_t i = 0; i < 10; i++);
}

// CPU发起I2C总线停止信号
void i2c_Stop(void)
{
	// 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	
	I2C_SDA_1();
}

// CPU发起I2C总线启动信号
void i2c_Start(void)
{
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	
	// 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
	I2C_SDA_0();
	i2c_Delay();
	
	I2C_SCL_0();
	i2c_Delay();
}

// 配置I2C总线的GPIO，采用模拟IO的方式实现
void i2c_Config(void)
{
	GPIO_InitTypeDef gpio_init_t;

	I2C_G_ClockCmd(I2C_G, ENABLE);	// 打开GPIO时钟

	gpio_init_t.GPIO_Pin = I2C_SCL | I2C_SDA;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_OD;	// 开漏输出
	GPIO_Init(I2C_GPIO, &gpio_init_t);

	// 给一个停止信号，复位I2C总线上的所有设备到待机模式
	i2c_Stop();
}


// CPU向I2C总线设备发送8bit数据
void i2c_SendByte(uint8_t c)
{
	uint8_t i;

	// 先发送字节的高位bit7
	for (i = 0; i < 8; i++)
	{		
		if (c & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();	
		I2C_SCL_0();
		
		if (i == 7)
			I2C_SDA_1(); // 释放总线
		
		c <<= 1;	// 左移 1bit
		i2c_Delay();
	}
}

// CPU从I2C总线设备读取8bit数据
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	// 读到第1个bit为数据的bit7
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		
		if (I2C_SDA_READ())
			++value;

		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

// CPU产生一个时钟，并读取器件的ACK应答信号
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	// CPU释放SDA总线
	i2c_Delay();
	
	I2C_SCL_1();	// CPU驱动SCL = 1, 此时器件会返回ACK应答
	i2c_Delay();
	
	re = I2C_SDA_READ();	// CPU读取SDA口线状态
	
	I2C_SCL_0();
	i2c_Delay();
	
	return re;		// 返回0表示正确应答，1表示无器件响应
}


// CPU产生一个ACK信号
void i2c_Ack(void)
{
	I2C_SDA_0();	// CPU驱动SDA = 0
	i2c_Delay();
	
	I2C_SCL_1();	// CPU产生1个时钟
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	
	I2C_SDA_1();	// CPU释放SDA总线
}


// CPU产生1个NACK信号
void i2c_NAck(void)
{
	I2C_SDA_1();	// CPU驱动SDA = 1
	i2c_Delay();
	
	I2C_SCL_1();	// CPU产生1个时钟
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

// 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
uint8_t i2c_ee_CheckDevice()
{
	uint8_t success;
	
	i2c_Config();	// 配置GPIO
	
	i2c_Start();	// 发送启动信号 

	// 发送设备地址+读写控制bit（0 = w，1 = r) bit7 先传
	i2c_SendByte(EEPROM_ADDRESS | EEPROM_I2C_WR);
	
	success = !i2c_WaitAck();	// 检测设备的ACK应答

	i2c_Stop();		// 发送停止信号

	return success;	// 返回0表示未探测到
}

uint8_t i2c_ee_ByteWrite(uint8_t c, uint8_t addr)
{
	// 第0步：发停止信号，启动内部写操作
	i2c_Stop();
	
	// 第1步：发起I2C总线启动信号
	i2c_Start();
	
	// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
	i2c_SendByte(EEPROM_ADDRESS | EEPROM_I2C_WR);	// 此处是写指令
	
	// 第3步：发送一个时钟，判断器件是否正确应答
	if (i2c_WaitAck() == 1)
	{
		// EEPROM器件无应答，发送I2C总线停止信号
		i2c_Stop();
		return 0;
	}
	
	// 第4步
	i2c_SendByte(addr);
	
	// 第5步：等待ACK
	if (i2c_WaitAck() == 1)
	{
		// EEPROM器件无应答，发送I2C总线停止信号
		i2c_Stop();
		return 0;
	}
	// 第6步：开始写入数据
	i2c_SendByte(c);

	// 第7步：发送ACK
	if (i2c_WaitAck() == 1)
	{
		// EEPROM器件无应答，发送I2C总线停止信号
		i2c_Stop();
		return 0;
	}

	// 命令执行成功，发送I2C总线停止信号
	i2c_Stop();
	
	// write cycle time (t_WR) max 5ms
	delay_ms(5);
	return 1;
}

// 从串行EEPROM指定地址处开始读取若干数据
uint8_t i2c_ee_ReadBytes(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	// 采用串行EEPROM随即读取指令序列，连续读取若干字节
	// 第1步：发起I2C总线启动信号
	i2c_Start();
	
	// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
	i2c_SendByte(EEPROM_ADDRESS | EEPROM_I2C_WR);	// 此处是写指令
	 
	// 第3步：等待ACK
	if (i2c_WaitAck() == 1)
	{
		// EEPROM器件无应答，发送I2C总线停止信号
		i2c_Stop();
		return 0;
	}

	// 第4步：发送字节地址
	i2c_SendByte(ReadAddr);
	
	// 第5步：等待ACK
	if (i2c_WaitAck() == 1)
	{
		// EEPROM器件无应答，发送I2C总线停止信号
		i2c_Stop();
		return 0;
	}
	
	// 第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据
	i2c_Start();
	
	// 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
	i2c_SendByte(EEPROM_ADDRESS | EEPROM_I2C_RD);	// 此处是读指令
	
	// 第8步：发送ACK
	if (i2c_WaitAck() == 1)
	{
		// EEPROM器件无应答，发送I2C总线停止信号
		i2c_Stop();
		return 0;
	}	
	
	// 第9步：循环读取数据
	while (NumByteToRead)
	{		
		*pBuffer = i2c_ReadByte();	// 读1个字节
		
		pBuffer++; 
		
		--NumByteToRead;

		if(NumByteToRead == 0)
		{
			// 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1)
			i2c_NAck();

			// 发送I2C总线停止信号
			i2c_Stop();
			
			break;
		}
		else
		{
			i2c_Ack();	// 中间字节读完后，CPU产生ACK信号(驱动SDA = 0)
		}
	}
	
	// Enable Acknowledgement to be ready for another reception
	i2c_Ack();
	return 1;
}

// 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
uint8_t i2c_ee_WriteBytes(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
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
			i2c_Stop();
			
			// write cycle time (t_WR) max 5ms
			delay_ms(5);
			
			// 第1步：发起I2C总线启动信号
			i2c_Start();
			
			// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
			i2c_SendByte(EEPROM_ADDRESS | EEPROM_I2C_WR);	// 此处是写指令
			
			// 第3步：发送一个时钟，判断器件是否正确应答
			if (i2c_WaitAck() == 1)
			{
				// EEPROM器件无应答，发送I2C总线停止信号
				i2c_Stop();
				return 0;
			}
			
			// 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了
			i2c_SendByte(addr);

			// 第5步：等待ACK
			if (i2c_WaitAck() == 1)
			{
				// EEPROM器件无应答，发送I2C总线停止信号
				i2c_Stop();
				return 0;
			}
		}
	
		// 第6步：开始写入数据
		i2c_SendByte(pBuffer[i]);
	
		// 第7步：发送ACK
		if (i2c_WaitAck() == 1)
		{
			// EEPROM器件无应答，发送I2C总线停止信号
			i2c_Stop();
			return 0;
		}

		addr++;	// 地址增1
	}
	
	// 命令执行成功，发送I2C总线停止信号
	i2c_Stop();
	
	// write cycle time (t_WR) max 5ms
	delay_ms(5);
	return 1;
}

#include <stdio.h>
void i2c_ee_example(void)
{
	#define BufferSize		20
	uint8_t Tx_Buffer[BufferSize] = "Hello,EveryOne~!";
	uint8_t Rx_Buffer[BufferSize] = {0};

	if (!i2c_ee_CheckDevice())
	{
		printf("Cannot find specified EEPROM!\n");
		return;
	}

	// 将发送缓冲区的数据写到flash中，写一页，一页的大小为BufferSize个字节
	if (i2c_ee_WriteBytes(Tx_Buffer, 0x50, BufferSize-1))
	{
		// 将刚刚写入的数据读出来放到接收缓冲区中
		i2c_ee_ReadBytes(Rx_Buffer, 0x50, BufferSize-1);

		// 检查写入的数据与读出的数据是否相等
		printf("DATA=%s\n", Rx_Buffer);
	}
	else
	{
		printf("WRITE_FAILED\n");
	}
}

