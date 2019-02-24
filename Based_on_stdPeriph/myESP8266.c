#include "mylib.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#if 0

#if defined (__CC_ARM)
#pragma anon_unions
#endif

//ESP8266 数据类型定义
typedef enum{
	STA,
	AP,
	STA_AP
} NetMode_TypeDef;

typedef enum{
	TCP,
	UDP,
} NetPro_TypeDef;

typedef enum {
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ID_NO_TypeDef;

typedef enum {
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} AP_PsdMode_TypeDef;

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

#define ESP8266_CH_PD_APBxClkCmd		RCC_APB2PeriphClockCmd
#define ESP8266_CH_PD_CLK				RCC_APB2Periph_GPIOB
#define ESP8266_CH_PD_PORT				GPIOB
#define ESP8266_CH_PD_PIN				GPIO_Pin_8

#define ESP8266_RST_APBxClkCmd			RCC_APB2PeriphClockCmd
#define ESP8266_RST_CLK					RCC_APB2Periph_GPIOB
#define ESP8266_RST_PORT				GPIOB
#define ESP8266_RST_PIN					GPIO_Pin_9

#define ESP8266_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
#define ESP8266_USART_GPIO_CLK			RCC_APB2Periph_GPIOB
#define ESP8266_USART_TX_PORT			GPIOB
#define ESP8266_USART_TX_PIN			GPIO_Pin_10
#define ESP8266_USART_RX_PORT			GPIOB
#define ESP8266_USART_RX_PIN			GPIO_Pin_11

#define ESP8266_USART_APBxClkCmd		RCC_APB1PeriphClockCmd
#define ESP8266_USART_CLK				RCC_APB1Periph_USART3
#define ESP8266_USARTx					USART3
#define ESP8266_USART_BAUD_RATE			115200
#define ESP8266_USART_IRQ				USART3_IRQn
#define ESP8266_USART_INT_FUN			USART3_IRQHandler

#define ESP8266_CH_ENABLE()			GPIO_SetBits(ESP8266_CH_PD_PORT, ESP8266_CH_PD_PIN)
#define ESP8266_CH_DISABLE()		GPIO_ResetBits(ESP8266_CH_PD_PORT, ESP8266_CH_PD_PIN)
#define ESP8266_RST_HIGH_LEVEL()	GPIO_SetBits(ESP8266_RST_PORT, ESP8266_RST_PIN)
#define ESP8266_RST_LOW_LEVEL()		GPIO_ResetBits(ESP8266_RST_PORT, ESP8266_RST_PIN)

void	ESP8266_Init				(void);
bool	ESP8266_Cmd					(char* cmd, char* reply1, char* reply2, uint32_t waittime);
bool	ESP8266_Net_Mode_Choose		(NetMode_TypeDef Mode);
bool	ESP8266_JoinAP				(char* pSSID, char* pPassWord);
bool	ESP8266_CreateAP			(char* pSSID, char* pPassWord, AP_PsdMode_TypeDef PsdMode);
bool	ESP8266_Enable_MultiConn	(FunctionalState multiConnection);
bool	ESP8266_Link_Server			(NetPro_TypeDef proto, char* ip, char* ComNum, ID_NO_TypeDef id);
bool	ESP8266_StartOrShutServer	(FunctionalState Mode, char* pPortNum, char* pTimeOver);
uint8_t	ESP8266_Get_LinkStatus		(void);
uint8_t	ESP8266_Get_IdLinkStatus	(void);
uint8_t ESP8266_GetDHCP_IP			(char* buffer_ip, uint8_t buffer_len);
bool	ESP8266_UnvarnishSend		(FunctionalState EnUnvarnishTx);
bool	ESP8266_SendString			(FunctionalState EnUnvarnishTx, char* pStr, uint32_t ulStrLength, ID_NO_TypeDef ucId);
char*	ESP8266_ReceiveString		(FunctionalState EnUnvarnishTx);
uint8_t ESP8266_GetConnectedIP		(char* buffer_connectedIP);
uint8_t ESP8266_SetIP				(char* buffer_ip);
void	ESP8266_GPIO_Config			(void);
void	ESP8266_USART_Config		(void);

struct STRUCT_USARTx_Frame strEsp8266_Frame_Record = { 0 };

// 初始化ESP8266用到的GPIO引脚
void ESP8266_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 配置 CH_PD 引脚
	ESP8266_CH_PD_APBxClkCmd(ESP8266_CH_PD_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_CH_PD_PORT, &GPIO_InitStructure);

	// 配置 RST 引脚
	ESP8266_RST_APBxClkCmd(ESP8266_RST_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ESP8266_RST_PIN;
	GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStructure);
}

