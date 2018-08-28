#include "mylib.h"

#define DHT11_Dout_SCK_APBxClock_FUN	RCC_APB2PeriphClockCmd
#define DHT11_Dout_GPIO_CLK		RCC_APB2Periph_GPIOE
#define DHT11_Dout_GPIO_PORT	GPIOE
#define DHT11_Dout_GPIO_PIN		GPIO_Pin_6

#define DHT11_Dout_0			GPIO_ResetBits(DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN) 
#define DHT11_Dout_1			GPIO_SetBits(DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN) 
#define DHT11_Dout_IN()			GPIO_ReadInputDataBit(DHT11_Dout_GPIO_PORT, DHT11_Dout_GPIO_PIN)

typedef struct
{
	uint8_t  humi_int;		// 湿度的整数部分
	uint8_t  humi_deci;		// 湿度的小数部分
	uint8_t  temp_int;		// 温度的整数部分
	uint8_t  temp_deci;		// 温度的小数部分
	uint8_t  check_sum;		// 校验和

} DHT11_Data_TypeDef;

// 使DHT11-DATA引脚变为上拉输入模式
void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);
}

//使DHT11-DATA引脚变为推挽输出模式
void DHT11_Mode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);
}

// 配置DHT11用到的I/O口
void DHT11_Init ( void )
{
	DHT11_Dout_SCK_APBxClock_FUN(DHT11_Dout_GPIO_CLK, ENABLE);
	DHT11_Mode_Out_PP();
	DHT11_Dout_1;	// 拉高GPIOB10
}

// 从DHT11读取一个字节，MSB先行
uint8_t DHT11_ReadByte ( void )
{
	uint8_t i, temp=0;
	
	for(i=0;i<8;i++)
	{
		// 每bit以50us低电平标置开始，轮询直到从机发出的50us 低电平 结束
		while(DHT11_Dout_IN()==Bit_RESET);

		// DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		// 通过检测 x us后的电平即可区别这两个状态，x 即下面的延时
		// 延时x us 这个延时需要大于数据0持续的时间即可
		delay_us(40);

		if(DHT11_Dout_IN()==Bit_SET)// x us后仍为高电平表示数据“1”
		{
			// 等待数据1的高电平结束
			while(DHT11_Dout_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行
		}
		else	// x us后为低电平表示数据“0”
		{
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	
	return temp;
}


// 一次完整的数据传输为40bit，高位先出
// 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{  
	//输出模式
	DHT11_Mode_Out_PP();
	//主机拉低
	DHT11_Dout_0;
	//延时18ms
	delay_ms(18);

	//总线拉高 主机延时30us
	DHT11_Dout_1; 

	delay_us(30);

	//主机设为输入 判断从机响应信号 
	DHT11_Mode_IPU();

	// 判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行   
	if(DHT11_Dout_IN()==Bit_RESET)
	{
		// 轮询直到从机发出 的80us 低电平 响应信号结束  
		while(DHT11_Dout_IN()==Bit_RESET);

		// 轮询直到从机发出的 80us 高电平 标置信号结束
		while(DHT11_Dout_IN()==Bit_SET);

		// 开始接收数据   
		DHT11_Data->humi_int= DHT11_ReadByte();

		DHT11_Data->humi_deci= DHT11_ReadByte();

		DHT11_Data->temp_int= DHT11_ReadByte();

		DHT11_Data->temp_deci= DHT11_ReadByte();

		DHT11_Data->check_sum= DHT11_ReadByte();

		// 读取结束，引脚改为输出模式
		DHT11_Mode_Out_PP();
		// 主机拉高
		DHT11_Dout_1;

		//检查读取的数据是否正确
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + 
			DHT11_Data->humi_deci + DHT11_Data->temp_int + DHT11_Data->temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	else
		return ERROR;
}

#include <stdio.h>
void DHT11_EXAMPLE(void)
{
	DHT11_Data_TypeDef DHT11_Data;
	DHT11_Init();

	while(1)
	{
		// 调用DHT11_Read_TempAndHumidity读取温湿度，若成功则输出该信息
		if (DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
		{
			printf("humidity=%d.%d%%RH\r\ntemperature=%d.%d`C\r\n",
					DHT11_Data.humi_int,DHT11_Data.humi_deci,
					DHT11_Data.temp_int,DHT11_Data.temp_deci);
		}
		else
		{
			printf("Read DHT11 ERROR!\r\n");
		}
		delay_ms(1000);
	}
}
