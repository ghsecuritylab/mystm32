#include "mylib.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

// Bluetooth 模块为 JDY-18
#if 0
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

struct STRUCT_USARTx_Frame strBluetooth_Frame_Record = { 0 };

//#define Bluetooth_PWRC_APBxClkCmd		RCC_APB2PeriphClockCmd
//#define Bluetooth_PWRC_CLK				RCC_APB2Periph_GPIOB
//#define Bluetooth_PWRC_PORT				GPIOB
//#define Bluetooth_PWRC_PIN				GPIO_Pin_8

//#define Bluetooth_STAT_APBxClkCmd		RCC_APB2PeriphClockCmd
//#define Bluetooth_STAT_CLK				RCC_APB2Periph_GPIOB
//#define Bluetooth_STAT_PORT				GPIOB
//#define Bluetooth_STAT_PIN				GPIO_Pin_9

#define Bluetooth_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
#define Bluetooth_USART_GPIO_CLK		RCC_APB2Periph_GPIOB
#define Bluetooth_USART_TX_PORT			GPIOB
#define Bluetooth_USART_TX_PIN			GPIO_Pin_10
#define Bluetooth_USART_RX_PORT			GPIOB
#define Bluetooth_USART_RX_PIN			GPIO_Pin_11

#define Bluetooth_USART_APBxClkCmd		RCC_APB1PeriphClockCmd
#define Bluetooth_USART_CLK				RCC_APB1Periph_USART3
#define Bluetooth_USARTx				USART3
#define Bluetooth_USART_BAUD_RATE		9600
#define Bluetooth_USART_IRQ				USART3_IRQn
#define Bluetooth_USART_INT_FUN			USART3_IRQHandler

//#define Bluetooth_PWRC_HIGH_LEVEL()		GPIO_SetBits(Bluetooth_PWRC_PORT, Bluetooth_PWRC_PIN)
//#define Bluetooth_PWRC_LOW_LEVEL()		GPIO_ResetBits(Bluetooth_PWRC_PORT, Bluetooth_PWRC_PIN)
//#define Bluetooth_STAT_IN()				GPIO_ReadInputDataBit(Bluetooth_STAT_PORT, Bluetooth_STAT_PIN)

bool	Bluetooth_SetName				(char* name);
bool	Bluetooth_Cmd					(char* cmd, char* reply1, char* reply2, uint32_t waittime);
bool	Bluetooth_Mode_Choose			(char mode);
uint8_t	Bluetooth_Get_LinkStatus		(void);
uint8_t Bluetooth_SetUUID				(bool UUID128, char ServiceUUID[], char CharacUUID1[], char CharacUUID2[]);
uint8_t Bluetooth_Scan					(void);
uint8_t Bluetooth_SetMac				(char* mac);
uint8_t Bluetooth_Band					(char* mac);
uint8_t Bluetooth_Connect				(char* mac);
char*	Bluetooth_ReceiveString			(void);
uint8_t Bluetooth_SetPass				(char* password);
void	Bluetooth_GPIO_Config			(void);
void	Bluetooth_USART_Config			(void);

// 初始化Bluetooth用到的GPIO引脚
void Bluetooth_GPIO_Config ( void )
{
//	GPIO_InitTypeDef GPIO_InitStructure;

//	// 配置 PWRC 引脚
//	Bluetooth_PWRC_APBxClkCmd(Bluetooth_PWRC_CLK, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = Bluetooth_PWRC_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(Bluetooth_PWRC_PORT, &GPIO_InitStructure);

//	// 配置 STAT 引脚
//	Bluetooth_STAT_APBxClkCmd(Bluetooth_STAT_CLK, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = Bluetooth_STAT_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(Bluetooth_STAT_PORT, &GPIO_InitStructure);
}

// 初始化Bluetooth用到的 USART
void Bluetooth_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART clock */
	Bluetooth_USART_APBxClkCmd(Bluetooth_USART_CLK, ENABLE);
	Bluetooth_USART_GPIO_APBxClkCmd(Bluetooth_USART_GPIO_CLK, ENABLE);
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = Bluetooth_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Bluetooth_USART_TX_PORT, &GPIO_InitStructure);
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = Bluetooth_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(Bluetooth_USART_RX_PORT, &GPIO_InitStructure);
	
	/* USART mode config */
	USART_InitStructure.USART_BaudRate = Bluetooth_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(Bluetooth_USARTx, &USART_InitStructure);
	
	/* 中断配置 */
	USART_ITConfig (Bluetooth_USARTx, USART_IT_RXNE, ENABLE); // 使能串口接收中断
	USART_ITConfig (Bluetooth_USARTx, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

	NVIC_CONFIG(Bluetooth_USART_IRQ, 0, 1);
	
	USART_Cmd(Bluetooth_USARTx, ENABLE);
}

