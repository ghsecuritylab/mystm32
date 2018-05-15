#include "mylib.h"
#define EXTI_G_ClockCmd 	RCC_APB2PeriphClockCmd
#define EXTI_G_ 			RCC_APB2Periph_GPIOB

#define GPIO_IN			GPIOB
#define GPIO_IN_Pin		GPIO_Pin_8

#define EXTI_LINEX		EXTI_Line8
// EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn, EXTI9_5_IRQn, EXTI15_10_IRQn
#define EXTIX_IRQn		EXTI9_5_IRQn
#define EXTIX_Handler	EXTI9_5_IRQHandler
#define EXTI_IN			GPIO_PortSourceGPIOB
#define EXTI_IN_Pin		GPIO_PinSource8

void EXTI_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	EXTI_G_ClockCmd(EXTI_G_, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_init_t.GPIO_Pin = GPIO_IN_Pin;
	GPIO_Init(GPIO_IN, &gpio_init_t);
	
	NVIC_CONFIG(EXTIX_IRQn, 2);
	
	EXTI_InitTypeDef exti_init_t;
	GPIO_EXTILineConfig(EXTI_IN, EXTI_IN_Pin);
	exti_init_t.EXTI_Line = EXTI_LINEX;
	exti_init_t.EXTI_LineCmd = ENABLE;
	exti_init_t.EXTI_Mode = EXTI_Mode_Interrupt;	// 设置为中断请求或者 EXTI_Mode_Event事件请求
	exti_init_t.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&exti_init_t);
}

void EXTI_CALLBACK(void);
void EXTIX_Handler()
{
	if (EXTI_GetITStatus(EXTI_LINEX) != RESET)
	{
		//EXTI_CALLBACK();
		EXTI_ClearITPendingBit(EXTI_LINEX);
	}
}