// 初始化ESP8266用到的 USART
void ESP8266_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART clock */
	ESP8266_USART_APBxClkCmd(ESP8266_USART_CLK, ENABLE);
	ESP8266_USART_GPIO_APBxClkCmd(ESP8266_USART_GPIO_CLK, ENABLE);
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = ESP8266_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_USART_TX_PORT, &GPIO_InitStructure);
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = ESP8266_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ESP8266_USART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = ESP8266_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(ESP8266_USARTx, &USART_InitStructure);
	
	/* 中断配置 */
	USART_ITConfig (ESP8266_USARTx, USART_IT_RXNE, ENABLE); // 使能串口接收中断
	USART_ITConfig (ESP8266_USARTx, USART_IT_IDLE, ENABLE); // 使能串口总线空闲中断

	NVIC_CONFIG(ESP8266_USART_IRQ, 0, 1);
	
	USART_Cmd(ESP8266_USARTx, ENABLE);
}

// 串口接收中断处理函数，将 ESP8266 发送来的数据保存到 Data_RX_BUF 中
void ESP8266_USART_INT_FUN(void)
{	
	uint8_t c;
	if (USART_GetITStatus(ESP8266_USARTx, USART_IT_RXNE) == SET)	// 接收中断
	{
		c = USART_ReceiveData(ESP8266_USARTx);
		if (strEsp8266_Frame_Record.InfoBit.FrameLength < (RX_BUF_MAX_LEN - 1))	// 预留1个字节写结束符
			strEsp8266_Frame_Record.Data_RX_BUF[strEsp8266_Frame_Record.InfoBit.FrameLength++] = c;
	}
	if (USART_GetITStatus(ESP8266_USARTx, USART_IT_IDLE) == SET)	// 数据帧接收完毕
	{
		strEsp8266_Frame_Record.InfoBit.FrameFinishFlag = 1;
		c = USART_ReceiveData(ESP8266_USARTx);	// 由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	}
}

// 向ESP8266发送数据
void ESP8266_SendStr(char* str)
{
	for(uint32_t i = 0; str[i]; ++i)
	{			
		USART_SendData(ESP8266_USARTx, str[i]);
		while(USART_GetFlagStatus(ESP8266_USARTx, USART_FLAG_TXE) == RESET);
	}
}


#include <string.h>
// 向ESP8266发送AT指令，reply1,reply2 为两种期待的响应字符串，0 不需响应
bool ESP8266_Cmd(char* cmd, char* reply1, char* reply2, uint32_t waittime)
{
	strEsp8266_Frame_Record.InfoBit.FrameLength = 0;	// 从新开始接收新的数据包

	ESP8266_SendStr(cmd);
	ESP8266_SendStr("\r\n");

	if ((reply1 == 0) && (reply2 == 0))	// 不需要接收数据
		return true;

	delay_ms(waittime);
	
	strEsp8266_Frame_Record.Data_RX_BUF[strEsp8266_Frame_Record.InfoBit.FrameLength] = '\0';
	printf(">>> %s", strEsp8266_Frame_Record.Data_RX_BUF);
  
	if ((reply1 != 0) && (reply2 != 0))
		return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply1) || 
				(bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply2)); 
	else if (reply1 != 0)
		return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply1));
	else
		return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply2));
}

