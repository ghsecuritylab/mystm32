#include "mylib.h"

// 对于12位的ADC，3.3V的ADC值为0xfff，
// 温度为25度时对应的电压值为 1.43V，ADC值为 0x6EE
#define V25			0x6EE

// 斜率，每摄氏度 4.3mV 对应每摄氏度 0x05
#define AVG_SLOPE	0x05 

void Temp_ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	// Enable ADC1 and GPIOC clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// 独立工作模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	// 扫描模式，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	// 连续转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// 由软件触发启动
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	// 仅一个通道转换
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	// 配置ADC时钟，为PCLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

	// 设置采样通道16, 设置采样时间
	// 温度传感器在内部和 ADC1_IN16 输入通道相接。采样时间为 1/9MHz*239.5=26.61us
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);

	// 使能温度传感器和内部参考电压
	ADC_TempSensorVrefintCmd(ENABLE);

	// 使用DMA时使用
	//ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// Celsius degree. Make sure VREF is connected to VCC!
float Temp_ADC1_Get(void)
{
	delay_ms(30);
	
	//return (V25 - ADC1->DR)/AVG_SLOPE+25;
	return (1.43 - (ADC1->DR)*3.3/4096.0) / 0.0043 + 25;
}
