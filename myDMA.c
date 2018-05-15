#include "mylib.h"
// DMA1 有7个通道，DMA2 有5个通道；DMA1 优先级高于 DMA2

void DMA_ADC_Config(ADC_TypeDef *ADCX, uint32_t DMA_, DMA_Channel_TypeDef *DMAX_ChannelX, uint16_t dest[], uint32_t ArrayLength)
{
	DMA_InitTypeDef dma_init_t;
	
	// 开启DMA时钟
	RCC_AHBPeriphClockCmd(DMA_, ENABLE);
	
	// 复位DMA
	DMA_DeInit(DMAX_ChannelX);

	// 设置DMA源地址：串口数据寄存器地址
	// ADC1 转换结果保存在DR寄存器，串口为(USARTX_BASE+0x04)
	dma_init_t.DMA_PeripheralBaseAddr = (uint32_t)&(ADCX->DR);
	// 存储器地址
	dma_init_t.DMA_MemoryBaseAddr = (uint32_t)dest;
	// 外设地址不增
	dma_init_t.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 内存地址自增
	dma_init_t.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 外设数据单位，双ADC模式时设为 DMA_PeripheralDataSize_Word
	dma_init_t.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// 内存数据单位，双ADC模式时设为 DMA_PeripheralDataSize_Word
	dma_init_t.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// 方向：从外设到内存
	dma_init_t.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 数据目的地大小,自增DataSize次
	dma_init_t.DMA_BufferSize = ArrayLength;
	// 禁止内存到内存的传输
	dma_init_t.DMA_M2M = DMA_M2M_Disable;
	// DMA模式，一次或者循环模式
	dma_init_t.DMA_Mode = DMA_Mode_Circular;
	// 优先级：高。当使用一个DMA通道时，优先级设置不影响；
	// 设置的优先级相同时，通道编号越低优先级越高
	dma_init_t.DMA_Priority = DMA_Priority_High;
	// 配置DMA通道
	DMA_Init(DMAX_ChannelX, &dma_init_t);
	// 使能DMA
	DMA_Cmd(DMAX_ChannelX, ENABLE);
}