// 选择WF-ESP8266模块的工作模式
bool ESP8266_Net_Mode_Choose(NetMode_TypeDef Mode)
{
	switch (Mode)
	{
	case STA:
		return ESP8266_Cmd("AT+CWMODE_CUR=1", "OK", "no change", 2500);
	case AP:
		return ESP8266_Cmd("AT+CWMODE_CUR=2", "OK", "no change", 2500);
	case STA_AP:
		return ESP8266_Cmd("AT+CWMODE_CUR=3", "OK", "no change", 2500);
	default:
		return false;
	}
}

// 连接外部WiFi，pSSID，WiFi名称字符串，pPassWord，WiFi密码字符串
bool ESP8266_JoinAP(char* pSSID, char* pPassWord)
{
	char cCmd[120];
	sprintf(cCmd, "AT+CWJAP_CUR=\"%s\",\"%s\"", pSSID, pPassWord );
	return ESP8266_Cmd (cCmd, "OK", 0, 5000);
}

// 创建WiFi热点，pSSID，WiFi名称字符串，pPassWord，WiFi密码字符串，enunPsdMode，WiFi加密方式代号字符串
bool ESP8266_CreateAP(char* pSSID, char* pPassWord, AP_PsdMode_TypeDef PsdMode )
{
	char cCmd [120];
	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%d", pSSID, pPassWord, PsdMode );
	return ESP8266_Cmd(cCmd, "OK", 0, 1000);
}

// 配置是否多连接（最多同时5个）默认为单连接；
// 只有非透传模式 (AT+CIPMODE=0)，才能设置为多连接；
// 必须在没有连接建立的情况下，设置连接模式
// 如果建立了TCP服务器，想切换为单连接，必须关闭服务器 (AT+CIPSERVER=0)
// 服务器仅支持多连接。
bool ESP8266_Enable_MultiConn(FunctionalState multiConnection)
{
	char cStr [20];
	sprintf (cStr, "AT+CIPMUX=%d", (multiConnection?1:0));
	return ESP8266_Cmd(cStr, "OK", 0, 500 );
}

// 连接外部服务器，proto，网络协议，ip，服务器IP字符串，ComNum，服务器端口字符串，id，连接ID
bool ESP8266_Link_Server(NetPro_TypeDef proto, char* ip, char* ComNum, ID_NO_TypeDef id)
{
	char cStr[100] = { 0 }, cCmd[120];
	switch (proto)
	{
	case TCP:
		sprintf(cStr, "\"TCP\",\"%s\",%s", ip, ComNum);
		break;
	case UDP:
		sprintf(cStr, "\"UDP\",\"%s\",%s", ip, ComNum);
		break;
	default:
		break;
	}
	if (id == Single_ID_0)
		sprintf(cCmd, "AT+CIPSTART=%s", cStr);
	else
		sprintf(cCmd, "AT+CIPSTART=%d,%s", id, cStr);
	return ESP8266_Cmd(cCmd, "OK", "ALREADY CONNECT", 4000);
}

// 开启或关闭服务器模式，Mode，开启/关闭，pPortNum，服务器端口号字符串，pTimeOver，服务器超时时间字符串，单位：秒
bool ESP8266_StartOrShutServer(FunctionalState Mode, char* pPortNum, char* pTimeOver)
{
	char cCmd1[120], cCmd2[120];
	if (Mode)
	{
		sprintf(cCmd1, "AT+CIPSERVER=1,%s", pPortNum);
		sprintf(cCmd2, "AT+CIPSTO=%s", pTimeOver);
		return (ESP8266_Cmd ( cCmd1, "OK", 0, 500) && ESP8266_Cmd(cCmd2, "OK", 0, 500));
	}
	else
	{
		sprintf(cCmd1, "AT+CIPSERVER=0,%s", pPortNum);
		return ESP8266_Cmd(cCmd1, "OK", 0, 500);
	}
}

// 获取 ESP8266 的连接状态，较适合单端口时使用
uint8_t ESP8266_Get_LinkStatus(void)
{
	if (ESP8266_Cmd("AT+CIPSTATUS", "OK", 0, 500))
	{
		if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "STATUS:2\r\n"))
			return 2;	// 已连接AP且获得ip
		else if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "STATUS:3\r\n"))
			return 3;	// 建立连接
		else if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "STATUS:4\r\n"))
			return 4;	// 失去连接
		else if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "STATUS:5\r\n"))
			return 5;	// 未连接AP
	}
	return 0;	//获取状态失败
}

