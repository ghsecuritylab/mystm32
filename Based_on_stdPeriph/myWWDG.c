#include "mylib.h"

// 窗口计数器值，范围为：0x40~0x7f，一般设置成最大0x7F
#define WWDG_CNT	0x7F

/* WWDG 配置函数
 * tr ：递减计时器的值，取值范围为：0x7f~0x40
 * wr ：窗口值，取值范围为：0x7f~0x40
 * prv：预分频器值，
 * 		WWDG_CNT_CLK = PCLK1(36MHz)/4096/(2^WDGTB)
 * 		WWDG_Prescaler_1: WDGTB=0
 * 		WWDG_Prescaler_2: WDGTB=1
 * 		WWDG_Prescaler_4: WDGTB=2
 * 		WWDG_Prescaler_8: WDGTB=3
 */
void WWDG_CONFIG(uint8_t tr, uint8_t wr, uint32_t prv)
{	
	// 开启 WWDG 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	// 设置递减计数器的值
	WWDG_SetCounter( tr );
	
	// 设置预分频器的值
	WWDG_SetPrescaler( prv );
	
	// 设置上窗口值
	WWDG_SetWindowValue( wr );
	
	// 设置计数器的值，使能WWDG
	WWDG_Enable(WWDG_CNT);	
	
	// 清除提前唤醒中断标志位
	WWDG_ClearFlag();
	
	// WWDG 中断优先级初始化
	NVIC_CONFIG(WWDG_IRQn, 0, 0);	
	
	// 开WWDG 中断
	WWDG_EnableIT();
}

// 喂狗，刷新递减计数器的值，设置成最大WDG_CNT=0X7F
#define WWDG_Feed()		WWDG_SetCounter( WWDG_CNT );

// WWDG 中断复服务程序，如果发生了此中断，表示程序已经出现了故障，
// 这是一个死前中断。在此中断服务程序中应该干最重要的事，
// 比如保存重要的数据等，这个时间具体有多长，由WDGTB的值决定：
// WDGTB=0 约 113us
// WDGTB=1 约 227us
// WDGTB=2 约 455us
// WDGTB=3 约 910us
// 计算方法是减一个数的时间 T=1/(WWDG_CNT_CLK)=(2^WDGTB)*4096/PCLK1(36MHz)
void WWDG_IRQHandler(void)
{
	// 清除中断标志位
	WWDG_ClearFlag();
	
	// do sth...
}

void WWDG_EXAMPLE(void)
{
	// 初始化WWDG：配置计数器初始值，配置上窗口值，启动WWDG，使能提前唤醒中断
	WWDG_CONFIG(WWDG_CNT, 0x5F, WWDG_Prescaler_8);
	
	while(1)
	{
		// 这部分应该写需要被WWDG监控的程序，运行时间为减一个数的时间*(上窗口值-0x40)
		
		// ...
		
		// 计时器值初始化成最大0x7F，不断减小。当计数器减少到0x40触发死前中断，
		// 再减一次到了0x3F时复位。计数器的值大于窗口值时喂狗也会复位；
		// 所以要当计数器的值在窗口值和0x40之间的时候喂狗，其中0x40是固定的。		
		if((WWDG->CR & 0x7F) < (WWDG->CFR & 0x7F))
		{
			WWDG_Feed();
		}
	}
}
