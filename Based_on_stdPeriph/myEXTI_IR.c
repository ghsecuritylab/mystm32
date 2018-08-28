#include "mylib.h"

#define IR_DEVICEID		0

// 接收 NEC 红外遥控使用的GPIO及时钟
#define IR_GPIO_PORT		GPIOE
#define IR_GPIO_CLK			RCC_APB2Periph_GPIOE
#define IR_GPIO_PIN			GPIO_Pin_5
#define IR_GPIO_PORT_SOURCE	GPIO_PortSourceGPIOE
#define IR_GPIO_PIN_SOURCE	GPIO_PinSource5

// 中断相关
#define IR_EXTI_LINE		EXTI_Line5
// EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn, EXTI9_5_IRQn, EXTI15_10_IRQn
#define IR_EXTI_IRQN		EXTI9_5_IRQn
#define IR_EXTI_IRQHANDLER	EXTI9_5_IRQHandler

// 读取引脚的电平
#define IR_DATA_IN()		GPIO_ReadInputDataBit(IR_GPIO_PORT, IR_GPIO_PIN)

uint32_t frame_data=0;	// 一帧数据缓存
uint8_t	frame_cnt=0;
uint8_t	frame_flag=0;	// 一帧数据接收完成标志

/* 初始化红外接收头1838用到的IO */
void IR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(IR_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

	/* config the NVIC */
	NVIC_CONFIG(IR_EXTI_IRQN, 1, 6);
	
	/* EXTI line gpio config */
	GPIO_InitStructure.GPIO_Pin = IR_GPIO_PIN;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
	GPIO_Init(IR_GPIO_PORT, &GPIO_InitStructure);

	/* EXTI line mode config */
	GPIO_EXTILineConfig(IR_GPIO_PORT_SOURCE, IR_GPIO_PIN_SOURCE); 
	EXTI_InitStructure.EXTI_Line = IR_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/* 获取保持高电平的时间 */
uint8_t Get_Pulse_Time(void)
{
	uint8_t time = 0;
	while (IR_DATA_IN())
	{
		time++;
		delay_us(20);
		if(time == 250)
			return time;   // 超时溢出
	}
	return time;
}

// 分析帧数据返回键值，校验失败返回 0
uint8_t IR_Process(void)
{
	// 帧数据有4个字节
	uint8_t first_byte, second_byte, third_byte, fouth_byte;

	first_byte = frame_data >> 24;	// 第一个字节是遥控的ID
	second_byte = (frame_data>>16) & 0xff;	// 第二个字节是第一个字节的反码
	third_byte = frame_data >> 8;	// 第三个数据是遥控的真正的键值
	fouth_byte = frame_data;		// 第四个字节是第三个字节的反码

	// 清标志位
	frame_flag = 0;

	if ((first_byte==(uint8_t)~second_byte) && (first_byte==IR_DEVICEID))
	{
		if( third_byte == (uint8_t)~fouth_byte )
			return third_byte;
	}

	return 0;
}

uint8_t isr_cnt;  /* 用于计算进了多少次中断 */

// IO 线下降沿中断，接红外接收头的数据管脚
void IR_EXTI_IRQHANDLER(void)
{
	uint8_t pulse_time = 0;
	uint8_t leader_code_flag = 0;	/* 引导码标志位，当引导码出现时，表示一帧数据开始 */
	uint8_t IR_data = 0;			/* 数据暂存位 */

	if(EXTI_GetITStatus(IR_EXTI_LINE) != RESET)	/* 确保是否产生了EXTI Line中断 */
	{
		while(1)
		{
			if( IR_DATA_IN()== SET )	/* 只测量高电平的时间 */
			{
				pulse_time = Get_Pulse_Time();

				/* >=5ms 不是有用信号 当出现干扰或者连发码时，也会break跳出while(1)循环 */
				if( pulse_time >= 250 )                
				{
					break;
				}

				if(pulse_time>=200 && pulse_time<250)	/* 获得前导位 4ms~4.5ms */
				{
					leader_code_flag = 1;
				}
				else if(pulse_time>=10 && pulse_time<50)	/* 0.56ms: 0.2ms~1ms */
				{
					IR_data = 0;
				}
				else if(pulse_time>=50 && pulse_time<100)	/* 1.68ms：1ms~2ms */
				{
					IR_data = 1;
				}
				else if (pulse_time>=100 && pulse_time<=200)	/* 2.1ms：2ms~4ms */
				{
					// 连发码，在第二次中断出现
					frame_flag = 1;	// 一帧数据接收完成
					frame_cnt++;	// 按键次数加1
					isr_cnt++;		// 进中断一次加1
					break;
				}

				if( leader_code_flag == 1 )
				{
					// 在第一次中断中完成
					frame_data <<= 1;
					frame_data += IR_data;
					frame_cnt = 0;
					isr_cnt = 1;
				}
			}
		}
		EXTI_ClearITPendingBit(IR_EXTI_LINE);	//清除中断标志位
	}
}

#include <stdio.h>
void IR_RECV_EXAMPLE(void)
{
	uint8_t key_val;

	/* 重新配置SysTick的中断优先级为最高，要不然SysTick延时中断抢占不了IO EXTI中断
	* 因为SysTick初始化时默认配置的优先级是最低的
	* 或者当你用其他定时器做延时的时候，要配置定时器的优先级高于IO EXTI中断的优先级
	*/
	NVIC_SetPriority (SysTick_IRQn, 0);

	// USART1 config 115200 8-N-1
	USART_CONFIG();
	printf("\r\n 这是一个红外遥控发射与接收实验 \r\n");

	// 初始化红外接收头CP1838用到的IO
	IR_Init();

	for (;;)
	{
		if (frame_flag == 1)	// 一帧红外数据接收完成
		{
			key_val = IR_Process();
			printf("\r\n key_val=%d \r\n",key_val);
			printf("\r\n 按键次数frame_cnt=%d \r\n",frame_cnt);
			printf("\r\n 中断次数isr_cnt=%d \r\n",isr_cnt);
		}
	}
}


void IDRA_RECV_RAW_EXAMPLE(void)
{
	
}

void IR_Send_Leader(void)
{
	tone(38000);	// NEC:38kHz
	delay_ms(9);	// NEC 编码的引导码为 9ms 的脉冲和 4.5ms 的间歇
	notone();
	delay_us(4500);
}

void IR_SEND_1(void)
{
	// NEC 编码是一种码宽表示法，以固定的脉冲时间（560μs）开始
	tone(38000);
	delay_us(560);
	// 再保持 3*560μs 的间歇表示逻辑 1 
	notone();
	delay_us(560);
	delay_us(560);
	delay_us(560);
}

void IR_SEND_0(void)
{
	// NEC 编码是一种码宽表示法，以固定的脉冲时间（560μs）开始
	tone(38000);
	delay_us(560);
	// 再保持 560μs 的间歇表示逻辑 0
	notone();
	delay_us(560);
}

void IR_SEND_LSB_BYTE(uint8_t c)
{
	if (c & 0x01)IR_SEND_1();else IR_SEND_0();
	if (c & 0x02)IR_SEND_1();else IR_SEND_0();
	if (c & 0x04)IR_SEND_1();else IR_SEND_0();
	if (c & 0x08)IR_SEND_1();else IR_SEND_0();
	if (c & 0x10)IR_SEND_1();else IR_SEND_0();
	if (c & 0x20)IR_SEND_1();else IR_SEND_0();
	if (c & 0x40)IR_SEND_1();else IR_SEND_0();
	if (c & 0x80)IR_SEND_1();else IR_SEND_0();
}

void IR_SEND_CMD(uint8_t deviceID, uint8_t commandID)
{
	// 引导码
	IR_Send_Leader();
	
	// 数据码的 LSB 在前，为了保证数据的正确性，发送 8 位遥控码后
	// 再发送 8 位遥控码的反码；再发送 8 位命令码和 8 位命令码的反码。
	IR_SEND_LSB_BYTE(deviceID);
	IR_SEND_LSB_BYTE(~deviceID);
	IR_SEND_LSB_BYTE(commandID);
	IR_SEND_LSB_BYTE(~commandID);
	
	notone();
}

void IR_Send_Repeat(void)
{
	tone(38000);	// NEC:38kHz
	delay_ms(9);	// 重复码的引导码为 9ms 的间歇和 2.25ms 的脉冲。
	notone();
	delay_us(2250);
	
	notone();
}

void IR_SEND_EXAMPLE(void)
{
	TIM_PWMOUT_CONFIG();
	notone();

	IR_SEND_CMD(IR_DEVICEID,0);
	
	// 发送重复码，周期 110ms，重复码只发引导码，占用 10ms的时间
	IR_Send_Repeat();
	delay_ms(100);
	IR_Send_Repeat();
	delay_ms(100);
	IR_Send_Repeat();
	delay_ms(100);
	IR_Send_Repeat();
}
