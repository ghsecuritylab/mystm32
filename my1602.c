#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

// VSS:0V
// VDD:5V
// V0:contrast,0
// A:Background light:high
// K:Background light:low

#define RS_UP		GPIO_ResetBits	(GPIOA,GPIO_Pin_11)
#define RS_DOWN		GPIO_SetBits		(GPIOA,GPIO_Pin_11)
#define RW_UP		GPIO_ResetBits	(GPIOA,GPIO_Pin_12)
#define RW_DOWN		GPIO_SetBits		(GPIOA,GPIO_Pin_12)
#define CE_UP		GPIO_ResetBits	(GPIOB,GPIO_Pin_5)
#define CE_DOWN		GPIO_SetBits		(GPIOB,GPIO_Pin_5)

#define GPIO_D_LOW	GPIOA
#define GPIO_D0	GPIO_Pin_7
#define GPIO_D1	GPIO_Pin_8
#define GPIO_D2	GPIO_Pin_9
#define GPIO_D3	GPIO_Pin_10

#define GPIO_D_HIGH	GPIOB
#define GPIO_D4	GPIO_Pin_0
#define GPIO_D5	GPIO_Pin_1
#define GPIO_D6	GPIO_Pin_10
#define GPIO_D7	GPIO_Pin_11

void write_byte(uint8_t c)
{
	if (c&0x01) GPIO_SetBits(GPIO_D_LOW,GPIO_D0);
		else GPIO_ResetBits(GPIO_D_LOW,GPIO_D0);
	if (c&0x02) GPIO_SetBits(GPIO_D_LOW,GPIO_D1);
		else GPIO_ResetBits(GPIO_D_LOW,GPIO_D1);
	if (c&0x04) GPIO_SetBits(GPIO_D_LOW,GPIO_D2);
		else GPIO_ResetBits(GPIO_D_LOW,GPIO_D2);
	if (c&0x08) GPIO_SetBits(GPIO_D_LOW,GPIO_D3);
		else GPIO_ResetBits(GPIO_D_LOW,GPIO_D3);
	if (c&0x10) GPIO_SetBits(GPIO_D_HIGH,GPIO_D4);
		else GPIO_ResetBits(GPIO_D_HIGH,GPIO_D4);
	if (c&0x20) GPIO_SetBits(GPIO_D_HIGH,GPIO_D5);
		else GPIO_ResetBits(GPIO_D_HIGH,GPIO_D5);
	if (c&0x40) GPIO_SetBits(GPIO_D_HIGH,GPIO_D6);
		else GPIO_ResetBits(GPIO_D_HIGH,GPIO_D6);
	if (c&0x80) GPIO_SetBits(GPIO_D_HIGH,GPIO_D7);
		else GPIO_ResetBits(GPIO_D_HIGH,GPIO_D7);
}

void LCD_WriteData(uint8_t c)
{
	CE_DOWN;
	RS_UP;
	RW_DOWN;
	delay_us(5);
	write_byte(c);
	CE_UP;
	delay_us(5);
	CE_DOWN;
}

void LCD_WriteInstruct(uint8_t c)
{
	CE_DOWN;
	RS_DOWN;
	RW_DOWN;
	delay_us(5);
	write_byte(c);
	CE_UP;
	delay_us(5);
	CE_DOWN;
}

uint8_t LCD_CheckBusy()
{
	write_byte(0xFF);
	CE_DOWN;
	RS_DOWN;
	RW_UP;
	delay_us(5);
	CE_UP;
	delay_us(5);
	return GPIO_D_HIGH->IDR & GPIO_D7;
}

// 光标返回
void QP()
{
	LCD_WriteInstruct(0x01);
	delay_ms(2);
	LCD_WriteInstruct(0x02);
	delay_ms(5);
}

void LCD1602_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;	
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_t.GPIO_Pin = GPIO_D0|GPIO_D1|GPIO_D2|GPIO_D3;
	GPIO_Init(GPIO_D_LOW, &gpio_init_t);
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Pin = GPIO_D4|GPIO_D5|GPIO_D6|GPIO_D7;
	GPIO_Init(GPIO_D_HIGH, &gpio_init_t);

	LCD_WriteInstruct(0x38);
	delay_us(50);
	LCD_WriteInstruct(0x06);	// 光标移动设置
	delay_us(50);
	LCD_WriteInstruct(0x0f);
	delay_us(50);
	QP();
}

