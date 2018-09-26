#include <mylib.h>

#define STEPPER_MOTOR_CLK			RCC_APB2Periph_GPIOB
#define STEPPER_MOTOR_APBxClock_FUN	RCC_APB2PeriphClockCmd

#define STEPPER_MOTOR_PORT		GPIOC
#define STEPPER_MOTOR_PHASE_A	GPIO_Pin_0
#define STEPPER_MOTOR_PHASE_B	GPIO_Pin_1
#define STEPPER_MOTOR_PHASE_C	GPIO_Pin_2
#define STEPPER_MOTOR_PHASE_D	GPIO_Pin_3

#define STEPPER_MOTOR_PAHSE		(STEPPER_MOTOR_PHASE_A | \
								 STEPPER_MOTOR_PHASE_B | \
								 STEPPER_MOTOR_PHASE_C | \
								 STEPPER_MOTOR_PHASE_D)

#define RESET_ALL_PHASE()		GPIO_ResetBits(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PAHSE)

void rotateClockWise(uint8_t speed, uint8_t times)
{
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
	STEPPER_MOTOR_APBxClock_FUN(STEPPER_MOTOR_CLK, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = STEPPER_MOTOR_PAHSE;
	GPIO_Init(NRF24L01_CSN_PORT, &gpio_init_t);
	
	delay_ms(50);	// wait for system to be steady
	while(1)
	{
		rotateClockWise(25, 10);	// rotate clockwise 10 times
		rotateClockWise(23, 10);
		rotateClockWise(20, 10);
		rotateClockWise(16, 10);
		rotateClockWise(10, 10);
		rotateClockWise(4, 10);
		rotateStop();
		delay_ms(900);
		
		rotateAntiClockWise(25, 10);	// rotate anticlockwise 10 times
		rotateAntiClockWise(23, 10);
		rotateAntiClockWise(20, 10);
		rotateAntiClockWise(16, 10);
		rotateAntiClockWise(10, 10);
		rotateAntiClockWise(4, 10);
		rotateStop();
		delay_ms(900);
	}
}
