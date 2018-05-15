#include "mylib.h"

// TIM6,TIM7 只有基本定时器功能，只需设置 TIM_Prescaler 和 TIM_Period

#define TIM_ClockCmd		RCC_APB1PeriphClockCmd
#define TIM_				RCC_APB1Periph_TIM2
#define TIMX				TIM2
#define TIMX_IRQn			TIM2_IRQn
#define TIMX_IRQHandler		TIM1_2_IRQHandler

uint32_t time;
void TIM_BASE_CONFIG()
{
	TIM_TimeBaseInitTypeDef time_init_t;
	
	TIM_ClockCmd(TIM_, ENABLE);
	
	// 设置定时器时钟与CK_INT频率与数字滤波器采样时钟频率分频比
	
	time_init_t.TIM_ClockDivision = TIM_CKD_DIV1;
	time_init_t.TIM_CounterMode = TIM_CounterMode_Up;
	time_init_t.TIM_Period = 1000;			// 计数
	time_init_t.TIM_Prescaler = 71;			// 72MHz / (71+1) = 1MHz
	time_init_t.TIM_RepetitionCounter = 0;	// 重复定时器
	TIM_TimeBaseInit(TIMX, &time_init_t);
	
	//TIM_ARRPreloadConfig(TIMX, ENABLE);	// 使能预加载寄存器后，写入ARR的数值在更新事件时才写入影子寄存器；禁用时，直接写入影子寄存器
	
	TIM_ClearFlag(TIMX, TIM_FLAG_Update);		// 清除中断标志位
	TIM_ITConfig(TIMX, TIM_IT_Update, ENABLE);	// TIM中断使能
	
	TIM_Cmd(TIMX, ENABLE);
	TIM_ClockCmd(TIM_, DISABLE);	// 临时关闭时钟
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitTypeDef nvic_init_t;
	nvic_init_t.NVIC_IRQChannel = TIMX_IRQn;
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_t.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_init_t);
	
	TIM_ClockCmd(TIM_, ENABLE);		// 开始计时
	
//	while (1)
//	{
//		if (time == 1000)
//			;// +1s
//	}
}

void TIMX_IRQHandler()
{
	if (TIM_GetITStatus(TIMX, TIM_IT_Update) == SET)
	{
		time++;
		TIM_ClearITPendingBit(TIMX, TIM_FLAG_Update);
	}	
}
