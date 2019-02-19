#include "mylib.h"
// ATGM336H-5N 系列是 GNSS 接收机模块。
// GNSS 接收机根据 NMEA-0183 协议的标准规范，
// 将位置、速度等信息通过串口传送到 PC 机、PDA 等设备。

#if 1

// NMEA-0183 数据解析

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
void readable_utctime(const char* p)
{
	// hhmmss.sss
	printf("Readable UTC Time:\t%c%c:%c%c:%c%c\r\n",
		p[0], p[1], p[2], p[3], p[4], p[5]);
}

void readable_utcdate(const char* p)
{
	// DDMMYY
	printf("Readable UTC Date:\t20%c%c-%c%c-%c%c\r\n",
		p[4], p[5], p[2], p[3], p[0], p[1]);
}

void readable_latitude(const char* p)
{
	// ddmm.mmmm
	char buffer[3];
	buffer[0] = p[0];
	buffer[1] = p[1];
	buffer[2] = '\0';
	int d = atoi(buffer);
	double m = atof(p + 2) / 60;
	while (*p != ',')++p;	// N or S
	printf("Readable latitude:\t%lf,%c\r\n", d + m, p[1]);
}

void readable_longitude(const char* p)
{
	// dddmm.mmmm
	char buffer[4];
	buffer[0] = p[0];
	buffer[1] = p[1];
	buffer[2] = p[2];
	buffer[3] = '\0';
	int d = atoi(buffer);
	double m = atof(p + 3) / 60;
	while (*p != ',')++p;	// W or E
	printf("Readable longitude:\t%lf,%c\r\n", d + m, p[1]);
}

uint8_t GNGGA(char* pBuf)
{
	char* p = pBuf;
	printf("GNGGA:\tGPS Fix Data\r\n");
	if (*p == ',')
		++p;	// 没有 UTC 时间
	else {
		p[10] = '\0';	// 替换逗号
		//printf("UTC Time(hhmmss.sss):\t%s\r\n", p);
		readable_utctime(p);
		p += 11;
	}
	if (*p == ',')
		p += 2;	// 没有纬度
	else {
		p[11] = '\0';
		//printf("Latitude(ddmm.mmmm):\t%s\r\n", p);
		readable_latitude(p);
		p += 12;
	}
	if (*p == ',')
		p += 2;	// 没有经度
	else {
		p[12] = '\0';
		//printf("Longitude(dddmm.mmmm):\t%s\r\n", p);
		readable_longitude(p);
		p += 13;
	}
	if (*p == ',')
		++p;	// 没有GPS状态
	else {
		switch (*p) {
		case '0': printf("GPS State(0):\tNot positioned\r\n"); break;
		case '1': printf("GPS State(1):\tNon-differential positioning\r\n"); break;
		case '2': printf("GPS State(2):\tDifferential positioning\r\n"); break;
		case '3': printf("GPS State(3):\tInvalid PPS\r\n"); break;
		case '6': printf("GPS State(6):\tEstimating\r\n"); break;
		default: printf("GPS State(%s):\tUnknown\r\n", p);
		}
		p += 2;
	}
	if (*p == ',')
		++p;	// 卫星数量
	else {
		p[2] = '\0';
		printf("Num of satelites used:\t%s\r\n", p);
		p += 3;
	}
	if (*p == ',')
		++p;	// 没有HDOP水平精度因子
	else {
		if (p[3] == ',') p[3] = '\0';
		else if (p[4] == ',') p[4] = '\0';
		printf("HDOP:\t%s\r\n", p);
		if (p[3] == '\0') p += 4;
		else if (p[4] == '\0') p += 5;
	}
	if (*p == ',')
		++p;	// 海拔
	else {
		if (p[3] == ',') p[3] = '\0';
		else if (p[4] == ',') p[4] = '\0';
		else if (p[5] == ',') p[5] = '\0';
		else if (p[6] == ',') p[6] = '\0';
		else if (p[7] == ',') p[7] = '\0';
		printf("Altitude(m):\t%s\r\n", p);
		if (p[3] == '\0') p += 4;
		else if (p[4] == '\0') p += 5;
		else if (p[5] == '\0') p += 6;
		else if (p[6] == '\0') p += 7;
		else if (p[7] == '\0') p += 8;
	}
	printf("\r\n");
	while (*p != '\n')++p;
	return (uint8_t)(p - pBuf + 1);
}

