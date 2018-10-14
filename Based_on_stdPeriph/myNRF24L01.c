#include "mylib.h"

// CS(NSS)引脚 片选选普通GPIO即可
#define NRF24L01_SPI_CS_APBxClkCmd		RCC_APB2PeriphClockCmd
#define NRF24L01_SPI_CS_CLK				RCC_APB2Periph_GPIOC
#define	NRF24L01_SPI_CS_PORT			GPIOC
#define NRF24L01_SPI_CS_PIN				GPIO_Pin_0
// SCK引脚
#define NRF24L01_SPI_SCK_APBxClkCmd		RCC_APB2PeriphClockCmd
#define NRF24L01_SPI_SCK_CLK			RCC_APB2Periph_GPIOA
#define NRF24L01_SPI_SCK_PORT			GPIOA
#define NRF24L01_SPI_SCK_PIN			GPIO_Pin_5
// MISO引脚
#define NRF24L01_SPI_MISO_APBxClkCmd	RCC_APB2PeriphClockCmd
#define NRF24L01_SPI_MISO_CLK			RCC_APB2Periph_GPIOA
#define	NRF24L01_SPI_MISO_PORT			GPIOA
#define NRF24L01_SPI_MISO_PIN			GPIO_Pin_6
// MOSI引脚
#define NRF24L01_SPI_MOSI_APBxClkCmd	RCC_APB2PeriphClockCmd
#define NRF24L01_SPI_MOSI_CLK			RCC_APB2Periph_GPIOA
#define	NRF24L01_SPI_MOSI_PORT			GPIOA
#define NRF24L01_SPI_MOSI_PIN			GPIO_Pin_7
// SPI
#define NRF24L01_SPIx					SPI1
#define NRF24L01_SPI_APBxClkCmd			RCC_APB2PeriphClockCmd
#define NRF24L01_SPI_CLK				RCC_APB2Periph_SPI1

#define SPI_NRF24L01_CS_LOW()		GPIO_ResetBits(NRF24L01_SPI_CS_PORT, NRF24L01_SPI_CS_PIN)
#define SPI_NRF24L01_CS_HIGH()		GPIO_SetBits(NRF24L01_SPI_CS_PORT, NRF24L01_SPI_CS_PIN)

void SPI_NRF24L01_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能SPI时钟
	NRF24L01_SPI_APBxClkCmd(NRF24L01_SPI_CLK, ENABLE);
	
	// 使能SPI引脚相关的时钟
	NRF24L01_SPI_CS_APBxClkCmd(NRF24L01_SPI_CS_CLK, ENABLE);
	NRF24L01_SPI_SCK_APBxClkCmd(NRF24L01_SPI_SCK_CLK, ENABLE);
	NRF24L01_SPI_MISO_APBxClkCmd(NRF24L01_SPI_MISO_PIN, ENABLE);
	NRF24L01_SPI_MOSI_APBxClkCmd(NRF24L01_SPI_MOSI_PIN, ENABLE);
	
	// 配置SPI的CS引脚，普通IO即可
	GPIO_InitStructure.GPIO_Pin = NRF24L01_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(NRF24L01_SPI_CS_PORT, &GPIO_InitStructure);

	// 停止信号 NRF24L01: CS引脚高电平
	SPI_NRF24L01_CS_HIGH();
	
	// 配置SPI的 SCK引脚
	GPIO_InitStructure.GPIO_Pin = NRF24L01_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(NRF24L01_SPI_SCK_PORT, &GPIO_InitStructure);

	// 配置SPI的 MISO引脚
	GPIO_InitStructure.GPIO_Pin = NRF24L01_SPI_MISO_PIN;
	GPIO_Init(NRF24L01_SPI_MISO_PORT, &GPIO_InitStructure);

	// 配置SPI的 MOSI引脚
	GPIO_InitStructure.GPIO_Pin = NRF24L01_SPI_MOSI_PIN;
	GPIO_Init(NRF24L01_SPI_MOSI_PORT, &GPIO_InitStructure);

	// SPI 模式配置（对24L01要设置 CPHA=0;CPOL=0;）
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		// CPOL
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	// CPHA
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;		// NRF24L01 并不需要 CRC
	SPI_Init(NRF24L01_SPIx , &SPI_InitStructure);
	
	// 使能 SPI  
	SPI_Cmd(NRF24L01_SPIx , ENABLE);
}

