#include "mylib.h"

#define KEYBOARD_WClockCmd 	RCC_APB2PeriphClockCmd
#define KEYBOARD_W 			RCC_APB2Periph_GPIOB
#define KEYBOARD_DClockCmd 	RCC_APB2PeriphClockCmd
#define KEYBOARD_D 			RCC_APB2Periph_GPIOB
#define GPIO_D		GPIOB
#define GPIO_D_Pin0	GPIO_Pin_6
#define GPIO_D_Pin1	GPIO_Pin_7
#define GPIO_D_Pin2	GPIO_Pin_8
#define GPIO_D_Pin3	GPIO_Pin_9
#define GPIO_W		GPIOB
#define GPIO_W_Pin0	GPIO_Pin_12
#define GPIO_W_Pin1	GPIO_Pin_13
#define GPIO_W_Pin2	GPIO_Pin_14
#define GPIO_W_Pin3	GPIO_Pin_15

void KEYBOARD_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	KEYBOARD_WClockCmd(KEYBOARD_W, ENABLE);
	KEYBOARD_DClockCmd(KEYBOARD_D, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;	
	gpio_init_t.GPIO_Mode = GPIO_Mode_IPD;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = GPIO_D_Pin0|GPIO_D_Pin1|GPIO_D_Pin2|GPIO_D_Pin3;
	GPIO_Init(GPIO_D, &gpio_init_t);
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Pin = GPIO_W_Pin0|GPIO_W_Pin1|GPIO_W_Pin2|GPIO_W_Pin3;
	GPIO_Init(GPIO_W, &gpio_init_t);
}

uint16_t key_table[] = {GPIO_W_Pin0,GPIO_W_Pin1,GPIO_W_Pin2,GPIO_W_Pin3};
#define N (sizeof(key_table)/sizeof(uint16_t))
uint8_t _getKey(uint8_t block)
{
	GPIO_SetBits(GPIO_W, GPIO_W_Pin0|GPIO_W_Pin1|GPIO_W_Pin2|GPIO_W_Pin3);
	switch (GPIO_ReadInputData(GPIO_D)&(GPIO_D_Pin0|GPIO_D_Pin1|GPIO_D_Pin2|GPIO_D_Pin3))
	{
		case GPIO_D_Pin0:
			delay_ms(10);
			if (GPIO_D->IDR & GPIO_D_Pin0)
			{
				for (uint8_t i=0; i < N; i++)	// the top-left key's id is zero
				{
					GPIO_ResetBits(GPIO_W, key_table[i]);
					if (!(GPIO_D->IDR & GPIO_D_Pin0))
					{
						GPIO_SetBits(GPIO_W, key_table[i]);
						if (block) while (GPIO_D->IDR & GPIO_D_Pin0);	// wait for user release the key
						return i;
					}
				}
			}
			return 0xFF;	// joggling
		case GPIO_D_Pin1:
			delay_ms(10);
			if (GPIO_D->IDR & GPIO_D_Pin1)
			{
				for (uint8_t i=0; i < N; i++)
				{
					GPIO_ResetBits(GPIO_W, key_table[i]);
					if (!(GPIO_D->IDR & GPIO_D_Pin1))
					{
						GPIO_SetBits(GPIO_W, key_table[i]);
						if (block) while (GPIO_D->IDR & GPIO_D_Pin1);
						return i+N;
					}
				}
			}
			return 0xFF;
		case GPIO_D_Pin2:
			delay_ms(10);
			if (GPIO_D->IDR & GPIO_D_Pin2)
			{
				for (uint8_t i=0; i < N; i++)
				{
					GPIO_ResetBits(GPIO_W, key_table[i]);
					if (!(GPIO_D->IDR & GPIO_D_Pin2))
					{
						GPIO_SetBits(GPIO_W, key_table[i]);
						if (block) while (GPIO_D->IDR & GPIO_D_Pin2);
						return i+2*N;
					}
				}
			}
			return 0xFF;
		case GPIO_D_Pin3:
			delay_ms(10);
			if (GPIO_D->IDR & GPIO_D_Pin3)
			{
				for (uint8_t i=0; i < N; i++)
				{
					GPIO_ResetBits(GPIO_W, key_table[i]);
					if (!(GPIO_D->IDR & GPIO_D_Pin3))
					{
						GPIO_SetBits(GPIO_W, key_table[i]);
						if (block) while (GPIO_D->IDR & GPIO_D_Pin3);
						return i+3*N;
					}
				}
			}
			return 0xFF;
	}
	return 0xFF;	// no key pressed
}

uint8_t getKey()
{
	return _getKey(1);
}

uint8_t getKey_nonBlock()
{
	return _getKey(0);
}
