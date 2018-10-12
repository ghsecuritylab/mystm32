#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t translate[] = 
{
	1,		2,		3,		'U',
	4,		5,		6,		'D',
	7,		8,		9,		'!',
	'.',	0,		'=',	'B'
};
void NRF24L01_EXAMPLE(void);
void STEPPER_MOTOR_EXAMPLE(void);
int main()
{
	//USART_CONFIG();
	while(1)
	{
		//getchar();
		//printf("START!!\r\n");
//		TIM_PWMOUT_CONFIG();
//		
//		for(int8_t i = 0; i < 6; i++) {
//			duty_cycle(i);
//			delay_ms(100);
//		}
//		for(int8_t i = 6-1; i >= 0; i--) {
//			duty_cycle(i);
//			delay_ms(100);
//		}
//		NRF24L01_EXAMPLE();
		STEPPER_MOTOR_EXAMPLE();
		//printf("OVER!!\r\n");
		while(1){}
	}
	return 0;
}
