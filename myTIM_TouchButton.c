#include "mylib.h"

// 通用定时器TIM参数定义，只限TIM2、3、4、5
#define TPAD_TIM_APBxClock		RCC_APB1PeriphClockCmd
#define TPAD_TIM_CLK			RCC_APB1Periph_TIM5
#define TPAD_TIM_CH_GPIO_CLK	RCC_APB2Periph_GPIOA
#define TPAD_TIM_CH_PORT		GPIOA
#define TPAD_TIM_CH_PIN			GPIO_Pin_1

#define TPAD_TIM_CHANNEL_x		TIM_Channel_2

#define TPAD_TIM				TIM5
#define TPAD_TIM_IT_CCx			TIM_IT_CC2
#define TPAD_TIM_Period			0XFFFF
#define TPAD_TIM_Prescaler		71
#define TPAD_TIM_IRQ			TIM5_IRQn
#define TPAD_TIM_INT_FUN		TIM5_IRQHandler

// 获取捕获寄存器值函数宏定义
#define TPAD_TIM_GetCapturex_FUN		TIM_GetCapture2
// 捕获信号极性函数宏定义
#define TPAD_TIM_OCxPolarityConfig_FUN	TIM_OC2PolarityConfig

// 电容按键被按下的时候门限值，需要根据不同的硬件实际测试，减小这个门限值可以提供响应速度
#define TPAD_GATE_VAL			70
// 电容按键空载的时候的最大和最小的充电时间，不同的硬件不一样，一般稳定在76
#define TPAD_DEFAULT_VAL_MIN	70
#define TPAD_DEFAULT_VAL_MAX	80

// 获取定时器捕获值（充电时间），如果超时，则直接返回定时器的计数值。
uint16_t TPAD_Get_Val(void)
{
	// 每次捕获的时候，必须先复位放电。
	// 开发板上电之后，电容按键默认已经充满了电，要想测得电容按键的充电时间
	// 就必须先把电容按键的电放掉，让接电容按键的IO输出低电平即可
	GPIO_InitTypeDef GPIO_InitStructure;

	// 输入捕获通道1 GPIO 初始化
	RCC_APB2PeriphClockCmd(TPAD_TIM_CH_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  TPAD_TIM_CH_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
	
	// 连接TPAD的IO配置为输出，然后输出低电平，延时一会，确保电容按键放电完毕
	GPIO_ResetBits(TPAD_TIM_CH_PORT, TPAD_TIM_CH_PIN);

	// 放电是很快的，一般是us级别
	delay_ms( 5 );
	
	// 放电完毕之后，再把连接电容按键的IO配置为输入，通过输入捕获的方法
	// 测量电容按键的充电时间。空载（没有手指触摸）的充电时间非常稳定，
	// 因为电路板的硬件已经确定了。不同的硬件充电时间不一样，需要实际测试所得
	// 一般为 76，如果你觉得单次测量不准确，你可以多次测量然后取个平均值。
	// 当有手指触摸的情况下，充电时间会变长，我们只需要
	// 对比这两个时间就可以知道电容按键是否有手指触摸。
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
	
	// 当电容按键复位放电之后，计数器清0开始计数
	TIM_SetCounter (TPAD_TIM, 0);
	// 清除相关的标志位
	TIM_ClearITPendingBit (TPAD_TIM, TPAD_TIM_IT_CCx | TIM_IT_Update);
	
	// 等待捕获上升沿，当电容按键充电到1.8V左右的时候，就会被认为是上升沿
	while(TIM_GetFlagStatus (TPAD_TIM, TPAD_TIM_IT_CCx) == RESET)
	{
		// 如果超时了，直接返回CNT的值；一般充电时间都是在ms级别以内，很少会超过定时器的最大计数值
		if (TIM_GetCounter(TPAD_TIM) > TPAD_TIM_Period-100)
		{
			return TIM_GetCounter (TPAD_TIM);
		}
	}
	
	// 获取捕获比较寄存器的值
	return TPAD_TIM_GetCapturex_FUN(TPAD_TIM);
}

void TPAD_TIM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 输入捕获通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(TPAD_TIM_CH_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TPAD_TIM_CH_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef time_init_t;
	TIM_ICInitTypeDef time_ic_init_t;
	
	// 开启定时器时钟,即内部时钟CK_INT=72M
	TPAD_TIM_APBxClock(TPAD_TIM_CLK, ENABLE);

	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	time_init_t.TIM_Period = TPAD_TIM_Period;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	time_init_t.TIM_Prescaler = TPAD_TIM_Prescaler;	
	// 时钟分频因子 ，配置死区时间时需要用到
	time_init_t.TIM_ClockDivision = TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	time_init_t.TIM_CounterMode = TIM_CounterMode_Up;		
	// 重复计数器的值，TIM_RepetitionCounter 只有 TIM[1,8,15,16,17]才有
	time_init_t.TIM_RepetitionCounter = 0;	
	// 初始化定时器
	TIM_TimeBaseInit(TPAD_TIM, &time_init_t);

	// 配置输入捕获的通道，需要根据具体的GPIO来配置
	time_ic_init_t.TIM_Channel = TPAD_TIM_CHANNEL_x;
	// 输入捕获信号的极性配置
	time_ic_init_t.TIM_ICPolarity = TIM_ICPolarity_Rising;
	// 输入通道和捕获通道的映射关系，有直连和非直连两种
	time_ic_init_t.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 输入的需要被捕获的信号的分频系数
	time_ic_init_t.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 输入的需要被捕获的信号的滤波系数
	time_ic_init_t.TIM_ICFilter = 0;
	// 定时器输入捕获初始化
	TIM_ICInit(TPAD_TIM, &time_ic_init_t);
	
	// 使能计数器
	TIM_Cmd(TPAD_TIM, ENABLE);
}


// 电容按键空载的时候充电时间
uint16_t tpad_default_val;

// 初始化触摸按键，获得空载的时候触摸按键的充电时间
uint8_t TPAD_Init(void)
{
	uint16_t temp;
	
	// 电容按键用到的输入捕获的IO和捕获模式参数初始化
	TPAD_TIM_Init();
	
	temp = TPAD_Get_Val();
	
	if ((temp > TPAD_DEFAULT_VAL_MIN) && (temp < TPAD_DEFAULT_VAL_MAX))
	{
		tpad_default_val = temp;
		return 0;
	}
	return 1;
}

// 读取num次定时器捕获值，返回读取到的最大定时器捕获值
uint16_t TPAD_Get_MaxVal( uint8_t num )
{
	uint16_t temp=0, max=0;
	while(num--)
	{
		temp = TPAD_Get_Val();
		if( temp > max )
			max = temp;
	}
	return max;	
}

uint8_t TPAD_Scan(void)
{
	// 采样多次，并取最大值，小的一般是干扰或者是误触摸
	// 当扫描的值大于空载值加上门限值之后，表示按键按下
	if(TPAD_Get_MaxVal(3) > (tpad_default_val + TPAD_GATE_VAL))
	{
		// 再次检测，类似于机械按键的去抖
		if (TPAD_Get_MaxVal(3) > (tpad_default_val + TPAD_GATE_VAL))
			return 1;
	}
	return 0;
}
