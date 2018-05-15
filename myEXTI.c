#include "mylib.h"
#define EXTI_G_ClockCmd 	RCC_APB2PeriphClockCmd
#define EXTI_G_ 			RCC_APB2Periph_GPIOB

#define GPIO_IN			GPIOB
#define GPIO_IN_Pin		GPIO_Pin_8
#define GPIO_OUT		GPIOB
#define GPIO_OUT_Pin	GPIO_Pin_6

#define EXTI_LINEX		EXTI_Line0
#define EXTIX_Handler	EXTI0_Handler
#define EXTI_IN			GPIO_PortSourceGPIOB
#define EXTI_IN_Pin		GPIO_PinSource8

void EXTI_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	EXTI_G_ClockCmd(EXTI_G_, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = GPIO_OUT_Pin;
	GPIO_Init(GPIO_OUT, &gpio_init_t);
	
	GPIO_SetBits(GPIO_OUT, GPIO_OUT_Pin);
	
	gpio_init_t.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_init_t.GPIO_Pin = GPIO_IN_Pin;
	GPIO_Init(GPIO_IN, &gpio_init_t);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitTypeDef nvic_init_t;
	nvic_init_t.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init_t.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvic_init_t);
	
	EXTI_InitTypeDef exti_init_t;
	GPIO_EXTILineConfig(EXTI_IN, EXTI_IN_Pin);
	exti_init_t.EXTI_Line = EXTI_LINEX;
	exti_init_t.EXTI_LineCmd = ENABLE;
	exti_init_t.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_t.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti_init_t);
}

void EXTIX_Handler()
{
	if (EXTI_GetITStatus(EXTI_LINEX) != RESET)
	{
		GPIO_OUT->ODR ^= GPIO_OUT_Pin;
		EXTI_ClearITPendingBit(EXTI_LINEX);
	}
}
