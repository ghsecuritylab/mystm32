#include "mylib.h"
#define OLED_ADDRESS	0x78 // 通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78

#include "mySPI_OLED.h"

#define SPI_G_ClockCmd	RCC_APB2PeriphClockCmd
#define SPI_G_ 			RCC_APB2Periph_GPIOA
#define SPI_ClockCmd	RCC_APB2PeriphClockCmd
#define SPI_			RCC_APB2Periph_SPI1

#define SPIX			SPI1
#define GPIOX			GPIOA
#define GPIO_DC			GPIO_Pin_3
#define GPIO_RST		GPIO_Pin_4
#define GPIO_SCK		GPIO_Pin_5
//#define GPIO_MISO		GPIO_Pin_6
#define GPIO_MOSI		GPIO_Pin_7

void SPI_OLED_CONFIG()
{
	SPI_G_ClockCmd(SPI_G_, ENABLE);
	SPI_ClockCmd(SPI_, ENABLE);

	// CS/SS 普通即可
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;

	gpio_init_t.GPIO_Pin = GPIO_DC;		// D/C
	GPIO_Init(GPIOX, &gpio_init_t);

	gpio_init_t.GPIO_Pin = GPIO_RST;	// RST
	GPIO_Init(GPIOX, &gpio_init_t);
	
	// SPI 引脚复用推挽
	gpio_init_t.GPIO_Mode = GPIO_Mode_AF_PP;

	gpio_init_t.GPIO_Pin = GPIO_SCK;	// SCK
	GPIO_Init(GPIOX, &gpio_init_t);
//	gpio_init_t.GPIO_Pin = GPIO_MISO;	// MISO
//	GPIO_Init(GPIOX, &gpio_init_t);
	gpio_init_t.GPIO_Pin = GPIO_MOSI;	// MOSI
	GPIO_Init(GPIOX, &gpio_init_t);

	SPI_InitTypeDef spi_init_t;
	spi_init_t.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;// 4分频
	spi_init_t.SPI_CPHA = SPI_CPHA_2Edge;
	spi_init_t.SPI_CPOL = SPI_CPOL_High;		// SPI 模式3
	spi_init_t.SPI_CRCPolynomial = 7;			// CRC校验用 SPI_CRC_Rx
	spi_init_t.SPI_DataSize = SPI_DataSize_8b;
	spi_init_t.SPI_FirstBit = SPI_FirstBit_MSB;
	spi_init_t.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	// 全双工
	spi_init_t.SPI_Mode = SPI_Mode_Master;
	spi_init_t.SPI_NSS = SPI_NSS_Soft;			// 软件NSS

	SPI_Init(SPIX, &spi_init_t);
	SPI_Cmd(SPIX, ENABLE);
}

//uint32_t SPI_WriteByte(uint8_t c
void SPI_WriteByte(uint8_t c)
{
	uint32_t timeout = 0x1000;
	while (!SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE))
		if (timeout-- == 0) return;// 0;
	
	SPI_I2S_SendData(SPIX, c);
	
//	timeout = 0x1000;
//	while (!SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE))
//		if (timeout-- == 0) return 0;
//	
//	return SPI_I2S_ReceiveData(SPIX);
//}

//uint8_t SPI_ReadByte()
//{
//	return SPI_WriteByte(0xFF);
}

// WriteCmd 向 OLED 写入命令
// I2C OLED 器件地址为 0x78，命令寄存器地址为 0x00
// SPI OLED 的 CS 已经接地，当 D/C 为低时传输命令
#define WriteCmd(cmd)	{GPIO_ResetBits(GPIOX, GPIO_DC);SPI_WriteByte(cmd);}

// WriteDat 向 OLED 写入数据
// I2C OLED 器件地址为 0x78，命令寄存器地址为 0x40
// SPI OLED 的 CS 已经接地，当 D/C 为高时传输数据，
#define WriteDat(data)	{GPIO_SetBits(GPIOX, GPIO_DC);SPI_WriteByte(data);}

