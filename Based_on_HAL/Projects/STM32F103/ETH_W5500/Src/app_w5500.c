#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"

//-----------EXAMPLE-APPLICATIONS-------------

#include "dhcp.h"
void W5500_DHCP(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	// set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	printf(" 网络已完成初始化……\r\n");
	printf(" 野火网络适配板作为DHCP客户端，尝试从DHCP服务器获取IP地址 \r\n");

	while(1)	// 循环执行的函数
	{
		do_dhcp();	// DHCP测试程
	}
}


#include "tcp_demo.h"
void W5500_TCP_SERVER(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	printf(" 野火网络适配版作为TCP 服务器，建立侦听，等待PC作为TCP Client建立连接 \r\n");
	printf(" W5500监听端口为： %d \r\n",local_port);
	printf(" 连接成功后，TCP Client发送数据给W5500，W5500将返回对应数据 \r\n");
	
	while(1)
		do_tcp_server();	// TCP_Client 数据回环测试程
}

#include "tcp_demo.h"
void W5500_TCP_CLIENT(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	printf(" 服务器IP:%d.%d.%d.%d\r\n",
		remote_ip[0],remote_ip[1],
		remote_ip[2],remote_ip[3]);
	printf(" 监听端口:%d \r\n",remote_port);
	printf(" 连接成功后，服务器发送数据给W5500，W5500将返回对应数据 \r\n");
	printf(" 应用程序执行中……\r\n"); 

	while(1)
		do_tcp_client();	// TCP_Client 数据回环测试程序
}

#include "udp_demo.h"
void W5500_UDP_CLIENT(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	printf(" W5500可以和电脑的UDP端口通讯 \r\n");
	printf(" W5500的本地端口为:%d \r\n",local_port);
	printf(" 远端端口为:%d \r\n",remote_port);
	printf(" 连接成功后，PC机发送数据给W5500，W5500将返回对应数据 \r\n");

	while(1)
		do_udp();	// UDP 数据回环测
}

#include "ping.h"
void W5500_PING(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	while(1)
	{
		do_ping();
		if (req>=4)
			break;
	}
	while(1) {}
}

#include "dns.h"
void W5500_DNS(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	printf(" 解析%s的IP\r\n",domain_name); 
	while(1)
	{
		do_dns();	// 域名解析测试程
		delay_ms(1000);
	}
}

#include "dns.h"
#include "smtp.h"
void W5500_SMTP(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	extern  uint8 mail_send_ok;
	printf(" 默认解析smtp.126.com的IP \r\n");
	printf(" 测试前请把收件人邮箱地址改为自己的邮箱 \r\n");
	printf(" 发件人邮箱为:%s \r\n",from);
	printf(" 收件人邮箱为:%s \r\n",to);
	printf(" 应用程序执行中…… \r\n"); 

	mail_message();	
	while(1)// 循环执行的函数 
	{
		do_dns();// 解析126邮箱的服务器域名smtp.126.com
		do_smtp(); // 执行邮件发送子函数
		if(mail_send_ok)
		while(1);
	}
}

#include "dhcp.h"
#include "ntp.h"
void W5500_NTP(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	// set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	printf(" NTP服务器IP为:%d.%d.%d.%d\r\n",ntp_server_ip[0],ntp_server_ip[1],ntp_server_ip[2],ntp_server_ip[3]);
	printf(" NTP服务器端口为:%d \r\n",ntp_port);

	ntp_client_init();	//  NTP初始化

	while(1)
	{
		do_dhcp();	// 从DHCP服务器获取并配置IP地址
		if (dhcp_ok==1)	// 成功获取IP地址
		{
			do_ntp_client();
		}
	}
}

#include "http_client.h"
void W5500_HTTP_CLIENT(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	printf(" YEElink服务器IP为:%d.%d.%d.%d\r\n",
			yeelink_server_ip[0],yeelink_server_ip[1],
			yeelink_server_ip[2],yeelink_server_ip[3]);
	printf(" YEElink服务器端口默认为:%d \r\n",yeelink_port);
	printf(" 我的Yelink ID为:%s \r\n",yeelink_id);
	printf(" 我的Yelink PWD为:%s \r\n",yeelink_pwd);
	printf(" 请登录以上账号观察实时温度变化， 串口也可以查看获取信息\r\n");
	printf(" 应用程序执行中……\r\n");
	while(1)
	{
		do_http_client();
	}
}

#include "http_server.h"
#include "httputil.h"
void W5500_HTTP_SERVER(void)
{
	gpio_for_w5500_config();		// 初始化MCU相关引脚
	reset_w5500();					// 硬复位W5500
	set_w5500_mac();				// 配置MAC地址
	set_w5500_ip();					// 配置IP地址，DHCP 时无需
	socket_buf_init(txsize, rxsize);	// 初始化8个Socket的发送接收缓存大小

	uint8 reboot_flag = 0;
	while(1)	// 循环执行的函数
	{
		do_https();	// Web server测试程序
		if (reboot_flag==1)
		reboot();
	}
}
