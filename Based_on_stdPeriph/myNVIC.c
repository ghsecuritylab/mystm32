#include "mylib.h"

// PreemptionPriority:0,1
// SubPriority:0,7
void NVIC_CONFIG(uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority)
{
	// stm32f103 原则上可配置255级优先级，但只使用了中断优先级寄存器NVIC_IPRx的高4位
	// 因此只能配置16级。优先级分组指定这4位中抢占式优先级的位数，设0则4位全部用于子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	// 设置抢占式优先级只有1位
	
	// 优先级数值越小优先级越高，如果优先级设置相同，中断源编号较小的优先级较高
	NVIC_InitTypeDef nvic_init_t;
	nvic_init_t.NVIC_IRQChannel = IRQChannel;
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
	nvic_init_t.NVIC_IRQChannelSubPriority = SubPriority;
	NVIC_Init(&nvic_init_t);
}
