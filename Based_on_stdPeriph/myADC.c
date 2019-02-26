#include "mylib.h"

// 温度传感器在内部和 ADC1 CH16 输入通道相接。读取芯片温度时使用
//#define GET_CHIP_TEMPERATURE

#define ADC_CH1_APBxClkCmd		RCC_APB2PeriphClockCmd
#define ADC_CH1_CLK				RCC_APB2Periph_GPIOC
#define ADC_CH1_PORT			GPIOC
#define ADC_CH1_PIN				GPIO_Pin_1

//#define ADC_CH2_APBxClkCmd		RCC_APB2PeriphClockCmd
//#define ADC_CH2_CLK				RCC_APB2Periph_GPIOB
//#define ADC_CH2_PORT			GPIOB
//#define ADC_CH2_PIN				GPIO_Pin_0

//#define ADC_CH3_APBxClkCmd		RCC_APB2PeriphClockCmd
//#define ADC_CH3_CLK				RCC_APB2Periph_GPIOB
//#define ADC_CH3_PORT			GPIOB
//#define ADC_CH3_PIN				GPIO_Pin_1

#define ADC_APBxClkCmd			RCC_APB2PeriphClockCmd
#define ADC_CLK					RCC_APB2Periph_ADC1
#define ADCx					ADC1
#define ADC_CH1					ADC_Channel_11
//#define ADC_CH2				ADC_Channel_8
//#define ADC_CH3				ADC_Channel_9

#define ADCx_IRQn				ADC1_2_IRQn
#define ADCx_IRQHandler			ADC1_2_IRQHandler

// 通道数（不要忘了温度传感器使用的通道）
#define NUM_OF_CHANNEL		1
uint16_t arr[NUM_OF_CHANNEL];

void ADC_SINGLE_CONFIG(void)
{
	GPIO_InitTypeDef gpio_init_t;
	ADC_InitTypeDef adc_init_t;
	
	// 开启 GPIO 时钟
	ADC_CH1_APBxClkCmd(ADC_CH1_CLK, ENABLE);

	// 模拟输入模式
	gpio_init_t.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化通道1
	gpio_init_t.GPIO_Pin = ADC_CH1_PIN;
	GPIO_Init(ADC_CH1_PORT, &gpio_init_t);
//	// 初始化通道2
//	gpio_init_t.GPIO_Pin = ADC_CH2_PIN;
//	GPIO_Init(ADC_CH2_PORT, &gpio_init_t);
//	// 初始化通道3
//	gpio_init_t.GPIO_Pin = ADC_CH3_PIN;
//	GPIO_Init(ADC_CH3_PORT, &gpio_init_t);
	
	// 开启 ADC 总线时钟
	ADC_APBxClkCmd(ADC_CLK, ENABLE);
	// 自己写的 DMA 初始化，多通道时必须使用 DMA
	//DMA_ADC_Config(ADCx, RCC_AHBPeriph_DMA1, DMA1_Channel1, arr, NUM_OF_CHANNEL);

	/* 仅一个ADC，独立模式，双 ADC 时两个 ADC_Mode 都设为 ADC_Mode_RegSimult
	 * 双ADC模式时，第一个ADC必须是ADC1，
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
	ADC_Init(ADCx, &adc_init_t);
	
	// 设置8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	// 设置各通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADCx, ADC_CH1, 1, ADC_SampleTime_55Cycles5);
//	ADC_RegularChannelConfig(ADCx, ADC_CH2, 2, ADC_SampleTime_55Cycles5);
//	ADC_RegularChannelConfig(ADCx, ADC_CH3, 3, ADC_SampleTime_55Cycles5);
	
#ifdef GET_CHIP_TEMPERATURE
	// 设置采样通道16, 设置采样时间为 1/9MHz*239.5=26.61us
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);
	
	// 使能温度传感器和内部参考电压
	ADC_TempSensorVrefintCmd(ENABLE);
#endif

//	// 配置中断
//	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);
//	NVIC_CONFIG(ADCx_IRQn, 1, 2);

	// 使用DMA时使用
	//ADC_DMACmd(ADCx, ENABLE);

	// 使能 ADC
	ADC_Cmd(ADCx, ENABLE);
	
	ADC_ResetCalibration(ADCx);	// 初始化ADC校准寄存器
	while (ADC_GetResetCalibrationStatus(ADCx));
	ADC_StartCalibration(ADCx);	// 启动ADC校准
	while (ADC_GetCalibrationStatus(ADCx));
	
	// 使能ADC的软件转换启动功能
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}
double Vsense;
// Celsius degree. Make sure VREF is connected to VCC!
double ADC_Temperature(void)
{
// 对于12位的ADC，3.3V的ADC值为0xfff，
// 温度为25度时对应的电压值为 1.43V
#define V25			1.43

// 斜率，4.3mV/`C
#define AVG_SLOPE	4.3
	
	double Vsense = ADC_GetConversionValue(ADC1)*3.3/0xFFF;
	//double Vsense = arr[0]*3.3/0xFFF;	// DMA 使用
	return (V25 - Vsense)*1000/AVG_SLOPE+25;	// 计算摄氏度
}

//void ADCx_IRQHandler()
//{
//	if(ADC_GetITStatus(ADCx, ADC_IT_EOC)==SET)
//		Vsense = ADC_GetConversionValue(ADCx)*3.3/0xFFF;
//	ADC_ClearITPendingBit(ADCx, ADC_IT_EOC);
//}
