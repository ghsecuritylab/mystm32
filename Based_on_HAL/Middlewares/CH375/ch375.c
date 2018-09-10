#include <stm32f1xx.h>
#include "ch375.h"

#define CH375_D0_GPIO_Port GPIOA
#define CH375_D1_GPIO_Port GPIOA
#define CH375_D2_GPIO_Port GPIOA
#define CH375_D3_GPIO_Port GPIOA
#define CH375_D4_GPIO_Port GPIOA
#define CH375_D5_GPIO_Port GPIOA
#define CH375_D6_GPIO_Port GPIOA
#define CH375_D7_GPIO_Port GPIOA
#define CH375_WR_GPIO_Port GPIOA
#define CH375_RD_GPIO_Port GPIOA
#define CH375_CS_GPIO_Port GPIOA
#define CH375_A0_GPIO_Port GPIOA
#define CH375_INT_GPIO_Port GPIOA
#define CH375_D0_Pin		GPIO_PIN_1
#define CH375_D1_Pin		GPIO_PIN_2
#define CH375_D2_Pin		GPIO_PIN_3
#define CH375_D3_Pin		GPIO_PIN_4
#define CH375_D4_Pin		GPIO_PIN_5
#define CH375_D5_Pin		GPIO_PIN_6
#define CH375_D6_Pin		GPIO_PIN_7
#define CH375_D7_Pin		GPIO_PIN_8
#define CH375_WR_Pin		GPIO_PIN_9
#define CH375_RD_Pin		GPIO_PIN_10
#define CH375_CS_Pin		GPIO_PIN_11
#define CH375_A0_Pin		GPIO_PIN_12
#define CH375_INT_Pin		GPIO_PIN_13