// 设置将要写入的地址
void LCD_SetAC(uint8_t line, uint8_t x)
{
	if (line==0)
	{
		LCD_WriteInstruct(0x80+x);
		delay_us(50);
	}
	else if(line==1)
	{
		LCD_WriteInstruct(0x80+0x40+x);
		delay_us(50);
	}
}

void LCD_WriteNum(uint8_t line, uint8_t x, uint8_t c)
{
	if(line==0)
	{
		LCD_WriteInstruct(0x80+x);
		delay_us(50);
		LCD_WriteData(0x30+c);
		delay_us(50);
	}
	else if(line==1)
	{
		LCD_WriteInstruct(0x80+0x40+x);
		delay_us(50);
		LCD_WriteData(0x30+c);
		delay_us(50);		
	}
}

void LCD_WriteChar(uint8_t line, uint8_t x, uint8_t c)
{
	if(line==0)
	{
		LCD_WriteInstruct(0x80+x);
		delay_us(50);
		LCD_WriteData(c);
		delay_us(50);
	}
	else if(line==1)
	{
		LCD_WriteInstruct(0x80+0x40+x);
		delay_us(50);
		LCD_WriteData(c);
		delay_us(50);		
	}
}

void LCD_WriteString(uint8_t line, uint8_t x, char* c)
{
	char *s = c;
	while( *s != '\0' )
	{
		LCD_WriteChar(line, x++, *s);
		s++;
	}
}

void LCD_WriteInteger(uint8_t line, uint8_t x, uint32_t c)
{
	uint8_t l = 1;
	uint32_t t = c;
	while (t>9)
	{
		++l;
		t/=10;
	}
	for (int8_t i = l; i>0; --i)
	{
		LCD_WriteNum(line, x+i-1, c%10);
		c /= 10;
	}
}

void LCD_WriteFloat(uint8_t line, uint8_t x, float f)
{
	uint32_t Temp;
	uint8_t k;
	uint8_t Flag=0;
	uint8_t DW;

	for( k=0; k<16 ; k++ )
	{
		LCD_WriteChar(line, k, ' ');
	}
	
	if( f < 0 )
	{
		LCD_WriteChar(line, x, '-');
		x++;	
		f = -f;	
	}

	if( f <=9999.0 )
	{
		Temp = (uint32_t)(f*1000);
		DW='V';	
	}
	else
	{
		if( 9999.0<f && f<=9999999.0)
		{
			Temp = f;
			DW='K';
		}	
	}
	
	if( 9999999.0<f )
	{
		LCD_WriteString(line, x, "Out of bounds");
		return;	
	}	

		k =	Temp/1000000%10;
		if( (Flag==2)||(k!=0) )
		{
			LCD_WriteNum(line, x, k);
			Flag = 2;
			x++;	
		}

		k =	Temp/100000%10;
		if( (Flag==2)||(k!=0))
		{
			LCD_WriteNum(line, x, k);	
			Flag = 2;
			x++;	
		}

		k =	Temp/10000%10;
		if( (Flag==2)||(k!=0))
		{
			LCD_WriteNum(line, x, k);	
			Flag = 2;
			x++;	
		}

		Flag = 2;
		k =	Temp/1000%10;
		if( (Flag==2)||(k!=0))
		{
			LCD_WriteNum( line , x , k );
			x++;	
		}
		
		LCD_WriteChar(line, x++, '.');
		k =	Temp/100%10;
		LCD_WriteNum(line, x++, k);
		k =	Temp/10%10;
		LCD_WriteNum(line, x++, k);
		k =	Temp%10;
		LCD_WriteNum(line, x++, k);
		LCD_WriteChar(line, x, DW);
}

//int fputc(int ch, FILE* f)
//{
//	LCD_WriteChar(ftell(f)/16%2,ftell(f)%16,ch);
//	return ch;
//}
