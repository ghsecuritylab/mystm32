#include "mylib.h"

#define LED1_GPIO_ClkCmd 	RCC_APB2PeriphClockCmd
#define LED1_GPIO_CLK 		RCC_APB2Periph_GPIOC
#define LED1_GPIO			GPIOC
#define LED1_GPIO_PIN		GPIO_Pin_0

#define LED2_GPIO_ClkCmd 	RCC_APB2PeriphClockCmd
#define LED2_GPIO_CLK 		RCC_APB2Periph_GPIOB
#define LED2_GPIO			GPIOB
#define LED2_GPIO_PIN		GPIO_Pin_0

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	LED1_GPIO_ClkCmd(LED1_GPIO_CLK, ENABLE);
//	LED2_GPIO_ClkCmd(LED2_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;	
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = LED1_GPIO_PIN;
	GPIO_Init(LED1_GPIO, &gpio_init_t);
	
//	gpio_init_t.GPIO_Pin = LED2_GPIO_PIN;
//	GPIO_Init(LED2_GPIO, &gpio_init_t);
}

void LED_HIGH(uint8_t LEDn)
{
	switch (LEDn)
	{
		case 1:
			GPIO_SetBits(LED1_GPIO, LED1_GPIO_PIN);
			break;
		case 2:
			GPIO_SetBits(LED2_GPIO, LED2_GPIO_PIN);
			break;
	}
}

void LED_LOW(uint8_t LEDn)
{
	switch (LEDn)
	{
		case 1:
			GPIO_ResetBits(LED1_GPIO, LED1_GPIO_PIN);
			break;
		case 2:
			GPIO_ResetBits(LED2_GPIO, LED2_GPIO_PIN);
			break;
	}
}

void LED_EXAMPLE(void)
{
	LED_Init();
		
	// 闪烁灯
	while (1)
	{
		LED_HIGH(1);
		delay_ms(500);
		LED_LOW(1);
		delay_ms(500);
	}
}

void LED_EXAMPLE2(void)
{
	LED_Init();
	
#define SPEED	30
	
	// 呼吸灯
	uint8_t k=0,increase=1;
	while (1)
	{
		for (uint8_t i=0; i<SPEED; i++)
		{
			LED_HIGH(1);
			delay_us(k);
			
			LED_LOW(1);
			delay_us(255-k);
		}
		
		k = increase ? (k<255 ? k+1 : 255) : (k>0 ? k-1 : 0);
		
		if (k==255 || k==0)
			increase=!increase;
	}
}
