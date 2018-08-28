//#include "hw_config.h" 
//#include "usb_lib.h"
//#include "usb_pwr.h"
//#include "usb_istr.h"

//// USB低优先级中断请求
//void USB_LP_CAN1_RX0_IRQHandler(void)
//{
//	USB_Istr();
//}

//// USB高优先级中断请求 或发送中断
//void USB_HP_CAN1_TX_IRQHandler(void)
//{
//	CTR_HP();
//}


//void USB_EXAMPLE(void)
//{
//	// 设置USB时钟为48M
//	Set_USBClock();

//	// 配置USB中断(包括SDIO中断)
//	USB_Interrupts_Config();

//	// USB初始化
//	USB_Init();
//	
//	//等待配置完成
//	while (bDeviceState != CONFIGURED);
//}

