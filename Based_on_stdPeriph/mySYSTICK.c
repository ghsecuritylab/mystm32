#include "mylib.h"
void delay_us(uint32_t us)
{
	// 开启系统定时器
	SysTick_Config(SystemCoreClock/1000000);
	for(uint32_t i=0;i<us;i++)
	{
		// 当计数器的值减到0时CTRL位16置1
		while(!((SysTick->CTRL)&(1<<16)));
	}
	// 关闭定时器
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
void delay_ms(uint32_t ms)
{
	// 开启系统定时器
	SysTick_Config(SystemCoreClock/1000);
	for(uint32_t i=0;i<ms;i++)
	{
		// 当计数器的值减到0时CTRL位16置1
		while(!((SysTick->CTRL)&(1<<16)));
	}
	// 关闭定时器
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
// 中断处理必须有，否则会卡死
void SysTick_Handler(){}

