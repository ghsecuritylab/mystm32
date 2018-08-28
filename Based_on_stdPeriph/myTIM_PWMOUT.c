#include "mylib.h"

#define TIM_ClockCmd			RCC_APB1PeriphClockCmd
#define TIM_					RCC_APB1Periph_TIM3
#define TIMX					TIM3

#define TIM_CHClockCmd			RCC_APB2PeriphClockCmd
#define TIM_CH					RCC_APB2Periph_GPIOA
#define GPIO_CH1				GPIOA
#define GPIO_CH1_Pin			GPIO_Pin_7
//#define GPIO_CH2				GPIOA
//#define GPIO_CH2_Pin			GPIO_Pin_9

// 互补输出（仅限高级定时器
//#define TIM_CHNClockCmd		RCC_APB2PeriphClockCmd
//#define TIM_CHN				RCC_APB2Periph_GPIOB
//#define GPIO_CHN1				GPIOB
//#define GPIO_CHN1_Pin			GPIO_Pin_13

//// 刹车（仅限高级定时器
//#define TIM_BKINClockCmd		RCC_APB2PeriphClockCmd
//#define TIM_BKIN				RCC_APB2Periph_GPIOB
//#define GPIO_BKIN				GPIOB
//#define GPIO_BKIN_Pin			GPIO_Pin_12

#define CCRX						CCR2
#define TIM_OC_CH1_Init				TIM_OC2Init
#define TIM_OC_CH1_PreloadConfig	TIM_OC2PreloadConfig
//#define TIM_OC_CH2_Init			TIM_OC2Init
//#define TIM_OC_CH2_PreloadConfig	TIM_OC2PreloadConfig
#define TIMX_PERIOD					6
// SystemCoreClock / TIMX_PERIOD = 	TIMX_PRESCALER
#define TIMX_PRESCALER				12
// By default, set all to be high 
#define TIMX_PULSE					TIMX_PERIOD

void TIM_PWMOUT_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	TIM_CHClockCmd(TIM_CH, ENABLE);
//	TIM_CHNClockCmd(TIM_CHN, ENABLE);
//	TIM_BKINClockCmd(TIM_BKIN, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = GPIO_CH1_Pin;
	GPIO_Init(GPIO_CH1, &gpio_init_t);
//	gpio_init_t.GPIO_Pin = GPIO_CHN1_Pin;
//	GPIO_Init(GPIO_CHN1, &gpio_init_t);
//	gpio_init_t.GPIO_Pin = GPIO_BKIN_Pin;
//	GPIO_Init(GPIO_BKIN, &gpio_init_t);
//	GPIO_ResetBits(GPIO_BKIN, GPIO_BKIN_Pin);
	
	TIM_ClockCmd(TIM_, ENABLE);
	
	TIM_TimeBaseInitTypeDef time_init_t;
	// 设置定时器时钟与CK_INT频率与数字滤波器采样时钟频率分频比
	time_init_t.TIM_ClockDivision = TIM_CKD_DIV1;
	time_init_t.TIM_CounterMode = TIM_CounterMode_Up;
	time_init_t.TIM_Period = TIMX_PERIOD-1;		// 计数,0~65535
	time_init_t.TIM_Prescaler = TIMX_PRESCALER-1;
	time_init_t.TIM_RepetitionCounter = 0;		// 重复定时器，属于高级控制寄存器位
	TIM_TimeBaseInit(TIMX, &time_init_t);
	
	TIM_OCInitTypeDef oc_init_t;		// 输出比较
	oc_init_t.TIM_OCMode = TIM_OCMode_PWM1;		// PWM1
	oc_init_t.TIM_OutputState = TIM_OutputState_Enable;
	oc_init_t.TIM_OutputNState= TIM_OutputNState_Disable;	// 禁用互补输出
	oc_init_t.TIM_OCIdleState = TIM_OCIdleState_Set;		// 空闲状态下通道输出电平（BDTR_MOE清零时）
	oc_init_t.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// 必须与 TIM_OCIdleState 相反
	oc_init_t.TIM_OCPolarity = TIM_OCPolarity_High;			// 电平极性设置，决定定时器通道有效电平
	oc_init_t.TIM_OCNPolarity = TIM_OCNPolarity_High;
	oc_init_t.TIM_Pulse = TIMX_PULSE;	// 脉冲宽度，0-Period
	TIM_OC_CH1_Init(TIMX, &oc_init_t);	// 配置通道1
	TIM_OC_CH1_PreloadConfig(TIMX,TIM_OCPreload_Enable);