// 串口接收中断处理函数，将 Bluetooth 发送来的数据保存到 Data_RX_BUF 中
void Bluetooth_USART_INT_FUN(void)
{
	uint8_t c;
	if (USART_GetITStatus(Bluetooth_USARTx, USART_IT_RXNE) == SET)	// 接收中断
	{
		c = USART_ReceiveData(Bluetooth_USARTx);
		if (strBluetooth_Frame_Record.InfoBit.FrameLength < (RX_BUF_MAX_LEN - 1))	// 预留1个字节写结束符
			strBluetooth_Frame_Record.Data_RX_BUF[strBluetooth_Frame_Record.InfoBit.FrameLength++] = c;
	}
	if (USART_GetITStatus(Bluetooth_USARTx, USART_IT_IDLE) == SET)	// 数据帧接收完毕
	{
		strBluetooth_Frame_Record.InfoBit.FrameFinishFlag = 1;
		c = USART_ReceiveData(Bluetooth_USARTx);	// 由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	}
}

// 向Bluetooth发送数据
void Bluetooth_SendStr(char* str)
{
	for(uint32_t i = 0; str[i]; ++i)
	{
		USART_SendData(Bluetooth_USARTx, str[i]);
		while(USART_GetFlagStatus(Bluetooth_USARTx, USART_FLAG_TXE) == RESET);
	}
}

// 向Bluetooth发送数据
void Bluetooth_SendData(char* data, uint32_t size)
{
	for(uint32_t i = 0; i < size; ++i)
	{
		USART_SendData(Bluetooth_USARTx, data[i]);
		while(USART_GetFlagStatus(Bluetooth_USARTx, USART_FLAG_TXE) == RESET);
	}
}

#include <string.h>
// 向Bluetooth发送AT指令，reply1,reply2 为两种期待的响应字符串，0 不需响应
bool Bluetooth_Cmd(char* cmd, char* reply1, char* reply2, uint32_t waittime)
{
	strBluetooth_Frame_Record.InfoBit.FrameLength = 0;	// 从新开始接收新的数据包

	Bluetooth_SendStr(cmd);
	Bluetooth_SendStr("\r\n");

	if ((reply1 == 0) && (reply2 == 0))	// 不需要接收数据
		return true;

	delay_ms(waittime);
	
	strBluetooth_Frame_Record.Data_RX_BUF[strBluetooth_Frame_Record.InfoBit.FrameLength] = '\0';
	printf(">>> %s", strBluetooth_Frame_Record.Data_RX_BUF);

	if ((reply1 != 0) && (reply2 != 0))
		return ((bool)strstr(strBluetooth_Frame_Record.Data_RX_BUF, reply1) || 
				(bool)strstr(strBluetooth_Frame_Record.Data_RX_BUF, reply2)); 
	else if (reply1 != 0)
		return ((bool)strstr(strBluetooth_Frame_Record.Data_RX_BUF, reply1));
	else
		return ((bool)strstr(strBluetooth_Frame_Record.Data_RX_BUF, reply2));
}

bool Bluetooth_SetName(char* name)
{
	char nameBuffer[64];
	sprintf(nameBuffer, "AT+NAME%s", name);
	return Bluetooth_Cmd(nameBuffer, "OK", NULL, 200);
}

// 选择Bluetooth模块的工作模式 0:从机 1:主机
bool Bluetooth_Mode_Choose(char mode)
{
	// 查询模块工作模式
	Bluetooth_Cmd("AT+ROLE", "+ROLE=", 0, 2500);
	if (mode==0 && strstr(strBluetooth_Frame_Record.Data_RX_BUF, "+ROLE=0\r\n"))
		return true;
	if (mode==1 && strstr(strBluetooth_Frame_Record.Data_RX_BUF, "+ROLE=1\r\n"))
		return true;

	// 模块更改模式后需要重启
	switch (mode)
	{
	case 0:
		return Bluetooth_Cmd("AT+ROLE0", "OK", 0, 2000) && Bluetooth_Cmd("AT+RESET", "OK", 0, 500);
	case 1:
		return Bluetooth_Cmd("AT+ROLE1", "OK", 0, 2000) && Bluetooth_Cmd("AT+RESET", "OK", 0, 500);
	default:
		return false;
	}
}

// 设置 UUID
uint8_t Bluetooth_SetUUID(bool UUID128, char ServiceUUID[], char CharacUUID1[], char CharacUUID2[])
{
	char uuid[32];

	if (UUID128) {
		// 128 位 UUID
		Bluetooth_Cmd("AT+UUIDLEN1", "OK", 0, 500);

		strBluetooth_Frame_Record.InfoBit.FrameLength = 0;	// 从新开始接收新的数据包
		Bluetooth_SendStr("AT+SVRUUID");
		Bluetooth_SendData(ServiceUUID, 16);
		Bluetooth_SendStr("\r\n");
		delay_ms(200);
		
		strBluetooth_Frame_Record.Data_RX_BUF[strBluetooth_Frame_Record.InfoBit.FrameLength] = '\0';
		printf(">>> %s", strBluetooth_Frame_Record.Data_RX_BUF);

		strBluetooth_Frame_Record.InfoBit.FrameLength = 0;
		Bluetooth_SendStr("AT+CHRUUID");
		Bluetooth_SendData(CharacUUID1, 16);
		Bluetooth_SendStr("\r\n");
		delay_ms(200);
		
		strBluetooth_Frame_Record.Data_RX_BUF[strBluetooth_Frame_Record.InfoBit.FrameLength] = '\0';
		printf(">>> %s", strBluetooth_Frame_Record.Data_RX_BUF);

		strBluetooth_Frame_Record.InfoBit.FrameLength = 0;
		Bluetooth_SendStr("AT+CRXUUID");
		Bluetooth_SendData(CharacUUID2, 16);
		Bluetooth_SendStr("\r\n");
		delay_ms(200);
		
		strBluetooth_Frame_Record.Data_RX_BUF[strBluetooth_Frame_Record.InfoBit.FrameLength] = '\0';
		printf(">>> %s", strBluetooth_Frame_Record.Data_RX_BUF);

	} else {
		// 16 位 UUID
		Bluetooth_Cmd("AT+UUIDLEN1", "OK", 0, 200);
		sprintf(uuid, "AT+SVRUUID%X%X", ServiceUUID[0], ServiceUUID[1]);
		Bluetooth_Cmd(uuid, "OK", 0, 200);
		sprintf(uuid, "AT+CHRUUID%X%X", CharacUUID1[0], CharacUUID1[1]);
		Bluetooth_Cmd(uuid, "OK", 0, 200);
		sprintf(uuid, "AT+CRXUUID%X%X", CharacUUID2[0], CharacUUID2[1]);
		Bluetooth_Cmd(uuid, "OK", 0, 200);
	}
	// 复位模块
	return Bluetooth_Cmd("AT+RESET", "OK", 0, 500);
}

// 扫描周边从机，输出从机的 MAC、RSSI、NAME 信息
uint8_t Bluetooth_Scan(void) 
{
	return Bluetooth_Cmd("AT+INQ", "OK", 0, 2500);
}

// 设置 Bluetooth 的 MAC
uint8_t Bluetooth_SetMac(char* mac)
{
	char macBuffer[32];
	sprintf(macBuffer, "AT+LADDR%X%X%X%X%X%X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return Bluetooth_Cmd(macBuffer, "OK", 0, 200) && Bluetooth_Cmd("AT+RESET", "OK", 0, 500);
}

// 设置 Bluetooth 主从绑定
uint8_t Bluetooth_Band(char* mac)
{
	char macBuffer[32];
	if (mac == NULL) // 取消绑定
		return Bluetooth_Cmd("AT+CLRBAND", "OK", 0, 200);

	sprintf(macBuffer, "AT+BAND%X%X%X%X%X%X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return Bluetooth_Cmd(macBuffer, "OK", 0, 200);
}

// Bluetooth 连接从机
uint8_t Bluetooth_Connect(char* mac)
{
	char macBuffer[32];
	if (mac == NULL) // 断开连接
		return Bluetooth_Cmd("AT+DISC", "OK", 0, 2000);

	sprintf(macBuffer, "AT+CONN%X%X%X%X%X%X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return Bluetooth_Cmd(macBuffer, "OK", 0, 2500);
}

//// 获取 Bluetooth 的连接状态 1:已连接
//uint8_t Bluetooth_Get_LinkStatus(void)
//{
//	Bluetooth_PWRC_LOW_LEVEL();	// 如果已经连接，拉低 PWRC 以进入 AT 模式
//	Bluetooth_Cmd("AT+STAT", "+STAT", 0, 500);
//	Bluetooth_PWRC_HIGH_LEVEL();
//	return (bool)strstr(strBluetooth_Frame_Record.Data_RX_BUF, "+STAT=1");
//}

// Bluetooth模块接收字符串返回接收到的字符串首地址
char* Bluetooth_ReceiveString(void)
{
	strBluetooth_Frame_Record.InfoBit.FrameLength = 0;
	strBluetooth_Frame_Record.InfoBit.FrameFinishFlag = 0;

	while (!strBluetooth_Frame_Record.InfoBit.FrameFinishFlag);
	strBluetooth_Frame_Record.Data_RX_BUF[strBluetooth_Frame_Record.InfoBit.FrameLength] = '\0';
	return strBluetooth_Frame_Record.Data_RX_BUF;
}

// 指定从机的连接密码
uint8_t Bluetooth_SetPass(char* password)
{
	// 不绑定密码
	if (password == NULL)
		return Bluetooth_Cmd("AT+TYPE0", "+TYPE=", 0, 100) && Bluetooth_Cmd("AT+RESET", "OK", 0, 2000);

	// 密码必须是 6 位数字
	if (password[0] < '0' || password[0] > '9') return 0;
	if (password[1] < '0' || password[1] > '9') return 0;
	if (password[2] < '0' || password[2] > '9') return 0;
	if (password[3] < '0' || password[3] > '9') return 0;
	if (password[4] < '0' || password[4] > '9') return 0;
	if (password[5] < '0' || password[5] > '9') return 0;

	char cCmd [30];
	sprintf(cCmd, "AT+PASS%s", password);
	if (Bluetooth_Cmd(cCmd, 0, 0, 100) && 
		Bluetooth_Cmd("AT+TYPE1", "+TYPE=", 0, 100) &&
		Bluetooth_Cmd("AT+RESET", "OK", 0, 2000))
		return 1;
	else 
		return 0;
}

void Bluetooth_EXAMPLE(void)
{
	Bluetooth_GPIO_Config(); 
	Bluetooth_USART_Config();

	printf("configuring...\r\n");
	delay_ms(1000);
	// 0000ff00-0000-1000-8000-00805f9b34fb
	//Bluetooth_SetUUID(true, "\x00\x00\xff\x00\x00\x00\x10\x00\x80\x00\x00\x80\x5f\x9b\x34\xfb", 
	//	"\x00\x00\xff\x02\x00\x00\x10\x00\x80\x00\x00\x80\x5f\x9b\x34\xfb", 
	//	"\x00\x00\xff\x01\x00\x00\x10\x00\x80\x00\x00\x80\x5f\x9b\x34\xfb");
	Bluetooth_SetUUID(false, "\xAA\xBB", "\xCC\xDD", "\xEE\xFF");
	Bluetooth_SetMac("\xAA\xBB\xCC\xDD\xEE\xFF");
	
	// 设置名称
	Bluetooth_SetName("IoT Demo");
	// 从机模式
	Bluetooth_Mode_Choose(0);
	// 不使用密码
	Bluetooth_SetPass(NULL);

//	// STAT 脚为高即发生连接
//	while (!Bluetooth_STAT_IN()) {}
	while (strcmp(Bluetooth_ReceiveString(), "+CONNECTED\r\n") != 0) {}
	printf("---CONNECTED---");
	printf("%s", Bluetooth_ReceiveString());
	Bluetooth_SendStr("hello,world!");

	getchar();
	
	// 断开连接
	Bluetooth_Connect(NULL);

	// 主机模式
	Bluetooth_Mode_Choose(1);
	Bluetooth_Scan();
	Bluetooth_Connect("\x94\x87\xe0\x89\xfd\xcd");
	//Bluetooth_Get_LinkStatus();

	Bluetooth_SendStr("hello,world!");
	printf("%s", Bluetooth_ReceiveString());
}
#endif