// 获取 WF-ESP8266 的端口（Id）连接状态，较适合多端口时使用
// 返回端口（Id）的连接状态，低5位为有效位，分别对应Id5~0，某位
// 若置1表示该Id建立了连接，若被清0表示该Id未建立连接
uint8_t ESP8266_Get_IdLinkStatus (void)
{
	uint8_t ucIdLinkStatus = 0x00;
	if (ESP8266_Cmd("AT+CIPSTATUS", "OK", 0, 500))
	{
		if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:0,"))
			ucIdLinkStatus |= 0x01;
		else 
			ucIdLinkStatus &= ~ 0x01;

		if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:1,"))
			ucIdLinkStatus |= 0x02;
		else 
			ucIdLinkStatus &= ~ 0x02;

		if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:2,"))
			ucIdLinkStatus |= 0x04;
		else 
			ucIdLinkStatus &= ~ 0x04;

		if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:3,"))
			ucIdLinkStatus |= 0x08;
		else 
			ucIdLinkStatus &= ~ 0x08;
		
		if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:4,"))
			ucIdLinkStatus |= 0x10;
		else
			ucIdLinkStatus &= ~ 0x10;	
	}
	return ucIdLinkStatus;
}

// 获取 F-ESP8266 的 DHCP IP
uint8_t ESP8266_GetDHCP_IP (char* buffer_ip, uint8_t buffer_len)
{
	uint8_t i;
	char *p;
	ESP8266_Cmd("AT+CIFSR", "OK", 0, 500);
	p = strstr(strEsp8266_Frame_Record.Data_RX_BUF, "APIP,\"");
	if (p)
		p += 6;
	else
		return 0;
	for (i=0; i < buffer_len; i++)
	{
		buffer_ip[i] = p[i];

		if (buffer_ip[i] == '\"')
		{
			buffer_ip[i] = '\0';
			break;
		}
	}
	return 1;
}

// 进入或退出透传模式
bool ESP8266_UnvarnishSend(FunctionalState EnUnvarnishTx)
{
	// WF-ESP8266 退出透传模式
	if (EnUnvarnishTx == DISABLE)
	{
		delay_ms(1000);	// 等待数据发完
		ESP8266_SendStr("+++");	// 退出透传不加回车换行也没有响应
		delay_ms(500);	// 等待退出透传
		return true;
	}
	else // WF-ESP8266 进入透传发送
	{
		if (!ESP8266_Cmd("AT+CIPMODE=1", "OK", 0, 500))
			return false;
		return ESP8266_Cmd("AT+CIPSEND", "OK", ">", 500);
	}
}

// 发送字符串，EnUnvarnishTx，声明是否已使能了透传模式，pStr，要发送的字符串
// ulStrLength，要发送的字符串的字节数，ucId，哪个ID发送的字符串
bool ESP8266_SendString(FunctionalState EnUnvarnishTx, char* pStr, uint32_t ulStrLength, ID_NO_TypeDef ucId)
{
	char cStr[20];
	if (EnUnvarnishTx)
	{
		// 透传模式直接发送
		ESP8266_SendStr(pStr);
		return true;
	}
	else
	{
		if (ucId == Single_ID_0)
			sprintf(cStr, "AT+CIPSEND=%d", ulStrLength + 2);
		else
			sprintf(cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2);
		ESP8266_Cmd(cStr, "> ", 0, 100);
		return ESP8266_Cmd(pStr, "SEND OK", 0, 500);
	}
}