void Bsp_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
 
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
 
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CH375_D0_GPIO_Port, CH375_D0_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_D1_GPIO_Port, CH375_D1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_D2_GPIO_Port, CH375_D2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_D3_GPIO_Port, CH375_D3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_D4_GPIO_Port, CH375_D4_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_D5_GPIO_Port, CH375_D5_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_D6_GPIO_Port, CH375_D6_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_D7_GPIO_Port, CH375_D7_Pin, GPIO_PIN_SET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CH375_WR_GPIO_Port, CH375_WR_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_RD_GPIO_Port, CH375_RD_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_CS_GPIO_Port, CH375_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CH375_A0_GPIO_Port, CH375_A0_Pin, GPIO_PIN_SET);	
	
  /*Configure GPIO pin : CH375_INT_Pin */
  GPIO_InitStruct.Pin = CH375_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CH375_INT_GPIO_Port, &GPIO_InitStruct);
 
  /*Configure GPIO pins : CH375_D0_Pin CH375_D1_Pin CH375_D2_Pin CH375_D3_Pin 
                           CH375_D4_Pin CH375_D5_Pin CH375_D6_Pin CH375_D7_Pin */
  GPIO_InitStruct.Pin = CH375_D0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CH375_D0_GPIO_Port, &GPIO_InitStruct);	
	
  GPIO_InitStruct.Pin = CH375_D1_Pin;
  HAL_GPIO_Init(CH375_D1_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_D2_Pin;
  HAL_GPIO_Init(CH375_D2_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_D3_Pin;
  HAL_GPIO_Init(CH375_D3_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_D4_Pin;
  HAL_GPIO_Init(CH375_D4_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_D5_Pin;
  HAL_GPIO_Init(CH375_D5_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_D6_Pin;
  HAL_GPIO_Init(CH375_D6_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_D7_Pin;
  HAL_GPIO_Init(CH375_D7_GPIO_Port, &GPIO_InitStruct);
	
  /*Configure GPIO pins : CH375_WR_Pin CH375_RD_Pin CH375_CS_Pin CH375_A0_Pin */
  GPIO_InitStruct.Pin = CH375_WR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CH375_WR_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_RD_Pin;
  HAL_GPIO_Init(CH375_RD_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_CS_Pin;
  HAL_GPIO_Init(CH375_CS_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = CH375_A0_Pin;
  HAL_GPIO_Init(CH375_A0_GPIO_Port, &GPIO_InitStruct);
	
  HAL_NVIC_SetPriority(EXTI4_15_IRQn,0,0);
  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
}


void Bsp_CH375_Write_Cmd(uint8_t cmd ) /* 向CH375的命令端口写入命令*/
{  
	Bsp_DelayUs(5);
	CH375_CS_0;
	CH375_A0_1;
	CH375_DATA(cmd);
	CH375_WR_0;
	Bsp_DelayUs(5);
	CH375_CS_1;
	CH375_WR_1;
}
 
void Bsp_CH375_Write_Data(uint8_t data ) /* 向CH375的命令端口写入数据*/
{  
	Bsp_DelayUs(5);
	CH375_CS_0;
	CH375_A0_0;
	CH375_DATA(data);
	CH375_WR_0;
	Bsp_DelayUs(5);
	CH375_CS_1;
	CH375_WR_1;
	CH375_A0_1;
}
 
uint8_t Bsp_CH375_Read_Data(void)
{
	uint8_t data;
	CH375_CS_0;
	CH375_A0_0;
	CH375_RD_0;
	CH375_DATA_INPUT();
	Bsp_DelayUs(5);
	data = CH375_DATA_PORT->IDR;
	CH375_CS_1;
	CH375_A0_1;
	CH375_RD_1;
	CH375_DATA_OUTPUT();
	return data;
}
 
uint8_t Bsp_CH375_Get_INTStatus(void)
{
	uint8_t status;
	Bsp_CH375_Write_Cmd(CMD_GET_STATUS); 
	status = Bsp_CH375_Read_Data();
	printf("USB interrupt status %d",status);
	return status;
}
 
uint8_t Bsp_CH375_Wait_INT(void)
{
	printf("Wait for USB interrupt...")
	while(CH375_INT_GPIO_Port->IDR & CH375_INT_Pin);
	return Bsp_CH375_Get_INTStatus();
}
 
uint8_t Bsp_CH375_Get_DeviceVersion(void)
{
	uint8_t version;
	Bsp_CH375_Write_Cmd(CMD_GET_IC_VER);
	Bsp_DelayUs(5);
	version = Bsp_CH375_Read_Data();
	version &= 0x7F;
	printf("USB version: 0x%X",version);
	return(version); 
}
 
uint8_t Bsp_CH375_Set_USBMode(USB_Mode_Type mode) {  /* 设置CH375的工作模式 */
	uint8_t i;
	Bsp_CH375_Write_Cmd( CMD_SET_USB_MODE );
	Bsp_CH375_Write_Data(mode);
	CH375_USB_Info.Endp6_Mode = CH375_USB_Info.Endp7_Mode = 0x80;
	Bsp_DelayUs(5);
	for( i=0; i!=10; i++ ) {  /* 等待设置模式操作完成,不超过30uS */
		if( Bsp_CH375_Read_Data()==CMD_RET_SUCCESS ) 
		{
			CH375_USB_Info.USB_Mode = mode;
			printf("Set USB Mode %d success",mode);
			return( 1 );  /* 成功 */
		}
	}
	printf("Set USB Mode faild");
	return(0);  /* CH375出错,例如芯片型号错或者处于串口方式或者不支持 */
}
 
/* 数据同步 */
/* USB的数据同步通过切换DATA0和DATA1实现: 在设备端, CH372/CH375可以自动切换;
   在主机端, 必须由SET_ENDP6和SET_ENDP7命令控制CH375切换DATA0与DATA1.
   主机端的程序处理方法是为SET_ENDP6和SET_ENDP7分别提供一个全局变量,
   初始值均为80H, 每执行一次成功事务后将位6取反, 每执行一次失败事务后将其复位为80H. */
 
/* 主机接收成功后,切换DATA0和DATA1实现数据同步 */
void BSP_CH375_Toggle_Recv(void) 
{  
	Bsp_CH375_Write_Cmd(CMD_SET_ENDP6);
	Bsp_CH375_Write_Data(CH375_USB_Info.Endp6_Mode);
	CH375_USB_Info.Endp6_Mode ^= 0x40;
	Bsp_DelayUs(2);
}
 
/* 主机发送成功后,切换DATA0和DATA1实现数据同步 */
void BSP_CH375_Toggle_Send(void) 
{  
	Bsp_CH375_Write_Cmd(CMD_SET_ENDP7);
	Bsp_CH375_Write_Data(CH375_USB_Info.Endp7_Mode);
	CH375_USB_Info.Endp7_Mode ^= 0x40;
	Bsp_DelayUs(2);
}
 
/* 主机接收失败后,复位设备端的数据同步到DATA0 */
uint8_t Bsp_CH375_ClrStall6(void) 
{  
	Bsp_CH375_Write_Cmd( CMD_CLR_STALL );
	Bsp_CH375_Write_Data( CH375_EndPointNum | 0x80 );  /* 如果设备端不是CH37X芯片,那么需要修改端点号 */
	CH375_USB_Info.Endp6_Mode = 0x80;
	return( Bsp_CH375_Wait_INT() );
}
 
/* 主机发送失败后,复位设备端的数据同步到DATA0 */
uint8_t Bsp_CH375_ClrStall7(void) 
{  
	Bsp_CH375_Write_Cmd( CMD_CLR_STALL );
	Bsp_CH375_Write_Data( CH375_EndPointNum );  /* 如果设备端不是CH37X芯片,那么需要修改端点号 */
	CH375_USB_Info.Endp7_Mode=0x80;
	return( Bsp_CH375_Wait_INT() );
}
 
/* 数据读写, 单片机读写CH372或者CH375芯片中的数据缓冲区 */
 
/* 从CH37X读出数据块 */
uint8_t Bsp_CH375_Read_USBData( uint8_t *buffer ) 
{
	uint8_t  i, len;
	Bsp_CH375_Write_Cmd( CMD_RD_USB_DATA );  /* 从CH375的端点缓冲区读取接收到的数据 */
	len = Bsp_CH375_Read_Data();  /* 后续数据长度 */
	for ( i=0; i!=len; i++ ) 
		*buffer++ = Bsp_CH375_Read_Data();
	return( len );
}
 
/* 向CH37X写入数据块 */
void Bsp_CH375_Write_USBData( uint8_t len, uint8_t *buffer ) 
{  
	Bsp_CH375_Write_Cmd( CMD_WR_USB_DATA7 );  /* 向CH375的端点缓冲区写入准备发送的数据 */
	Bsp_CH375_Write_Data( len );  /* 后续数据长度, len不能大于64 */
	while( len-- ) 
		Bsp_CH375_Write_Data( *buffer++ );
}
 
/* 主机操作 */
 
/* 执行USB事务 */
uint8_t Bsp_CH375_IssueToken(uint8_t endp_and_pid) 
{  
	/* 执行完成后, 将产生中断通知单片机, 如果是USB_INT_SUCCESS就说明操作成功 */
	uint8_t status;
	Bsp_CH375_Write_Cmd(CMD_ISSUE_TOKEN);
	Bsp_CH375_Write_Data(endp_and_pid);  /* 高4位目的端点号, 低4位令牌PID */
	status=Bsp_CH375_Wait_INT();  /* 等待CH375操作完成 */
	if ( status!=USB_INT_SUCCESS && status!=USB_INT_USB_READY && ( endp_and_pid&0xF0) == (CH375_EndPointNum << 4) ) /* 操作失败,如果设备端不是CH37X芯片,那么需要修改端点号 */
	{  
		if ( (endp_and_pid&0x0F) == DEF_USB_PID_OUT ) 
			Bsp_CH375_ClrStall7();  /* 复位设备端接收 */
		else if ( (endp_and_pid&0x0F) == DEF_USB_PID_IN ) 
			Bsp_CH375_ClrStall6();  /* 复位设备端发送 */
	}
	return( status );
}
 
/* 主机发送 */
void Bsp_CH375_Host_Send( uint8_t len, uint8_t *buffer ) 
{  
	Bsp_CH375_Write_USBData( len, buffer );
	BSP_CH375_Toggle_Send();
	if ( Bsp_CH375_IssueToken( ( CH375_EndPointNum << 4 ) | DEF_USB_PID_OUT )!=USB_INT_SUCCESS ) 
		Error_Handler(1);  /* 如果设备端不是CH37X芯片,那么需要修改端点号 */
}
 
/* 主机接收, 返回长度 */
uint8_t Bsp_CH375_Host_Recv( uint8_t *buffer ) 
{
	BSP_CH375_Toggle_Recv();
	if ( Bsp_CH375_IssueToken( ( CH375_EndPointNum << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS ) 
		Error_Handler();  /* 如果设备端不是CH37X芯片,那么需要修改端点号 */
	return( Bsp_CH375_Read_USBData( buffer ) );
}
 
 /* 从设备端获取描述符 */
uint8_t Bsp_CH375_Get_USBDescr( uint8_t type ) 
{ 
	uint8_t status;
	Bsp_CH375_Write_Cmd( CMD_GET_DESCR );
	Bsp_CH375_Write_Data( type );  /* 描述符类型, 只支持1(设备)或者2(配置) */
	status = Bsp_CH375_Wait_INT();  /* 等待CH375操作完成 */
	return( status );
}
/* 设置设备端的USB地址 */
uint8_t Bsp_CH375_SetAddr( uint8_t addr ) 
{  
	uint8_t status;
	Bsp_CH375_Write_Cmd( CMD_SET_ADDRESS );  /* 设置USB设备端的USB地址 */
	Bsp_CH375_Write_Data( addr );  /* 地址, 从1到127之间的任意值, 常用2到20 */
	status=Bsp_CH375_Wait_INT();  /* 等待CH375操作完成 */
	/* 操作成功 */
	if ( status==USB_INT_SUCCESS ) 
	{  
		Bsp_CH375_Write_Cmd( CMD_SET_USB_ADDR );  /* 设置USB主机端的USB地址 */
		Bsp_CH375_Write_Data( addr );  /* 当目标USB设备的地址成功修改后,应该同步修改主机端的USB地址 */
	}
	return( status );
}
 
/* 设置设备端的USB配置 */
uint8_t Bsp_CH375_SetConfig( uint8_t cfg ) 
{  
	CH375_USB_Info.Endp6_Mode = CH375_USB_Info.Endp7_Mode = 0x80;  /* 复位USB数据同步标志 */
	Bsp_CH375_Write_Cmd( CMD_SET_CONFIG );  /* 设置USB设备端的配置值 */
	Bsp_CH375_Write_Data( cfg );  /* 此值取自USB设备的配置描述符中 */
	return( Bsp_CH375_Wait_INT() );  /* 等待CH375操作完成 */
}
 
void Bsp_CH375_Set_Retry(uint8_t num)
{	
	Bsp_CH375_Write_Cmd( CMD_SET_RETRY); 
	Bsp_CH375_Write_Data( 0x25);
	Bsp_CH375_Write_Data( num);
	Bsp_DelayUs(2);
}
 
uint8_t Bsp_CH375_Reset_Device(void)
{
	/* USB规范中未要求在USB设备插入后必须复位该设备,但是计算机的WINDOWS总是这样做,所以有些USB设备也要求在插入后必须先复位才能工作 */
	if(Bsp_CH375_Set_USBMode( RstBUS_Host ) == 0)return 0;  /* 复位USB设备,CH375向USB信号线的D+和D-输出低电平 */
	Bsp_DelayMs(10);
	if(Bsp_CH375_Set_USBMode( AutoSOF_Host ) == 0)return 0;  /* 结束复位 */
	Bsp_DelayMs(100);
	return 1;
}
