

#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"

#include "usb_istr.h"

/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB低优先级中断请求
 * 输入  ：无		 
 * 输出  ：无
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}

/*
 * 函数名：USB_HP_CAN1_TX_IRQHandler
 * 描述  ：USB高优先级中断请求 或发送中断
 * 输入  ：无		 
 * 输出  ：无
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
	CTR_HP();
}


void USB_EXAMPLE(void)
{
	/*设置USB时钟为48M*/
	Set_USBClock();

	/*配置USB中断(包括SDIO中断)*/
	USB_Interrupts_Config();

	/*USB初始化*/
	USB_Init();

	while (bDeviceState != CONFIGURED);	 //等待配置完成
}

