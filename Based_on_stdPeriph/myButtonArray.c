#include "mylib.h"

// 位选
#define KEYBOARD_W_APBxClkCmd 	RCC_APB2PeriphClockCmd
#define KEYBOARD_W_CLK			RCC_APB2Periph_GPIOB
#define KEYBOARD_W_PORT			GPIOB
#define KEYBOARD_W_PIN0			GPIO_Pin_12
#define KEYBOARD_W_PIN1			GPIO_Pin_13
#define KEYBOARD_W_PIN2			GPIO_Pin_14
#define KEYBOARD_W_PIN3			GPIO_Pin_15

// 段选
#define KEYBOARD_D_APBxClkCmd 	RCC_APB2PeriphClockCmd
#define KEYBOARD_D_CLK			RCC_APB2Periph_GPIOB
#define KEYBOARD_D_PORT			GPIOB
#define KEYBOARD_D_PIN0			GPIO_Pin_8
#define KEYBOARD_D_PIN1			GPIO_Pin_9
#define KEYBOARD_D_PIN2			GPIO_Pin_10
#define KEYBOARD_D_PIN3			GPIO_Pin_11

void KEYBOARD_CONFIG(void)
{
	KEYBOARD_W_APBxClkCmd(KEYBOARD_W_CLK, ENABLE);
	KEYBOARD_D_APBxClkCmd(KEYBOARD_D_CLK, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_IPD;	// 段选：下拉输入（F103ZET6 PB.6 和 PB.7 的下拉输入无效）
	gpio_init_t.GPIO_Pin = KEYBOARD_D_PIN0|KEYBOARD_D_PIN1|KEYBOARD_D_PIN2|KEYBOARD_D_PIN3;
	GPIO_Init(KEYBOARD_D_PORT, &gpio_init_t);
	
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;	// 位选：推挽输出
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = KEYBOARD_W_PIN0|KEYBOARD_W_PIN1|KEYBOARD_W_PIN2|KEYBOARD_W_PIN3;
	GPIO_Init(KEYBOARD_W_PORT, &gpio_init_t);
}

uint16_t w_table[] = {KEYBOARD_W_PIN0, KEYBOARD_W_PIN1, KEYBOARD_W_PIN2, KEYBOARD_W_PIN3};
#define N (sizeof(w_table)/sizeof(uint16_t))
uint8_t getKey(void)
{
	// 位选全部置高
	GPIO_SetBits(KEYBOARD_W_PORT, KEYBOARD_W_PIN0|KEYBOARD_W_PIN1|KEYBOARD_W_PIN2|KEYBOARD_W_PIN3);
	// 读段选，被按下的为高电平
	switch (GPIO_ReadInputData(KEYBOARD_D_PORT)&(KEYBOARD_D_PIN0|KEYBOARD_D_PIN1|KEYBOARD_D_PIN2|KEYBOARD_D_PIN3))
	{
		case KEYBOARD_D_PIN0:
			delay_ms(10);
			// 抖动检测
			if (KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN0)
			{
				// 位选逐个置低
				for (uint8_t i=0; i < N; i++)
				{
					GPIO_ResetBits(KEYBOARD_W_PORT, w_table[i]);
					// 段选上的高电平检测不到了
					if (!(KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN0))
					{
						// 恢复位选
						GPIO_SetBits(KEYBOARD_W_PORT, KEYBOARD_W_PIN0|KEYBOARD_W_PIN1|KEYBOARD_W_PIN2|KEYBOARD_W_PIN3);
						return i;
					}
				}
			}
			return 0xFF;	// joggling
		case KEYBOARD_D_PIN1:
			delay_ms(10);
			if (KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN1)
			{
				for (uint8_t i=0; i < N; i++)
				{
					GPIO_ResetBits(KEYBOARD_W_PORT, w_table[i]);
					if (!(KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN1))
					{
						GPIO_SetBits(KEYBOARD_W_PORT, KEYBOARD_W_PIN0|KEYBOARD_W_PIN1|KEYBOARD_W_PIN2|KEYBOARD_W_PIN3);
						return i+N;
					}
				}
			}
			return 0xFF;
		case KEYBOARD_D_PIN2:
			delay_ms(10);
			if (KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN2)
			{
				for (uint8_t i=0; i < N; i++)
				{
					GPIO_ResetBits(KEYBOARD_W_PORT, w_table[i]);
					if (!(KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN2))
					{
						GPIO_SetBits(KEYBOARD_W_PORT, KEYBOARD_W_PIN0|KEYBOARD_W_PIN1|KEYBOARD_W_PIN2|KEYBOARD_W_PIN3);
						return i+2*N;
					}
				}
			}
			return 0xFF;
		case KEYBOARD_D_PIN3:
			delay_ms(10);
			if (KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN3)
			{
				for (uint8_t i=0; i < N; i++)
				{
					GPIO_ResetBits(KEYBOARD_W_PORT, w_table[i]);
					if (!(KEYBOARD_D_PORT->IDR & KEYBOARD_D_PIN3))
					{
						GPIO_SetBits(KEYBOARD_W_PORT, KEYBOARD_W_PIN0|KEYBOARD_W_PIN1|KEYBOARD_W_PIN2|KEYBOARD_W_PIN3);
						return i+3*N;
					}
				}
			}
			return 0xFF;
	}
	return 0xFF;	// no key pressed
}

uint16_t d_table[] = {KEYBOARD_D_PIN0, KEYBOARD_D_PIN1, KEYBOARD_D_PIN2, KEYBOARD_D_PIN3};
void waitKeyUp(uint8_t keyId)
{	
	// wait for user release the key
	while (KEYBOARD_D_PORT->IDR & d_table[keyId/N]) {}
}