// 使用SPI发送一个字节数据，返回接收到的数据
uint8_t SPI_NRF24L01_SendByte(uint8_t c)
{
	uint32_t SPITimeout = 0x1000;

	// 等待发送缓冲区为空，TXE事件
	while (SPI_I2S_GetFlagStatus(NRF24L01_SPIx , SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0)
			return 0xFF;
	}
	
	// 写入数据寄存器，把要写入的数据写入发送缓冲区 
	SPI_I2S_SendData(NRF24L01_SPIx, c);

	SPITimeout = 0x1000;
	
	// 等待接收缓冲区非空，RXNE事件 
	while (SPI_I2S_GetFlagStatus(NRF24L01_SPIx , SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0)
			return 0xFF;
	}
	
	// 读取数据寄存器，获取接收缓冲区数据
	return SPI_I2S_ReceiveData(NRF24L01_SPIx);
}


// 寄存器地址
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address 
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address 
#define SETUP_AW        0x03  // 'Setup address width' register address 
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address 
#define RF_CH           0x05  // 'RF channel' register address 
#define RF_SETUP        0x06  // 'RF setup' register address 
#define STATUS          0x07  // 'Status' register address 
#define OBSERVE_TX      0x08  // 'Observe TX' register address 
#define CD              0x09  // 'Carrier Detect' register address 
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address 
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address 
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address 
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address 
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address 
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address 
#define TX_ADDR         0x10  // 'TX address' register address 
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address 
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address 
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address 
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address 
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address 
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address 
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address 

// 寄存器操作命令
#define READ_CONFREG    0x00  // 读配置寄存器，低5位为寄存器地址
#define WRITE_CONFREG   0x20  // 写配置寄存器，低5位为寄存器地址
#define RD_RX_PLOAD     0x61  // 读RX有效数据，1~32字节
#define WR_TX_PLOAD     0xA0  // 写TX有效数据，1~32字节
#define FLUSH_TX        0xE1  // 清除TX FIFO寄存器。发射模式下用
#define FLUSH_RX        0xE2  // 清除RX FIFO寄存器。接收模式下用
#define REUSE_TX_PL     0xE3  // 重新使用上一包数据，CE为高，数据包被不断发送
#define NOP             0xFF  // 空操作，可以用来读状态寄存器

// 状态信息
#define MAX_TX          0x10  // 达到最大发送次数中断
#define TX_OK           0x20  // TX发送完成中断
#define RX_OK           0x40  // 接收到数据中断

// 发送接收数据宽度定义
#define TX_ADR_WIDTH    5     // 5字节的地址宽度
#define RX_ADR_WIDTH    5     // 5字节的地址宽度
#define TX_PLOAD_WIDTH  20    // 20字节的用户数据宽度
#define RX_PLOAD_WIDTH  20    // 20字节的用户数据宽度

uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x2f,0x31,0xdf,0x1a,0x86};
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x2f,0x31,0xdf,0x1a,0x86};

#define NRF24L01_CE_APBxClkCmd			RCC_APB2PeriphClockCmd
#define NRF24L01_CE_CLK					RCC_APB2Periph_GPIOB
#define NRF24L01_CE_PORT				GPIOC
#define NRF24L01_CE_PIN					GPIO_Pin_1

#define NRF24L01_IRQ_APBxClkCmd			RCC_APB2PeriphClockCmd
#define NRF24L01_IRQ_CLK				RCC_APB2Periph_GPIOB
#define NRF24L01_IRQ_PORT				GPIOC
#define NRF24L01_IRQ_PIN				GPIO_Pin_2

#define NRF24L01_CE_LOW()		GPIO_ResetBits(NRF24L01_CE_PORT, NRF24L01_CE_PIN)
#define NRF24L01_CE_HIGH()		GPIO_SetBits(NRF24L01_CE_PORT, NRF24L01_CE_PIN)

uint8_t TxPacket(uint8_t *txbuf);
uint8_t RxPacket(uint8_t *rxbuf);

uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value) 
{ 
   uint8_t status; 
   SPI_NRF24L01_CS_LOW();					// CSN low, init SPI transaction 
   status = SPI_NRF24L01_SendByte(reg);		// select register 
   SPI_NRF24L01_SendByte(value);			// write value to it
   SPI_NRF24L01_CS_HIGH();					// CSN high again 
 
   return(status);				// return nRF24L01 status byte 
}
 
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes) 
{ 
   uint8_t status, byte_ctr;

   SPI_NRF24L01_CS_LOW();					// Set CSN low, init SPI tranaction 
   status = SPI_NRF24L01_SendByte(reg);		// Select register to write to and read status byte 
 
   for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) 
       pBuf[byte_ctr] = SPI_NRF24L01_SendByte(0);	// Perform SPI_RW to read byte from nRF24L01 
 
   SPI_NRF24L01_CS_HIGH();					// Set CSN high again
 
   return(status);				// return nRF24L01 status byte 
}
 
uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status,byte_ctr;

    SPI_NRF24L01_CS_LOW();					// Set CSN low, init SPI tranaction
    status = SPI_NRF24L01_SendByte(reg);	// Select register to write to and read status byte
    delay_us(10);				// Wait register to be ready 
 
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // then write all byte in buffer(*pBuf)
        SPI_NRF24L01_SendByte(*pBuf++);
    SPI_NRF24L01_CS_HIGH();					// Set CSN high again
 
    return(status);				// return nRF24L01 status byte 
}

void TX_Mode(void)
{
    NRF24L01_CE_LOW();
    SPI_Write_Buf(WRITE_CONFREG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH); 
    SPI_Write_Buf(WRITE_CONFREG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
  
    SPI_RW_Reg(WRITE_CONFREG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
    SPI_RW_Reg(WRITE_CONFREG + EN_RXADDR, 0x01);  // Enable Pipe0
    SPI_RW_Reg(WRITE_CONFREG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
    SPI_RW_Reg(WRITE_CONFREG + RF_CH, 40);        // Select RF channel 40
    SPI_RW_Reg(WRITE_CONFREG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
    SPI_RW_Reg(WRITE_CONFREG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
    NRF24L01_CE_HIGH(); 
}


// 发送数据包，发送过程阻塞
uint8_t TxPacket(uint8_t *txbuf)
{
    uint8_t state;
 
    NRF24L01_CE_LOW();
    SPI_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	// 写数据到 TX
    NRF24L01_CE_HIGH();	// 启动发送
 
    while (GPIO_ReadInputDataBit(NRF24L01_IRQ_PORT, NRF24L01_IRQ_PIN) != 0)
    {}	// 等待发送完成
 
    state = SPI_NRF24L01_SendByte(READ_CONFREG + STATUS);	// 读取状态寄存器的值
    SPI_RW_Reg(WRITE_CONFREG + STATUS, state);	// 清除TX_DS或MAX_RT中断标志
 
    if(state & MAX_TX)	// 达到最大重发次数
    {
        SPI_RW_Reg(FLUSH_TX, 0xff);	// 清除TX FIFO寄存器 
        return MAX_TX;
    }
    if(state & TX_OK)	// 发送完成
    {
        return TX_OK;
    }
    return 0xff;	// 其他原因发送失败
}

void RX_Mode(void)
{
    NRF24L01_CE_LOW();
    SPI_Write_Buf(WRITE_CONFREG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);
 
    SPI_RW_Reg(WRITE_CONFREG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
    SPI_RW_Reg(WRITE_CONFREG + EN_RXADDR, 0x01);  // Enable Pipe0
    SPI_RW_Reg(WRITE_CONFREG + RF_CH, 40);        // Select RF channel 40
    SPI_RW_Reg(WRITE_CONFREG + RX_PW_P0, RX_PLOAD_WIDTH);
    SPI_RW_Reg(WRITE_CONFREG + RF_SETUP, 0x07);
    SPI_RW_Reg(WRITE_CONFREG + CONFIG, 0x0f);     // Set PWR_UP bit,
    // enable CRC(2 bytes) & Prim:RX. RX_DR enabled.. 
 
    NRF24L01_CE_HIGH(); // Set CE pin high to enable RX device 
 
    // This device is now ready to receive one packet of 16 bytes payload
    // from a TX device sending to address
    // '3443101001', with auto acknowledgment, retransmit count of 10,
    // RF channel 40 and datarate = 2Mbps. 
}

uint8_t RxPacket(uint8_t *rxbuf)
{
    uint8_t state;
 
    state = SPI_NRF24L01_SendByte(READ_CONFREG + STATUS);	// 读取状态寄存器的值         
    SPI_RW_Reg(WRITE_CONFREG + STATUS, state);	// 清除TX_DS或MAX_RT中断标志

    if(state & RX_OK)	// 接收到数据
    {
        SPI_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);	// 读取数据
        SPI_RW_Reg(FLUSH_RX, 0xff);	// 清除RX FIFO寄存器 
        return 0;
    }
    return 0xff;    // 没收到任何数据
}

//uint8_t TX_PAYLOAD[TX_PLOAD_WIDTH] = "this is a test";
uint8_t RX_BUFFER[RX_PLOAD_WIDTH];
void NRF24L01_EXAMPLE(void)
{
	SPI_NRF24L01_Init();
	NRF24L01_CE_APBxClkCmd(NRF24L01_CE_CLK, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = NRF24L01_CE_PIN;
	GPIO_Init(NRF24L01_CE_PORT, &gpio_init_t);

	RX_Mode();
	RxPacket(RX_BUFFER);
	
	TX_Mode();
	TxPacket(RX_BUFFER);
}
