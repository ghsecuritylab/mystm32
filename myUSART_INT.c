#include "mylib.h"

#define USART_G_ClockCmd	RCC_APB2PeriphClockCmd
#define USART_G 			RCC_APB2Periph_GPIOA

#define USART_ClockCmd		RCC_APB1PeriphClockCmd
#define USART_				RCC_APB2Periph_USART1

#define USART_TXD			GPIOA
#define USART_TXD_Pin		GPIO_Pin_9
#define USART_RXD			GPIOA
#define USART_RXD_Pin		GPIO_Pin_10

#define USARTX				USART1
#define USARTX_IRQn			USART1_IRQn
#define USARTX_IRQHandler	USART1_IRQHandler

void USART_INT_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	USART_G_ClockCmd(USART_G, ENABLE);
	USART_ClockCmd(USART_, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_AF_PP;	// 必须用复用
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = USART_RXD_Pin;
	GPIO_Init(USART_RXD, &gpio_init_t);
	
	gpio_init_t.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_init_t.GPIO_Pin = USART_TXD_Pin;
	GPIO_Init(USART_TXD, &gpio_init_t);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef nvic_init_t;
	nvic_init_t.NVIC_IRQChannel = USARTX_IRQn;
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init_t.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvic_init_t);
	
	USART_InitTypeDef usart_init_t;
	usart_init_t.USART_BaudRate = 9600;
	usart_init_t.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart_init_t.USART_Parity = USART_Parity_No;
	usart_init_t.USART_StopBits = USART_StopBits_1;
	usart_init_t.USART_WordLength = USART_WordLength_8b;
	usart_init_t.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USARTX, &usart_init_t);
	
	USART_ITConfig(USARTX, USART_IT_RXNE, ENABLE);
	USART_Cmd(USARTX, ENABLE);
	
//	while(1);
}
void USARTX_IRQHandler()
{
	if (USART_GetITStatus(USARTX, USART_IT_RXNE) != RESET)
	{
		uint8_t c = USART_ReceiveData(USARTX);
		USART_SendData(USARTX, c);
		while(USART_GetFlagStatus(USARTX, USART_FLAG_TXE) == RESET);
	}
}
