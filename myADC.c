#include "mylib.h"

#define ADC_ClockCmd		RCC_APB2PeriphClockCmd
#define ADC_				RCC_APB2Periph_ADC1
#define ADC_G_ClockCmd		RCC_APB2PeriphClockCmd
#define ADC_G_CH1_			RCC_APB2Periph_GPIOC
//#define ADC_G_CH2_			RCC_APB2Periph_GPIOB
//#define ADC_G_CH3_			RCC_APB2Periph_GPIOB

#define ADCX				ADC1
#define GPIO_ADC_CH1		GPIOC
#define GPIO_ADC_CH1_Pin	GPIO_Pin_1
#define ADC_CH1				ADC_Channel_11
//#define GPIO_ADC_CH2		GPIOB
//#define GPIO_ADC_CH2_Pin	GPIO_Pin_0
//#define ADC_CH2			ADC_Channel_8
//#define GPIO_ADC_CH3		GPIOB
//#define GPIO_ADC_CH3_Pin	GPIO_Pin_1
//#define ADC_CH3			ADC_Channel_9

//#define ADCXIRQn			ADC1_2_IRQn
//#define ADCXIRQHandler		ADC1_2_Handler

// 温度传感器在内部和 ADC1_IN16 输入通道相接。读取芯片温度时使用
//#define GET_CHIP_TEMPERATURE

// 通道数（不要忘了温度传感器使用的通道）
#define NUM_OF_CHANNEL		1
uint16_t arr[NUM_OF_CHANNEL];

void ADC_SINGLE_CONFIG()
{
	GPIO_InitTypeDef gpio_init_t;
	ADC_InitTypeDef adc_init_t;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	// 开启 GPIO 时钟和 ADC 时钟
	ADC_G_ClockCmd(ADC_G_CH1_, ENABLE);
	ADC_ClockCmd(ADC_,ENABLE);
	
	// 模拟输入模式
	gpio_init_t.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化通道1
	gpio_init_t.GPIO_Pin = GPIO_ADC_CH1_Pin;
	GPIO_Init(GPIO_ADC_CH1, &gpio_init_t);
//	// 初始化通道2
//	gpio_init_t.GPIO_Pin = GPIO_ADC_CH2_Pin;
//	GPIO_Init(GPIO_ADC_CH2, &gpio_init_t);
//	// 初始化通道3
//	gpio_init_t.GPIO_Pin = GPIO_ADC_CH3_Pin;
//	GPIO_Init(GPIO_ADC_CH3, &gpio_init_t);
	
	// 自己写的 DMA 初始化，多通道时必须使用 DMA
	DMA_ADC_Config(ADCX, RCC_AHBPeriph_DMA1, DMA1_Channel1, arr, NUM_OF_CHANNEL);

//	// 配置中断
//	NVIC_CONFIG(ADCXIRQn, 10);
//	ADC_ITConfig(ADCX, ADC_IT_EOC, ENABLE);

	// 仅一个ADC，独立模式，双 ADC 时两个 ADC_Mode 都设为 ADC_Mode_RegSimult
	/* 双ADC模式时，第一个ADC必须是ADC1，
	 * ADC1和ADC2转换的数据都存放在ADC1的数据寄存器，
	 * ADC1的在低16位，ADC2的在高16位。
	 */
	adc_init_t.ADC_Mode = ADC_Mode_Independent;
	// 多通道时使用，单通道无需扫描
	adc_init_t.ADC_ScanConvMode = (NUM_OF_CHANNEL > 1 ? ENABLE : DISABLE);
	// 连续转换
	adc_init_t.ADC_ContinuousConvMode = ENABLE;
	// 由软件触发启动
	adc_init_t.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 右对齐
	adc_init_t.ADC_DataAlign = ADC_DataAlign_Right;
	// 转换通道数
	adc_init_t.ADC_NbrOfChannel = NUM_OF_CHANNEL;
	ADC_Init(ADCX, &adc_init_t);
	
	// 设置8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	// 设置各通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADCX, ADC_CH1, 1, ADC_SampleTime_239Cycles5);
//	ADC_RegularChannelConfig(ADCX, ADC_CH2, 2, ADC_SampleTime_55Cycles5);
//	ADC_RegularChannelConfig(ADCX, ADC_CH3, 3, ADC_SampleTime_55Cycles5);
	
#ifdef GET_CHIP_TEMPERATURE
	// 设置采样通道16, 设置采样时间为 1/9MHz*239.5=26.61us
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_239Cycles5);
	
	// 使能温度传感器和内部参考电压
	ADC_TempSensorVrefintCmd(ENABLE);
#endif

	// 使用DMA时使用
	ADC_DMACmd(ADCX, ENABLE);

	ADC_Cmd(ADCX, ENABLE);
	
	ADC_ResetCalibration(ADCX);	// 初始化ADC校准寄存器
	while (ADC_GetResetCalibrationStatus(ADCX));
	ADC_StartCalibration(ADCX);	// 启动ADC校准
	while (ADC_GetCalibrationStatus(ADCX));
	
	ADC_SoftwareStartConvCmd(ADCX, ENABLE);
}

// Celsius degree. Make sure VREF is connected to VCC!
float Temp_ADC1_Get(void)
{
	delay_ms(30);
	
// 对于12位的ADC，3.3V的ADC值为0xfff，
// 温度为25度时对应的电压值为 1.43V，ADC值为 0x6EE
#define V25			0x6EE

// 斜率，每摄氏度 4.3mV 对应每摄氏度 0x05
#define AVG_SLOPE	0x05 

	//return (V25 - ADC1->DR)/AVG_SLOPE+25;
	return (1.43 - arr[1]*3.3/4096.0) / 0.0043 + 25;
}

//void ADCXIRQHandler()
//{
//	int16_t v;
//	if(ADC_GetITStatus(ADCX, ADC_IT_EOC)==SET)
//		v = ADC_GetConversionValue(ADCX)*255/4096;
//	ADC_ClearITPendingBit(ADCX, ADC_IT_EOC);
//}
