#include "mylib.h"

// TIM6,TIM7 只有基本定时器功能，只需设置 TIM_Prescaler 和 TIM_Period

#define TIM_ClockCmd		RCC_APB1PeriphClockCmd
#define TIM_				RCC_APB1Periph_TIM2
#define TIMX				TIM2
//TIM1_BRK_IRQn,TIM1_UP_IRQn,TIM1_TRG_COM_IRQn,TIM1_CC_IRQn,
//TIM1_BRK_TIM15_IRQn,TIM1_UP_TIM16_IRQn,TIM1_TRG_COM_TIM17_IRQn,
//TIM2_IRQn,TIM3_IRQn,TIM4_IRQn,TIM5_IRQn,TIM6_IRQn,TIM6_DAC_IRQn,TIM7_IRQn,
//TIM8_BRK_IRQn,TIM8_UP_IRQn,TIM8_TRG_COM_IRQn,TIM8_CC_IRQn,
//TIM12_IRQn,TIM13_IRQn,TIM14_IRQn
#define TIMX_IRQn			TIM2_IRQn
#define TIMX_IRQHandler		TIM2_IRQHandler

uint32_t time_10ms;
void TIM_BASE_CONFIG()
{
	TIM_TimeBaseInitTypeDef time_init_t;
	
	TIM_ClockCmd(TIM_, ENABLE);
	
	// 设置定时器时钟与CK_INT频率与数字滤波器采样时钟频率分频比
	
	time_init_t.TIM_ClockDivision = TIM_CKD_DIV1;
	time_init_t.TIM_CounterMode = TIM_CounterMode_Up;
	time_init_t.TIM_Period = 10000;			// 计数
	time_init_t.TIM_Prescaler = 71;			// 72MHz / (71+1) = 1MHz
	time_init_t.TIM_RepetitionCounter = 0;	// 重复定时器
	TIM_TimeBaseInit(TIMX, &time_init_t);
	
	//TIM_ARRPreloadConfig(TIMX, ENABLE);	// 使能预加载寄存器后，写入ARR的数值在更新事件时才写入影子寄存器；禁用时，直接写入影子寄存器
	
	TIM_ClearFlag(TIMX, TIM_FLAG_Update);		// 清除中断标志位
	TIM_ITConfig(TIMX, TIM_IT_Update, ENABLE);	// TIM中断使能
	
	NVIC_CONFIG(TIMX_IRQn, 3);
	
	TIM_Cmd(TIMX, ENABLE);
	
	// 暂停计时器以便需要时开启
	TIM_ClockCmd(TIM_, DISABLE);
	
	// 开始计时
	TIM_ClockCmd(TIM_, ENABLE);
}

#include <stdio.h>
void TIMX_IRQHandler()
{
	if (TIM_GetITStatus(TIMX, TIM_IT_Update) == SET)
	{
		time_10ms++;
		if (time_10ms == 100)
		{
			printf("A_SECONDS_PASSED\n");
			time_10ms = 0;			
		}
		TIM_ClearITPendingBit(TIMX, TIM_FLAG_Update);
	}	
}
