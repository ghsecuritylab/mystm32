#include "mylib.h"

#define STEPPER_MOTOR_APBxClkCmd	RCC_APB2PeriphClockCmd
#define STEPPER_MOTOR_CLK			RCC_APB2Periph_GPIOB

#define STEPPER_MOTOR_PORT		GPIOB
#define STEPPER_MOTOR_PHASE_A	GPIO_Pin_5
#define STEPPER_MOTOR_PHASE_B	GPIO_Pin_6
#define STEPPER_MOTOR_PHASE_C	GPIO_Pin_7
#define STEPPER_MOTOR_PHASE_D	GPIO_Pin_8

#define STEPPER_MOTOR_PAHSE		(STEPPER_MOTOR_PHASE_A | \
								 STEPPER_MOTOR_PHASE_B | \
								 STEPPER_MOTOR_PHASE_C | \
								 STEPPER_MOTOR_PHASE_D)

#define RESET_ALL_PHASE()		GPIO_ResetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PAHSE)

void rotateClockWise(uint8_t speed, uint8_t times)
{
	if (speed > 20) speed=20;
	speed = 20 - speed;
	for (uint8_t i=0; i<times; ++i)
	{
		// 4 phase 8 steps -> D > DC > C > CB > B > BA > A > AD

		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_D);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_D | STEPPER_MOTOR_PHASE_C);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_C);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_C | STEPPER_MOTOR_PHASE_B);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_B);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_B | STEPPER_MOTOR_PHASE_A);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_A);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_A | STEPPER_MOTOR_PHASE_D);
		delay_ms(speed);
	}
}

void rotateAntiClockWise(uint8_t speed, uint8_t times)
{
	if (speed > 20) speed=20;
	speed = 20 - speed;
	for (uint8_t i=0; i<times; ++i)
	{
		// 4 phase 8 steps -> A > AB > B > BC > C > CD > D > DA
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_A);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_A | STEPPER_MOTOR_PHASE_B);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_B);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_B | STEPPER_MOTOR_PHASE_C);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_C);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_C | STEPPER_MOTOR_PHASE_D);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_D);
		delay_ms(speed);
		
		RESET_ALL_PHASE();
		GPIO_SetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PHASE_D | STEPPER_MOTOR_PHASE_A);
		delay_ms(speed);
	}
}

void rotateStop(void)
{
	RESET_ALL_PHASE();
}

void STEPPER_MOTOR_EXAMPLE(void)
{
	// initialize GPIO Pins
	STEPPER_MOTOR_APBxClkCmd(STEPPER_MOTOR_CLK, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = STEPPER_MOTOR_PAHSE;
	GPIO_Init(STEPPER_MOTOR_PORT, &gpio_init_t);
	
	delay_ms(50);	// wait for system to be steady
	while(1)
	{
		rotateClockWise(15, 10);	// rotate clockwise 10 times
		rotateClockWise(13, 10);	// speed gradually decline
		rotateClockWise(10, 10);
		rotateClockWise(6, 10);
		rotateClockWise(1, 10);
		rotateStop();
		delay_ms(500);
		
		rotateAntiClockWise(15, 10);	// rotate anticlockwise 10 times
		rotateAntiClockWise(13, 10);
		rotateAntiClockWise(10, 10);
		rotateAntiClockWise(6, 10);
		rotateAntiClockWise(1, 10);
		rotateStop();
		delay_ms(500);
	}
}