//	TIM_OC_CH2_Init(TIMX, &oc_init_t);	// 配置通道2
//	TIM_OC_CH2_PreloadConfig(TIMX,TIM_OCPreload_Enable);
	
//	// 初始化刹车
//	TIM_BDTRInitTypeDef bdtr_init_t;
//	bdtr_init_t.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//	bdtr_init_t.TIM_Break = TIM_Break_Enable;
//	bdtr_init_t.TIM_BreakPolarity = TIM_BreakPolarity_High;	// 检测到高电平时停止输出
//	bdtr_init_t.TIM_DeadTime = 11;	// 输出比较信号死区时间为 152ns
//	bdtr_init_t.TIM_LOCKLevel = TIM_LOCKLevel_1;
//	bdtr_init_t.TIM_OSSIState = TIM_OSSIState_Enable;	// 空闲模式下关闭状态选择
//	bdtr_init_t.TIM_OSSRState = TIM_OSSRState_Enable;	// 运行模式下关闭状态选择
//	TIM_BDTRConfig(TIMX, &bdtr_init_t);
	
	TIM_Cmd(TIMX, ENABLE);
//	TIM_CtrlPWMOutputs(TIMX, ENABLE);	// 使用通用定时器（TIM2-TIM5） 时这句不需要
}

void duty_cycle(uint16_t pulse)
{
	// 0 <= pulse <= TIMX_PERIOD-1
	TIMX->CCRX = pulse;	// TIM_SetCompareX(TIMX, pulse)
}
void PWM_BREATH_EXAMPLE(void)
{
	while (1)
	{
		for(int8_t i = 0; i < TIMX_PERIOD; i++) {
			duty_cycle(i);
			delay_ms(100);
		}
		for(int8_t i = TIMX_PERIOD-1; i >= 0; i--) {
			duty_cycle(i);
			delay_ms(100);
		}
	}
}

const uint16_t notes_c4[] = {264,292,330,352,396,440,495,523};
const uint16_t notes_c5[] = {523,587,659,698,784,880,988,1046};
const uint16_t notes_c6[] = {1046,1175,1319,1397,1568,1760,1976,2093};
const uint16_t notes_c7[] = {2093,2349,2637,2794,3136,3520,3951,4186};
void tone(uint16_t Hz)
{
	// Hz = 72000000 / PERIOD / PSC
	TIMX->CCRX = TIMX_PERIOD/2;	// 比较寄存器 TIM_SetCompareX(TIMX, pulse)
	TIMX->ARR = TIMX_PERIOD-1;	// 自动重装值 TIM_SetAutoreload(TIMX, TIMX_PERIOD-1);
	TIMX->PSC = (SystemCoreClock / TIMX_PERIOD) / Hz - 1;	// 预分频
}
void notone()
{
	// 将PWM作为输入引脚时通电逻辑相反，置为全高即可
	TIMX->CCRX = TIMX_PERIOD;
}
void PWM_MELODY_EXAMPLE(void)
{
	while (1)
	{
		for(int8_t i = 0; i < sizeof(notes_c6)/sizeof(uint16_t); i++) {
			tone(notes_c7[i]);
			delay_ms(400);
			notone();
			delay_ms(200);
		}
	}
}

void PWM_WAVE_PLAYER_EXAMPLE(void)
{
	uint32_t imageFileInfo (const char *name, const uint8_t **data);
	const uint8_t* data;
	uint32_t size = imageFileInfo("music.pcm", &data);
	
	TIMX->ARR = 65535;	// 16bit采样精度
	TIMX->PSC = 1;		// 以最高频率输出音量PWM
	while (1)
	{
		for(uint32_t i = 0; i < size/2; i++) {
			TIMX->CCRX = ((uint16_t*)data)[i];
			delay_us(91);		// 1/91us =10989Hz ~~ 11025Hz 采样率
		}
	}
}
