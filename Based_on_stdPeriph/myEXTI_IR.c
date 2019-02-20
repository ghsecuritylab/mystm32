#include "mylib.h"

// 遥控码
#define IR_DEVICEID		0

// 接收 NEC 红外遥控使用的GPIO及时钟
#define IR_GPIO_PORT		GPIOE
#define IR_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
#define IR_GPIO_CLK			RCC_APB2Periph_GPIOE
#define IR_GPIO_PIN			GPIO_Pin_5

// 中断相关
#define IR_GPIO_PORT_SOURCE	GPIO_PortSourceGPIOE
#define IR_GPIO_PIN_SOURCE	GPIO_PinSource5

#define IR_EXTI_LINE			EXTI_Line5
// EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn, EXTI9_5_IRQn, EXTI15_10_IRQn
#define IR_EXTI_IRQN			EXTI9_5_IRQn
//#define IR_EXTI_IRQHANDLER_NEC	EXTI9_5_IRQHandler
#define IR_EXTI_IRQHANDLER_RAW	EXTI9_5_IRQHandler

// 读取引脚的电平
#define IR_DATA_IN()		GPIO_ReadInputDataBit(IR_GPIO_PORT, IR_GPIO_PIN)

// 初始化红外接收头1838用到的IO
void IR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	IR_GPIO_APBxClkCmd(IR_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

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
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

// 获取保持高电平的时间（×20us）
uint8_t Get_Pulse_Time(void)
{
	uint8_t time = 0;
	while (IR_DATA_IN())
	{
		time++;
		delay_us(20);
		if(time == 250)
			return time;   // 5ms 超时溢出
	}
	return time;
}

/*
 * 脉冲是占空比为 0.5 的 PWM 波，间歇是占空比为 0 的 PWM 波，使用硬件 PWM 输出。NEC 编码使用 38kHz 的载波频率。
 * NEC 编码是一种码宽表示法，以固定的脉冲时间（560μs）开始，再保持 3*560μs 的脉冲间歇表示逻辑 1，或 560μs 的脉冲间歇表示逻辑 0。
 * 红外遥控发送的编码一般包含引导码和数据码两部分：引导码标记数据的开始，数据码是编码的主体，包括遥控码和命令码。
 * NEC 编码的引导码为 9ms 的脉冲和 4.5ms 的间歇；重复码的引导码为 9ms 的脉冲和 2.25ms 的间歇。数据码的 LSB 在前，为了保证数据的正确性，
 * 发送 8 位遥控码后再发送 8 位遥控码的反码；再发送 8 位命令码和 8 位命令码的反码。
 * 每个遥控器对应一个遥控码，每个按钮对应一个命令码。如果长时间按键则产生重复码，其形式多样：
 * NEC 的重复码只含引导码，不发数据码（周期 110ms）
 * 注意：1838 解调器的输出结果电平是反的
 */

uint8_t isr_cnt;  /* 用于计算进了多少次中断 */
uint32_t frame_data=0;	// 一帧数据缓存（加上反码一共 4 字节）
uint8_t	frame_cnt=0;
uint8_t	frame_flag=0;	// 一帧数据接收完成标志

// IO 线下降沿中断，接红外接收头的数据管脚。这里解析 NEC 编码
void IR_EXTI_IRQHANDLER_NEC(void)
{
	uint8_t pulse_time = 0;
	uint8_t leader_code_flag = 0;	/* 引导码标志位，当引导码出现时，表示一帧数据开始 */
	uint8_t IR_data = 0;			/* 数据暂存位 */

	if (EXTI_GetITStatus(IR_EXTI_LINE) == RESET)	/* 确保是否产生了EXTI Line中断 */
		return;

	while (1) {
		/* 只测量高电平的时间，由于 1838 解调器的输出结果电平是反的，相当于测量间歇时长 */
		if (IR_DATA_IN() == SET)
		{
			pulse_time = Get_Pulse_Time();

			/* >=5ms 不是有用信号 当出现干扰或者连发码时，也会break跳出while(1)循环 */
			if (pulse_time >= 250)
				break;

			if (pulse_time>=200 && pulse_time<250)	/* 获得前导位 4ms~4.5ms */
				leader_code_flag = 1;
			else if (pulse_time>=10 && pulse_time<50)	/* 0.56ms: 0.2ms~1ms */
				IR_data = 0;
			else if (pulse_time>=50 && pulse_time<100)	/* 1.68ms：1ms~2ms */
				IR_data = 1;
			else if (pulse_time>=100 && pulse_time<=200)	/* 2.25ms：2ms~4ms */
			{
				// 连发码，只在数据帧后出现
				frame_flag = 1;	// 一帧数据接收完成
				frame_cnt++;	// 按键次数加1
				isr_cnt++;		// 进中断一次加1
				break;
			}

			if (leader_code_flag == 1)
			{
				frame_data <<= 1;
				frame_data += IR_data;
				frame_cnt = 0;
				isr_cnt = 1;
			}
		}
	}
	EXTI_ClearITPendingBit(IR_EXTI_LINE);	// 清除中断标志位
}

// 分析帧数据返回键值，校验失败返回 0
uint8_t IR_Process_NEC(void)
{
	// 帧数据有4个字节
	uint8_t first_byte, second_byte, third_byte, fouth_byte;

	first_byte = frame_data >> 24;	// 第一个字节是遥控的ID
	second_byte = frame_data >> 16;	// 第二个字节是第一个字节的反码
	third_byte = frame_data >> 8;	// 第三个数据是遥控的真正的键值
	fouth_byte = frame_data;		// 第四个字节是第三个字节的反码

	// 清标志位：开始下一帧接收
	frame_flag = 0;

	if ((first_byte==~second_byte) && (first_byte==IR_DEVICEID) && (third_byte == ~fouth_byte))
		return third_byte;
	return 0;
}

#include <stdio.h>
void IR_RECV_NEC_EXAMPLE(void)
{
	/* 重新配置SysTick的中断优先级为最高，要不然SysTick延时中断抢占不了IO EXTI中断
	 * 因为SysTick初始化时默认配置的优先级是最低的，要配置定时器的优先级高于IO EXTI
	 * 中断的优先级。当你用其他定时器做延时的时候也是一样。
	 */
	NVIC_SetPriority(SysTick_IRQn, 0);

	printf("configuring... \r\n");

	// 初始化红外接收头CP1838用到的IO
	IR_Init();

	for (;;)
	{
		if (frame_flag == 1)	// 一帧红外数据接收完成
		{
			printf("key_val=%d \r\n", IR_Process_NEC());
			printf("frame_cnt=%d \r\n", frame_cnt);
			printf("isr_cnt=%d \r\n", isr_cnt);
		}
	}
}

void IR_Send_Leader_NEC(void)
{
	tone(38000);	// NEC:38kHz
	delay_ms(9);	// NEC 编码的引导码为 9ms 的脉冲和 4.5ms 的间歇
	notone();
	delay_us(4500);
}

void IR_Send_1(void)
{
	// NEC 编码是一种码宽表示法，以固定的脉冲时间（560μs）开始
	tone(38000);
	delay_us(560);
	// 再保持 3*560μs 的间歇表示逻辑 1 
	notone();
	delay_us(1680);
}

void IR_Send_0(void)
{
	// NEC 编码是一种码宽表示法，以固定的脉冲时间（560μs）开始
	tone(38000);
	delay_us(560);
	// 再保持 560μs 的间歇表示逻辑 0
	notone();
	delay_us(560);
}

void IR_Send_LSB_Byte(uint8_t c)
{
	if (c & 0x01)IR_Send_1();else IR_Send_0();
	if (c & 0x02)IR_Send_1();else IR_Send_0();
	if (c & 0x04)IR_Send_1();else IR_Send_0();
	if (c & 0x08)IR_Send_1();else IR_Send_0();
	if (c & 0x10)IR_Send_1();else IR_Send_0();
	if (c & 0x20)IR_Send_1();else IR_Send_0();
	if (c & 0x40)IR_Send_1();else IR_Send_0();
	if (c & 0x80)IR_Send_1();else IR_Send_0();
}

void IR_Send_Cmd_NEC(uint8_t deviceID, uint8_t commandID)
{
	// 引导码
	IR_Send_Leader_NEC();
	
	// 数据码的 LSB 在前，为了保证数据的正确性，发送 8 位遥控码后
	// 再发送 8 位遥控码的反码；再发送 8 位命令码和 8 位命令码的反码。
	IR_Send_LSB_Byte(deviceID);
	IR_Send_LSB_Byte(~deviceID);
	IR_Send_LSB_Byte(commandID);
	IR_Send_LSB_Byte(~commandID);
	
	notone();
}

void IR_Send_Repeat(void)
{
	tone(38000);	// NEC:38kHz
	delay_ms(9);	// 重复码的引导码为 9ms 的脉冲和 2.25ms 的间歇
	notone();
	delay_us(2250);
}

void IR_SEND_NEC_EXAMPLE(void)
{
	TIM_PWMOUT_CONFIG();
	notone();

	IR_Send_Cmd_NEC(IR_DEVICEID, 0);
	
	// 发送重复码，周期 110ms，重复码只发引导码，占用 10ms的时间
	IR_Send_Repeat();
	delay_ms(100);
	IR_Send_Repeat();
	delay_ms(100);
	IR_Send_Repeat();
	delay_ms(100);
	IR_Send_Repeat();
}

/*=================================================================
 * 通用红外接收/发射
 * 先接收原始红外数据，然后原样输出。
 */

// 获取保持低电平的时间（×20us）
uint16_t Get_Low_Time(void)
{
	uint16_t time = 0;
	while (IR_DATA_IN()==RESET)
	{
		time++;
		delay_us(20);
	}
	return time;
}

// 获取保持高电平的时间（×20us）
uint16_t Get_High_Time(void)
{
	uint16_t time = 0;
	while (IR_DATA_IN()==SET)
	{
		time++;
		delay_us(20);
		if(time == 2500)
			return time;   // 50ms 超时溢出
	}
	return time;
}

// 记录红外原始数据，第一个 uint16_t 是低电平时长（×20us），第二个是高电平时长，以此类推
uint16_t record[256];
uint16_t record_cnt;	// 有效元素个数（最后回到高电平，不用记录，因此应该是奇数）

void IR_EXTI_IRQHANDLER_RAW(void)
{
	uint16_t pulse_time = 0;

	if (EXTI_GetITStatus(IR_EXTI_LINE) == RESET)	/* 确保是否产生了EXTI Line中断 */
		return;

	record_cnt = 0;
	while (1) {
		record[record_cnt++] = Get_Low_Time();	// 下降沿触发，开始记录低电平时长
		if (record_cnt == 256) break;			// 达到记录限额
		pulse_time = Get_High_Time();			// 测量高电平时长
		if (pulse_time >= 2500) break;			// 超过 50ms 说明这帧发完
		record[record_cnt++] = pulse_time;
		if (record_cnt == 256) break;			// 达到记录限额
	}
	frame_flag = 1;	// 记录完成
	EXTI_ClearITPendingBit(IR_EXTI_LINE);	// 清除中断标志位
}

void IR_RECV_RAW_EXAMPLE(void)
{
	uint16_t i;
	/* 重新配置SysTick的中断优先级为最高，要不然SysTick延时中断抢占不了IO EXTI中断
	 * 因为SysTick初始化时默认配置的优先级是最低的，要配置定时器的优先级高于IO EXTI
	 * 中断的优先级。当你用其他定时器做延时的时候也是一样。
	 */
	NVIC_SetPriority(SysTick_IRQn, 0);

	printf("configuring... \r\n");

	// 初始化红外接收头CP1838用到的IO
	IR_Init();

	for (;;)
	{
		if (frame_flag == 1)	// 一帧红外数据接收完成
		{
			for (i=0; i < record_cnt; ++i)
				printf("%d,", record[i]);
			printf("\r\n");
			frame_flag = 0;
		}
	}
}

void IR_SEND_RAW_EXAMPLE(void)
{
	// 180ms 低 180ms 高交替
	uint16_t key[] = {
		9000,9000,9000,9000,9000,9000,9000,9000,9000,9000,
		9000,9000,9000,9000,9000,9000,9000,9000,9000,9000,
		9000,9000,9000,9000,9000,9000,9000,9000,9000,9000
	};
	uint16_t i;
	TIM_PWMOUT_CONFIG();
	notone();
	while(1) {
		for (i=0; i < sizeof(key)/sizeof(uint16_t); )
		{
			tone(38000);
			delay_us(key[i++]*20);
			notone();
			delay_us(key[i++]*20);		
		}
		delay_ms(1000);
	}
}
