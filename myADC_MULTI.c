#include "mylib.h"

#define ADC_G_ClockCmd		RCC_APB2PeriphClockCmd
#define ADC_G_				RCC_APB2Periph_GPIOB
#define ADC_ClockCmd		RCC_APB2PeriphClockCmd
#define ADC_				RCC_APB2Periph_ADC1

#define ADCX				ADC1
#define GPIO_ADC_CH2		GPIOB
#define GPIO_ADC_CH2_Pin	GPIO_Pin_0
#define GPIO_ADC_CH1		GPIOB
#define GPIO_ADC_CH1_Pin	GPIO_Pin_1
#define ADC_CH1				ADC_Channel_8
#define ADC_CH2				ADC_Channel_9

/*	双ADC模式时，第一个ADC必须是ADC1，
	ADC1和ADC2转换的数据都存放在ADC1的数据寄存器，
	ADC1的在低16位，ADC2的在高16位。	*/

#define NUM_OF_CHANNEL		2
uint16_t arr[NUM_OF_CHANNEL];

void ADC_MULTI_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	ADC_G_ClockCmd(ADC_G_, ENABLE);
	ADC_ClockCmd(ADC_,ENABLE);
	
	// 模拟输入模式
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化通道1
	gpio_init_t.GPIO_Pin = GPIO_ADC_CH1_Pin;
	GPIO_Init(GPIO_ADC_CH1, &gpio_init_t);
	
	// 初始化通道2
	gpio_init_t.GPIO_Pin = GPIO_ADC_CH2_Pin;
	GPIO_Init(GPIO_ADC_CH2, &gpio_init_t);
	
	// 自己写的 DMA 初始化
	DMA_ADC_Config(ADCX, RCC_AHBPeriph_DMA1, DMA1_Channel1, arr, NUM_OF_CHANNEL);
	
	ADC_InitTypeDef adc_init_t;
	// 仅一个ADC，独立模式，双 ADC 时两个都设为 ADC_Mode_RegSimult
	adc_init_t.ADC_Mode = ADC_Mode_Independent;	
	// 扫描模式，单通道不需要
	adc_init_t.ADC_ScanConvMode = ENABLE;
	// 连续转换模式
	adc_init_t.ADC_ContinuousConvMode = ENABLE;
	// 不用外部触发转换，软件开启即可
	adc_init_t.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 转换通道数
	adc_init_t.ADC_NbrOfChannel = NUM_OF_CHANNEL;
	// 转换结果右对齐
	adc_init_t.ADC_DataAlign = ADC_DataAlign_Right;
	// 初始化ADC
	ADC_Init(ADCX, &adc_init_t);
	// 设置 ADC 时钟为 PCLK2 的 8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	// 设置各通道转换顺序，采样时间为55.5个时钟周期，双ADC时设为ADC_SampleTime_239Cycles5
	ADC_RegularChannelConfig(ADCX, ADC_CH1, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCX, ADC_CH2, 2, ADC_SampleTime_55Cycles5);
	
	// 使用DMA时使用
	ADC_DMACmd(ADCX, ENABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(ADCX, ENABLE);
	
	// 初始化ADC校准寄存器
	ADC_ResetCalibration(ADCX);
	// 等待校准寄存器初始化完成
	while (ADC_GetResetCalibrationStatus(ADCX));
	// 启动ADC校准
	ADC_StartCalibration(ADCX);
	// 等待校准完成
	while (ADC_GetCalibrationStatus(ADCX));
	// 由于没有采用外部触发，所以使用软件触发ADC转换
	ADC_SoftwareStartConvCmd(ADCX, ENABLE);
	
//	while (1)
//	{
//		// do something with arr[0]*255/4096
//	}
}