uint8_t GNRMC(char* pBuf)
{
	char* p = pBuf;
	printf("GNRMC:\tRecommended Minimum Specific GPS/TRANSIT Data\r\n");
	if (*p == ',')
		++p;	// 没有 UTC 时间
	else {
		p[10] = '\0';	// 替换逗号
		//printf("UTC Time(hhmmss.sss):\t%s\r\n", p);
		readable_utctime(p);
		p += 11;
	}
	if (*p == ',')
		++p;	// 没有状态
	else {
		switch (*p) {
		case 'A': printf("Position State(A):\tPositioned\r\n"); break;
		case 'V': printf("Position State(V):\tNot positioned\r\n"); break;
		default: printf("Position State(%s):\tUnknown\r\n", p);
		}
		p += 2;
	}
	if (*p == ',')
		p += 2;	// 没有纬度
	else {
		p[11] = '\0';
		//printf("Latitude(ddmm.mmmm):\t%s\r\n", p);
		readable_latitude(p);
		p += 12;
	}
	if (*p == ',')
		p += 2;	// 没有经度
	else {
		p[12] = '\0';
		//printf("Longitude(dddmm.mmmm):\t%s\r\n", p);
		readable_longitude(p);
		p += 13;
	}
	if (*p == ',')
		++p;	// 没有速度（节，Knots）
	else {
		if (p[3] == ',') p[3] = '\0';
		else if (p[4] == ',') p[4] = '\0';
		else if (p[5] == ',') p[5] = '\0';
		else if (p[6] == ',') p[6] = '\0';
		else if (p[7] == ',') p[7] = '\0';
		printf("Speed(Knots):\t%s\r\n", p);
		if (p[3] == '\0') p += 4;
		else if (p[4] == '\0') p += 5;
		else if (p[5] == '\0') p += 6;
		else if (p[6] == '\0') p += 7;
		else if (p[7] == '\0') p += 8;
	}
	if (*p == ',')
		++p;	// 没有方位角（度）
	else {
		if (p[3] == ',') p[3] = '\0';
		else if (p[4] == ',') p[4] = '\0';
		else if (p[5] == ',') p[5] = '\0';
		else if (p[6] == ',') p[6] = '\0';
		else if (p[7] == ',') p[7] = '\0';
		printf("Azimuth angle(degree):\t%s\r\n", p);
		if (p[3] == '\0') p += 4;
		else if (p[4] == '\0') p += 5;
		else if (p[5] == '\0') p += 6;
		else if (p[6] == '\0') p += 7;
		else if (p[7] == '\0') p += 8;
	}
	if (*p == ',')
		++p;	// 没有 UTC 日期
	else {
		p[6] = '\0';
		//printf("UTC Date(DDMMYY):\t%s\r\n", p);
		readable_utcdate(p);
		p += 7;
	}
	printf("\r\n");
	while (*p != '\n')++p;
	return (uint8_t)(p - pBuf + 1);
}

uint8_t GPTXT(char* pBuf)
{
	char* p;
	printf("GPTXT:\tText Transit Data\r\n");
	p = strstr(pBuf, "ANTENNA");
	if (p != NULL) {
		switch (p[9]) {
		case 'H':printf("Antenna:\tShort\r\n"); break;
		case 'P':printf("Antenna:\tOpen\r\n"); break;
		case 'K':printf("Antenna:\tOK\r\n"); break;
		default: printf("Antenna:\tUnknown\r\n"); break;
		}
	} else printf("Antenna:\tUnknown\r\n");

	printf("\r\n");
	while (*p != '\n')++p;
	return (uint8_t)(p - pBuf + 1);
}

#if defined (__CC_ARM)
#pragma anon_unions
#endif

// 最大接收缓存字节数
#define RX_BUF_MAX_LEN     1024

// 串口数据帧的处理结构体
struct STRUCT_USARTx_Frame
{
	char Data_RX_BUF[RX_BUF_MAX_LEN];
	union {
		__IO uint16_t InfoAll;
		struct {
			__IO uint16_t FrameLength		:15;	// 14:0 
			__IO uint16_t FrameFinishFlag	:1;		// 15 
		} InfoBit;
	};
};