// WF-ESP8266模块接收字符串，EnUnvarnishTx，声明是否已使能了透传模式，返回接收到的字符串首地址
char* ESP8266_ReceiveString(FunctionalState EnUnvarnishTx)
{
	strEsp8266_Frame_Record.InfoBit.FrameLength = 0;
	strEsp8266_Frame_Record.InfoBit.FrameFinishFlag = 0;

	while (!strEsp8266_Frame_Record.InfoBit.FrameFinishFlag);
	strEsp8266_Frame_Record.Data_RX_BUF[strEsp8266_Frame_Record.InfoBit.FrameLength] = '\0';
	
	if (EnUnvarnishTx) // 透传模式直接接收
		return strEsp8266_Frame_Record.Data_RX_BUF;
	else
	{
		if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+IPD"))
			return strEsp8266_Frame_Record.Data_RX_BUF;
		else
			return NULL;
	}
}

// 查询已接入设备的IP
uint8_t ESP8266_GetConnectedIP(char* buffer_connectedIP)
{
	uint8_t i, len;
	char *p, *p2;
	ESP8266_Cmd("AT+CWLIF", "OK", 0, 100);

	p2 = strstr(strEsp8266_Frame_Record.Data_RX_BUF, ",");

	if (p2)
	{
		p = strstr(strEsp8266_Frame_Record.Data_RX_BUF, "AT+CWLIF\r\r\n") + 11;
		len = p2 - p;
	}
	else
		return 0;

	for (i = 0; i < len; i++)
		buffer_connectedIP[i] = p[i];

	buffer_connectedIP[len] = '\0';
	return 1;
}


// 指定模块的IP
uint8_t ESP8266_SetIP(char* buffer_ip)
{
	char cCmd [30];
	sprintf (cCmd, "AT+CIPAP=\"%s\"", buffer_ip);
	if (ESP8266_Cmd(cCmd, "OK", 0, 5000))
		return 1;
	else 
		return 0;
}

#define	macUser_ESP8266_SSID			"304"			/* 要连接的热点的名称 */
#define	macUser_ESP8266_PSK				"password"		/* 要连接的热点的密钥 */
#define	macUser_ESP8266_TcpServer_IP	"192.168.0.223"	/* 要连接的服务器的 IP */
#define	macUser_ESP8266_TcpServer_Port	"5377"			/* 要连接的服务器的端口 */

void ESP8266_EXAMPLE(void)
{
	uint8_t count;
	ESP8266_GPIO_Config(); 
	ESP8266_USART_Config();
	ESP8266_RST_HIGH_LEVEL();
	ESP8266_CH_DISABLE();

	printf("configuring...\r\n");
	ESP8266_CH_ENABLE();
	
	// 对WF-ESP8266模块进行AT测试启动
	delay_ms(1000);
	for (count=0; count < 10; ++count)
	{
		if (ESP8266_Cmd("AT", "OK", NULL, 500))
			break;
		// 重启WF-ESP8266模块
//		macESP8266_RST_LOW_LEVEL();
//		delay_ms(500); 
//		macESP8266_RST_HIGH_LEVEL();
		// 或者
		ESP8266_Cmd ("AT+RST", "OK", "ready", 2500);
	}
	if (count==10)
	{
		printf("config failed...\r\n");
		return;
	}
	
	// 查看固件版本
	ESP8266_Cmd("AT+GMR", "OK", NULL, 500);
	
	// STATION 模式
	ESP8266_Net_Mode_Choose(STA);

	// 加入AP
	while (!ESP8266_JoinAP(macUser_ESP8266_SSID, macUser_ESP8266_PSK));	

	// 连接服务
	while (!ESP8266_Link_Server(TCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0));
	
	// 进入透传模式
	while (!ESP8266_UnvarnishSend(ENABLE));

	printf("config successfully\r\n");

	while (1)
	{
		// 发送数据
		ESP8266_SendString(ENABLE, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\nABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\nABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n", 0, Single_ID_0);
		
		// 发送间隔
		//delay_ms(1000);
		
		// 接下来要发送指令，先退出透传模式
		ESP8266_UnvarnishSend(DISABLE);
		
		// 失去连接后重连
		if (ESP8266_Get_LinkStatus() == 4)
		{
			printf("reconnecting...\r\n" );
			while (!ESP8266_Link_Server(TCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0));
			printf("reconnect successfully\r\n");
		}
		// 进入透传模式
		while (!ESP8266_UnvarnishSend(ENABLE));
	}
}
#endif
