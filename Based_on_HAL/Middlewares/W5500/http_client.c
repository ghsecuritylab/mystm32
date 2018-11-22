/**
******************************************************************************
* @file   		http_client.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		http_client 演示函数 
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "http_client.h"
#include "W5500_conf.h"
#include "bsp_dht11.h"
#include "w5500.h"
#include "socket.h"
#include "utility.h"

uint8 temp_rh[2];																										/*temp_rh[0]是湿度 temp_rh[1]是温度*/
DHT11_Data_TypeDef dht11_data;
char  postT[]={
"POST /v1.0/device/351740/sensor/395542/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:1689c460bd674715079cecc4e244042b\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"\r\n"
"{\"value\":xx}\r\n"
};/*xx用来填充温度数值*/

char  postH[]={//提交湿度
"POST /v1.0/device/351740/sensor/395544/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:1689c460bd674715079cecc4e244042b\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"\r\n"
"{\"value\":xx}\r\n"
};/*xx用来填充温度数值*/
char *post[]={postH,postT};

uint8 yeelink_server_ip[4]={118,190,25,51};													/*api.yeelink.net  的ip地址*/
//uint8 yeelink_server_ip[4]={192,168,1,103};													/*api.yeelink.net  的ip地址*/

uint8 yeelink_port=80;																							/*api.yeelink.net  的端口号*/	

/**
*@brief		执行http client主函数
*@param		无
*@return	无
*/
void do_http_client(void)
{	
	static uint8 i=0;
	uint8 ch=SOCK_HUMTEM;
	uint8 buffer[1024]={0};
	uint16 anyport=3000;
	switch(getSn_SR(ch))																							/*获取socket SOCK_TCPS 的状态*/
	{
		case SOCK_INIT:																									/*socket初始化完成*/
			connect(ch, yeelink_server_ip ,yeelink_port);									/*打开socket端口*/
		break;	
		case SOCK_ESTABLISHED: 																					/*socket连接建立*/
			if(getSn_IR(ch) & Sn_IR_CON)  
			{
				setSn_IR(ch, Sn_IR_CON);																		/*清除接收中断标志*/
			}

			if( Read_DHT11(&dht11_data)==SUCCESS)													/*判断是否得到DHT11数据*/

			{
				temp_rh[0]=dht11_data.humi_int;															/*8bit 湿度整数	*/
				temp_rh[1]=dht11_data.temp_int;															/*8bit 温度整数*/
				memcpy(buffer,post[i],strlen(post[i]));	
				buffer[233]=temp_rh[i]/10+0x30;		  
				buffer[234]=temp_rh[i]%10+0x30;
				send(ch,(const uint8 *)buffer,sizeof(buffer));							/*W5500向 Yeelink服务器发送数据*/
				
				if(i==0)
					i=1;
				else 
					i=0;
				
				printf("i= %d ,RH:%d, Temp:%d\r\n", i,temp_rh[0], temp_rh[1]);
			}

			delay_ms(500);
			
		break;
		case SOCK_CLOSE_WAIT:  																					/*socket等待关闭状态*/
			close(ch);  
		break;
		case SOCK_CLOSED: 																							/*socket关闭*/
			if(i==1)printf("Send to Yeelink:OK\r\n");
//       i=0;		
			socket(ch, Sn_MR_TCP,anyport++ , 0x00);   
		break;
		default:
		break;
	}  
}

