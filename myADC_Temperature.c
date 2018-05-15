#include "mylib.h"

// 对于12位的ADC，3.3V的ADC值为0xfff，
// 温度为25度时对应的电压值为 1.43V，ADC值为 0x6EE
#define V25			0x6EE

// 斜率，每摄氏度 4.3mV 对应每摄氏度 0x05
#define AVG_SLOPE	0x05 

uint16_t ADC_ConvertedValue[1];

void Temp_ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	// Enable ADC1 and GPIOC clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	// 自己写的 DMA 初始化
	DMA_ADC_Config(ADC1, RCC_AHBPeriph_DMA1, DMA1_Channel1, ADC_ConvertedValue, 1);
	
	// 独立工作模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	// 多通道
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	// 连续转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// 由软件触发启动
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	// 仅一个通道转换
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	// 配置ADC时钟，为PCLK2的8分频，即9Hz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

	// 设置采样通道16, 设置采样时间
	// 温度传感器在内部和 ADC1_IN16 输入通道相接
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	

	// 使能温度传感器和内部参考电压
	ADC_TempSensorVrefintCmd(ENABLE);
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// Celsius degree
uint16_t Temp_ADC1_Get(void)
{
	delay_ms(500);
	
	//return (1.43 - ADC_ConvertedValue[0]*3.3/4096)*1000 / 4.3 + 25;
	return (V25-ADC_ConvertedValue[0])/AVG_SLOPE+25;
}