// OLED_Init，初始化OLED
void OLED_Init(void)
{
	GPIO_SetBits(GPIOX, GPIO_RST);
	delay_ms(1);
	// 启动内部初始化
	GPIO_ResetBits(GPIOX, GPIO_RST);
	delay_ms(10);
	// 开始自定义初始化
	GPIO_SetBits(GPIOX, GPIO_RST);
	delay_ms(100);	// 这里的延时很重要
	
	WriteCmd(0xAE); // display off
	WriteCmd(0x20);	// Set Memory Addressing Mode	
	WriteCmd(0x10);	// 00,Horizontal Addressing Mode;
					// 01,Vertical Addressing Mode;
					// 10,Page Addressing Mode (RESET);
					// 11,Invalid
	WriteCmd(0xb0);	// Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	// Set COM Output Scan Direction
	WriteCmd(0x00); // set low column address
	WriteCmd(0x10); // set high column address
	WriteCmd(0x40); // set start line address
	WriteCmd(0x81); // set contrast control register
	WriteCmd(0xff); // set brightness 0x00~0xff
	WriteCmd(0xa1); // set segment re-map 0 to 127
	WriteCmd(0xa6); // set normal display
	WriteCmd(0xa8); // set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); // set display offset
	WriteCmd(0x00); // not offset
	WriteCmd(0xd5); // set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); // set divide ratio
	WriteCmd(0xd9); // set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); // set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); // set vcomh
	WriteCmd(0x20); // 0x20,0.77xVcc
	WriteCmd(0x8d); // set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); // turn on oled panel
}

// 设置光标
void OLED_SetPos(uint8_t x, uint8_t y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}


// 填充整个屏幕
void OLED_Fill(uint8_t fill_Data)
{
	uint8_t m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);	//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
		{
			WriteDat(fill_Data);
		}
	}
}

// 清屏
#define OLED_CLS()	OLED_Fill(0x00)

// 将OLED从休眠中唤醒
void OLED_ON(void)
{
	WriteCmd(0X8D);  // 设置电荷泵
	WriteCmd(0X14);  // 开启电荷泵
	WriteCmd(0XAF);  // OLED唤醒
}

// 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
void OLED_OFF(void)
{
	WriteCmd(0X8D);  // 设置电荷泵
	WriteCmd(0X10);  // 关闭电荷泵
	WriteCmd(0XAE);  // OLED休眠
}

// 显示ASCII字符，TextSize 字符大小，1为6*8，2为8*16
void OLED_ShowStr(uint8_t x, uint8_t y, const uint8_t str[], uint8_t TextSize)
{
	uint8_t c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(str[j] != '\0')
			{
				c = str[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}
		break;
		case 2:
		{
			while(str[j] != '\0')
			{
				c = str[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}
		break;
	}
}

// 显示中的汉字,16*16点阵,N:汉字的索引
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N)
{
	uint8_t wm=0;
	uint16_t adder = 32*N;
	
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	
	OLED_SetPos(x,y + 1);	
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}


// 显示BMP位图。注意 x1 < 128，y1 < 8。因为一次性写入8个列像素，8*8=64
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[])
{
	uint16_t j=0;
	uint8_t x,y;

	if(y1%8==0)
		y = y1/8;
	else
		y = y1/8 + 1;
	
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
		for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}

void OLED_EXAMPLE(void)
{
	uint8_t i;
	
	SPI_OLED_CONFIG();
	
	OLED_Init();
	
	OLED_Fill(0xFF);	// 全屏点亮
	delay_ms(2000);
	
	OLED_Fill(0x00);	// 全屏灭
	delay_ms(2000);
	
	for(i=0;i<4;i++)
	{
		OLED_ShowCN(22+i*16,0,i);		// 测试显示中文
	}
	delay_ms(2000);
	
	OLED_ShowStr(0,3,(uint8_t*)"Wildfire Tech",1);	// 测试6*8字符
	
	OLED_ShowStr(0,4,(uint8_t*)"Hello wildfire",2);	// 测试8*16字符

	delay_ms(10000);
	
	OLED_CLS();	// 清屏
	
	OLED_OFF();	// 测试OLED休眠
	
	delay_ms(2000);
	
	OLED_ON();	// 测试OLED休眠后唤醒
	
	OLED_DrawBMP(0, 0, 128, 8, BMP1);	// 测试BMP位图显示（128x64）
	
	delay_ms(10000);
}
