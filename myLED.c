#include "mylib.h"

#define LED_G_ClockCmd 	RCC_APB2PeriphClockCmd
#define LED_G_ 			RCC_APB2Periph_GPIOA
#define LED_GPIO		GPIOA
#define LED_GPIO_Pin	GPIO_Pin_13

int LED_EXAMPLE()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	LED_G_ClockCmd(LED_G_, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;	
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = LED_GPIO_Pin;
	GPIO_Init(LED_GPIO, &gpio_init_t);
		
	// 闪烁灯
	while (1)
	{
		GPIO_SetBits(LED_GPIO, LED_GPIO_Pin);
		delay_ms(500);
		GPIO_ResetBits(LED_GPIO, LED_GPIO_Pin);
		delay_ms(500);
	}
	
	// 呼吸灯
	int k=0,flag=1;
	while (1)
	{
		GPIO_SetBits(LED_GPIO, LED_GPIO_Pin);
		for (int i=0;i<k;i++)
			delay_ms(2);
		
		GPIO_ResetBits(LED_GPIO, LED_GPIO_Pin);
		for (int i=255;i>=k;i--)
			delay_ms(2);
		
		k = flag ? (k<255 ? k+1 : 255) : (k>0 ? k-1 : 0);
		
		if (k==255 || k==0)
			flag=!flag;
	}
}
