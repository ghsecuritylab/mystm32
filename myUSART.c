#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_USART1

// UART1 挂载到 APB2 总线，其他 UART 挂载到 APB1 总线
#ifdef DEBUG_USART1

#define DEBUG_USARTx				USART1
#define DEBUG_USART_CLK				RCC_APB2Periph_USART1
#define DEBUG_USART_APBxClkCmd		RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE		115200

#define DEBUG_USART_GPIO_CLK		(RCC_APB2Periph_GPIOA)
#define DEBUG_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
    
#define DEBUG_USART_TX_GPIO_PORT	GPIOA
#define DEBUG_USART_TX_GPIO_PIN		GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT	GPIOA
#define DEBUG_USART_RX_GPIO_PIN		GPIO_Pin_10

#define DEBUG_USART_IRQ				USART1_IRQn
#define DEBUG_USART_IRQHandler		USART1_IRQHandler

#endif
#ifdef DEBUG_USART2

#define DEBUG_USARTx				USART2
#define DEBUG_USART_CLK				RCC_APB1Periph_USART2
#define DEBUG_USART_APBxClkCmd		RCC_APB1PeriphClockCmd
#define DEBUG_USART_BAUDRATE		115200

#define DEBUG_USART_GPIO_CLK		(RCC_APB2Periph_GPIOA)
#define DEBUG_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
   
#define DEBUG_USART_TX_GPIO_PORT	GPIOA   
#define DEBUG_USART_TX_GPIO_PIN		GPIO_Pin_2
#define DEBUG_USART_RX_GPIO_PORT	GPIOA
#define DEBUG_USART_RX_GPIO_PIN		GPIO_Pin_3

#define DEBUG_USART_IRQ				USART2_IRQn
#define DEBUG_USART_IRQHandler		USART2_IRQHandler

#endif
#ifdef DEBUG_USART3

#define DEBUG_USARTx				USART3
#define DEBUG_USART_CLK				RCC_APB1Periph_USART3
#define DEBUG_USART_APBxClkCmd		RCC_APB1PeriphClockCmd
#define DEBUG_USART_BAUDRATE		115200

#define DEBUG_USART_GPIO_CLK		(RCC_APB2Periph_GPIOB)
#define DEBUG_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
   
#define DEBUG_USART_TX_GPIO_PORT	GPIOB   
#define DEBUG_USART_TX_GPIO_PIN		GPIO_Pin_10
#define DEBUG_USART_RX_GPIO_PORT	GPIOB
#define DEBUG_USART_RX_GPIO_PIN		GPIO_Pin_11

#define DEBUG_USART_IRQ				USART3_IRQn
#define DEBUG_USART_IRQHandler		USART3_IRQHandler

#endif
#ifdef DEBUG_USART4

#define DEBUG_USARTx				UART4
#define DEBUG_USART_CLK				RCC_APB1Periph_UART4
#define DEBUG_USART_APBxClkCmd		RCC_APB1PeriphClockCmd
#define DEBUG_USART_BAUDRATE		115200

#define DEBUG_USART_GPIO_CLK		(RCC_APB2Periph_GPIOC)
#define DEBUG_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd

#define DEBUG_USART_TX_GPIO_PORT	GPIOC   
#define DEBUG_USART_TX_GPIO_PIN		GPIO_Pin_10
#define DEBUG_USART_RX_GPIO_PORT	GPIOC
#define DEBUG_USART_RX_GPIO_PIN		GPIO_Pin_11

#define DEBUG_USART_IRQ				UART4_IRQn
#define DEBUG_USART_IRQHandler		UART4_IRQHandler

#endif
#ifdef DEBUG_USART5

#define DEBUG_USARTx				UART5
#define DEBUG_USART_CLK				RCC_APB1Periph_UART5
#define DEBUG_USART_APBxClkCmd		RCC_APB1PeriphClockCmd
#define DEBUG_USART_BAUDRATE		115200

#define DEBUG_USART_GPIO_CLK		(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)
#define DEBUG_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
   
#define DEBUG_USART_TX_GPIO_PORT	GPIOC   
#define DEBUG_USART_TX_GPIO_PIN		GPIO_Pin_12
#define DEBUG_USART_RX_GPIO_PORT	GPIOD
#define DEBUG_USART_RX_GPIO_PIN		GPIO_Pin_2

#define DEBUG_USART_IRQ				UART5_IRQn
#define DEBUG_USART_IRQHandler		UART5_IRQHandler

#endif

int fputc(int ch, FILE* f)
{
	USART_SendData(DEBUG_USARTx, (uint8_t)ch);
	while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
	return ch;
}
int fgetc(FILE* f)
{
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);
	uint8_t c = USART_ReceiveData(DEBUG_USARTx);
	return (int)c;
}
void USART_CONFIG()
{
	GPIO_InitTypeDef gpio_init_t;
	USART_InitTypeDef usart_init_t;

	// 打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	gpio_init_t.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	gpio_init_t.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &gpio_init_t);

	// 将USART Rx的GPIO配置为浮空输入模式
	gpio_init_t.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	gpio_init_t.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &gpio_init_t);
	
	// 配置串口的工作参数
	// 配置波特率
	usart_init_t.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// 配置帧数据字长
	usart_init_t.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	usart_init_t.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	usart_init_t.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	usart_init_t.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	usart_init_t.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &usart_init_t);

	// 使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);
}
