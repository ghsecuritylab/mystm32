#include "mylib.h"
#define SPI_G_ClockCmd	RCC_APB2PeriphClockCmd
#define SPI_G_ 			RCC_APB2Periph_GPIOA
#define SPI_ClockCmd	RCC_APB2PeriphClockCmd
#define SPI_			RCC_APB2Periph_SPI1

#define SPIX			SPI1
#define GPIOX			GPIOA
#define GPIO_CS			GPIO_Pin_4
#define GPIO_SCK		GPIO_Pin_5
#define GPIO_MISO		GPIO_Pin_6
#define GPIO_MOSI		GPIO_Pin_7

void SPI_CONFIG()
{
	SPI_G_ClockCmd(SPI_G_, ENABLE);
	SPI_ClockCmd(SPI_, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;	// 片选，普通即可
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = GPIO_CS;
	GPIO_Init(GPIOX, &gpio_init_t);
	
	GPIO_SetBits(GPIOX, GPIO_CS);
	
	gpio_init_t.GPIO_Mode = GPIO_Mode_AF_PP;// 复用推挽
	gpio_init_t.GPIO_Pin = GPIO_SCK;	// SCK
	GPIO_Init(GPIOX, &gpio_init_t);
	gpio_init_t.GPIO_Pin = GPIO_MISO;	// MISO
	GPIO_Init(GPIOX, &gpio_init_t);
	gpio_init_t.GPIO_Pin = GPIO_MOSI;	// MOSI
	GPIO_Init(GPIOX, &gpio_init_t);
	
	SPI_InitTypeDef spi_init_t;
	spi_init_t.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;// 4分频
	spi_init_t.SPI_CPHA = SPI_CPHA_2Edge;
	spi_init_t.SPI_CPOL = SPI_CPOL_High;		// SPI 模式3
	spi_init_t.SPI_CRCPolynomial = SPI_CRC_Rx;	// CRC校验
	spi_init_t.SPI_DataSize = SPI_DataSize_8b;
	spi_init_t.SPI_FirstBit = SPI_FirstBit_MSB;
	spi_init_t.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	// 全双工
	spi_init_t.SPI_Mode = SPI_Mode_Master;
	spi_init_t.SPI_NSS = SPI_NSS_Soft;			// 软件NSS
	
	SPI_Init(SPIX, &spi_init_t);
	SPI_Cmd(SPIX, ENABLE);
}
uint32_t SPI_WriteByte(uint8_t c)
{
	uint32_t timeout = 0x1000;
	while (!SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE))
		if (timeout-- == 0) return 0;
	
	SPI_I2S_SendData(SPI1, c);
	
	timeout = 0x1000;
	while (!SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE))
		if (timeout-- == 0) return 0;
	
	return SPI_I2S_ReceiveData(SPI1);
}

uint8_t SPI_ReadByte()
{
	return SPI_WriteByte(0xFF);
}