struct STRUCT_USARTx_Frame strGNSS_Frame_Record = { 0 };

#define GNSS_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
#define GNSS_USART_GPIO_CLK			RCC_APB2Periph_GPIOB
#define GNSS_USART_TX_PORT			GPIOB
#define GNSS_USART_TX_PIN			GPIO_Pin_10
#define GNSS_USART_RX_PORT			GPIOB
#define GNSS_USART_RX_PIN			GPIO_Pin_11

#define GNSS_USART_APBxClkCmd		RCC_APB1PeriphClockCmd
#define GNSS_USART_CLK				RCC_APB1Periph_USART3
#define GNSS_USARTx					USART3
#define GNSS_USART_BAUD_RATE		9600
#define GNSS_USART_IRQ				USART3_IRQn
#define GNSS_USART_INT_FUN			USART3_IRQHandler

void	GNSS_USART_Config			(void);
char*	GNSS_ReceiveString			(void);

// 初始化GNSS用到的 USART
void GNSS_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART clock */
	GNSS_USART_APBxClkCmd(GNSS_USART_CLK, ENABLE);
	GNSS_USART_GPIO_APBxClkCmd(GNSS_USART_GPIO_CLK, ENABLE);
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GNSS_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GNSS_USART_TX_PORT, &GPIO_InitStructure);
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GNSS_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GNSS_USART_RX_PORT, &GPIO_InitStructure);
	
	/* USART mode config */
	USART_InitStructure.USART_BaudRate = GNSS_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(GNSS_USARTx, &USART_InitStructure);
	
	/* 中断配置 */
	USART_ITConfig (GNSS_USARTx, USART_IT_RXNE, ENABLE); // 使能串口接收中断
	USART_ITConfig (GNSS_USARTx, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

	NVIC_CONFIG(GNSS_USART_IRQ, 0, 1);
	
	USART_Cmd(GNSS_USARTx, ENABLE);
}

// 串口接收中断处理函数，将 GNSS 发送来的数据保存到 Data_RX_BUF 中
void GNSS_USART_INT_FUN(void)
{
	uint8_t c;
	if (USART_GetITStatus(GNSS_USARTx, USART_IT_RXNE) == SET)	// 接收中断
	{
		c = USART_ReceiveData(GNSS_USARTx);
		if (strGNSS_Frame_Record.InfoBit.FrameLength < (RX_BUF_MAX_LEN - 1))	// 预留1个字节写结束符
			strGNSS_Frame_Record.Data_RX_BUF[strGNSS_Frame_Record.InfoBit.FrameLength++] = c;
	}
	if (USART_GetITStatus(GNSS_USARTx, USART_IT_IDLE) == SET)	// 数据帧接收完毕
	{
		strGNSS_Frame_Record.InfoBit.FrameFinishFlag = 1;
		c = USART_ReceiveData(GNSS_USARTx);	// 由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	}
}

// GNSS模块接收字符串返回接收到的字符串首地址
char* GNSS_ReceiveString(void)
{
	strGNSS_Frame_Record.InfoBit.FrameLength = 0;
	strGNSS_Frame_Record.InfoBit.FrameFinishFlag = 0;

	while (!strGNSS_Frame_Record.InfoBit.FrameFinishFlag);
	strGNSS_Frame_Record.Data_RX_BUF[strGNSS_Frame_Record.InfoBit.FrameLength] = '\0';
	return strGNSS_Frame_Record.Data_RX_BUF;
}

#include <stdio.h>
void GNSS_EXAMPLE(void)
{
	char* i;
	GNSS_USART_Config();

	printf("configuring...\r\n");
	delay_ms(1000);

	while (1) {
		for (i = GNSS_ReceiveString(); *i; ++i)
		{
			if (*i == '$') {
				i[6] = '\0';	// 替换第一个逗号
				if (strcmp(i, "$GNGGA") == 0)
					i += GNGGA(i + 7) + 6;
				else if (strcmp(i, "$GNRMC") == 0)
					i += GNRMC(i + 7) + 6;
				else if (strcmp(i, "$GPTXT") == 0)
					i += GPTXT(i + 7) + 6;
				else
					i += 7;	// 跳过
			}
		}
	}
}
#endif
