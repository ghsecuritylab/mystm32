#include "mylib.h"

#define ADC_G_ClockCmd		RCC_APB2PeriphClockCmd
#define ADC_G_				RCC_APB2Periph_GPIOC
#define ADC_ClockCmd		RCC_APB2PeriphClockCmd
#define ADC_				RCC_APB2Periph_ADC1

#define ADCX				ADC1
#define ADCXIRQn			ADC1_2_IRQn
#define ADCXIRQHandler		ADC1_2_Handler

#define GPIO_ADC			GPIOC
#define GPIO_ADC_Pin		GPIO_Pin_1
int16_t v;

void ADC_SINGLE_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	ADC_G_ClockCmd(ADC_G_, ENABLE);
	ADC_ClockCmd(ADC_,ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_AIN;
	gpio_init_t.GPIO_Pin = GPIO_ADC_Pin;
	GPIO_Init(GPIO_ADC, &gpio_init_t);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef nvic_init_t;
	nvic_init_t.NVIC_IRQChannel = ADCXIRQn;
	nvic_init_t.NVIC_IRQChannelCmd = ENABLE;
	nvic_init_t.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init_t.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvic_init_t);
	ADC_ITConfig(ADCX, ADC_IT_EOC, ENABLE);
	
	ADC_InitTypeDef adc_init_t;
	adc_init_t.ADC_Mode = ADC_Mode_Independent;	// 仅一个ADC，独立模式
	adc_init_t.ADC_ScanConvMode = DISABLE;		// 多通道时使用，单通道无需扫描
	adc_init_t.ADC_ContinuousConvMode = ENABLE;
	adc_init_t.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc_init_t.ADC_NbrOfChannel = 1;	// 转换通道数
	adc_init_t.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_Init(ADCX, &adc_init_t);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);	// 设置8分频，即9MHz
	
	// 设置各通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADCX, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
	
	ADC_Cmd(ADCX, ENABLE);
	
	ADC_ResetCalibration(ADCX);	// 初始化ADC校准寄存器
	while (ADC_GetResetCalibrationStatus(ADCX));
	ADC_StartCalibration(ADCX);	// 启动ADC校准
	while (ADC_GetCalibrationStatus(ADCX));
	
	ADC_SoftwareStartConvCmd(ADCX, ENABLE);
	
//	while (1)
//	{
//		// do something with v
//	}
}

void ADCXIRQHandler()
{
	if(ADC_GetITStatus(ADCX, ADC_IT_EOC)==SET)
		v = ADC_GetConversionValue(ADCX)*255/4096;
	ADC_ClearITPendingBit(ADCX, ADC_IT_EOC);
}
