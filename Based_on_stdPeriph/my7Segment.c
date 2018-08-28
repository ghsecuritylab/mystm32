#include "mylib.h"

#define SEGMENT_G_W_ClockCmd 	RCC_APB2PeriphClockCmd
#define SEGMENT_G_W 			RCC_APB2Periph_GPIOB

#define SEGMENT_G_D_ClockCmd 	RCC_APB2PeriphClockCmd
#define SEGMENT_G_D 			RCC_APB2Periph_GPIOA

#define SEGMENT_G_D_DotClockCmd RCC_APB2PeriphClockCmd
#define SEGMENT_G_D_Dot 		RCC_APB2Periph_GPIOC

#define GPIO_D			GPIOA
#define GPIO_D_Pin		0xFF

#define GPIO_D_Dot		GPIOC
#define GPIO_D_Dot_Pin	GPIO_Pin_15

#define GPIO_W			GPIOB
#define GPIO_W_Pin0		GPIO_Pin_5
#define GPIO_W_Pin1		GPIO_Pin_12
#define GPIO_W_Pin2		GPIO_Pin_10
#define GPIO_W_Pin3		GPIO_Pin_11

#define GPIO_W_EX		GPIOA
#define GPIO_W_EX_Pin0	GPIO_Pin_11
#define GPIO_W_EX_Pin1	GPIO_Pin_10
#define GPIO_W_EX_Pin2	GPIO_Pin_9
#define GPIO_W_EX_Pin3	GPIO_Pin_8

#define	fastSet(p,i)	{p->BSRR=i;}
#define fastReset(p,i)	{p->BRR=i;}
#define fastToggle(p,i)	{p->ODR^=i;}

void DISPLAY_CONFIG()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	SEGMENT_G_W_ClockCmd(SEGMENT_G_W, ENABLE);
	SEGMENT_G_D_ClockCmd(SEGMENT_G_D, ENABLE);
	SEGMENT_G_D_DotClockCmd(SEGMENT_G_D_Dot, ENABLE);
	
	GPIO_InitTypeDef gpio_init_t;
	gpio_init_t.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_t.GPIO_Speed = GPIO_Speed_50MHz;
	// 初始化小数点
	gpio_init_t.GPIO_Pin = GPIO_D_Dot_Pin;
	GPIO_Init(GPIO_D_Dot, &gpio_init_t);
	// 初始化段选
	gpio_init_t.GPIO_Pin = GPIO_D_Pin;
	GPIO_Init(GPIO_D, &gpio_init_t);
	// 初始化低位位选
	gpio_init_t.GPIO_Pin = GPIO_W_Pin0|GPIO_W_Pin1|GPIO_W_Pin2|GPIO_W_Pin3;
	GPIO_Init(GPIO_W, &gpio_init_t);
	// 未选中
	GPIO_SetBits(GPIO_W, gpio_init_t.GPIO_Pin);
	// 初始化高位位选
	gpio_init_t.GPIO_Pin = GPIO_W_EX_Pin0|GPIO_W_EX_Pin1|GPIO_W_EX_Pin2|GPIO_W_EX_Pin3;
	GPIO_Init(GPIO_W_EX, &gpio_init_t);
	// 未选中
	GPIO_SetBits(GPIO_W_EX, gpio_init_t.GPIO_Pin);
}

char led_table[]={
    0x3f,0x06,0x5b,0x4f,
    0x66,0x6d,0x7d,0x07,
    0x7f,0x6f,0x77,0x7c,
    0x39,0x5e,0x79,0x71
};

// 7-segment low 4 digits
void display(uint16_t number)
{
	if (number<1000)
		for(int j=0;j<0xfff;j++);
	else {
		GPIO_ResetBits(GPIO_W, GPIO_W_Pin0);
		GPIO_SetBits(GPIO_D, led_table[number/1000]);
		for(int j=0;j<0xfff;j++);
		GPIO_ResetBits(GPIO_D, 0xFF);
		GPIO_SetBits(GPIO_W, GPIO_W_Pin0);
	}
	if (number<100)
		for(int j=0;j<0xfff;j++);
	else {
		GPIO_ResetBits(GPIO_W, GPIO_W_Pin1);
		GPIO_SetBits(GPIO_D, led_table[number%1000/100]);
		for(int j=0;j<0xfff;j++);
		GPIO_ResetBits(GPIO_D, 0xFF);
		GPIO_SetBits(GPIO_W, GPIO_W_Pin1);
	}
	if (number<10)
		for(int j=0;j<0xfff;j++);
	else {	
		GPIO_ResetBits(GPIO_W, GPIO_W_Pin2);
		GPIO_SetBits(GPIO_D, led_table[number%100/10]);
		for(int j=0;j<0xfff;j++);
		GPIO_ResetBits(GPIO_D, 0xFF);
		GPIO_SetBits(GPIO_W, GPIO_W_Pin2);
	}	
	GPIO_ResetBits(GPIO_W, GPIO_W_Pin3);
	GPIO_SetBits(GPIO_D, led_table[number%10]);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W, GPIO_W_Pin3);
}

// 7-segment high 4 digits
void displayEX(uint16_t number)
{
	if (number<1000)
		for(int j=0;j<0xfff;j++);
	else {
		GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin0);
		GPIO_SetBits(GPIO_D, led_table[number/1000]);
		for(int j=0;j<0xfff;j++);
		GPIO_ResetBits(GPIO_D, 0xFF);
		GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin0);
	}
	if (number<100)
		for(int j=0;j<0xfff;j++);
	else {
		GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin1);
		GPIO_SetBits(GPIO_D, led_table[number%1000/100]);
		for(int j=0;j<0xfff;j++);
		GPIO_ResetBits(GPIO_D, 0xFF);
		GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin1);
	}
	if (number<10)
		for(int j=0;j<0xfff;j++);
	else {
		GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin2);
		GPIO_SetBits(GPIO_D, led_table[number%100/10]);
		for(int j=0;j<0xfff;j++);
		GPIO_ResetBits(GPIO_D, 0xFF);
		GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin2);
	}
	GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin3);
	GPIO_SetBits(GPIO_D, led_table[number%10]);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin3);
}

void displayStr(uint8_t c1,uint8_t c2,uint8_t c3,uint8_t c4)
{
	GPIO_ResetBits(GPIO_W, GPIO_W_Pin0);
	GPIO_SetBits(GPIO_D, c1);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W, GPIO_W_Pin0);
	
	GPIO_ResetBits(GPIO_W, GPIO_W_Pin1);
	GPIO_SetBits(GPIO_D, c2);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W, GPIO_W_Pin1);
	
	GPIO_ResetBits(GPIO_W, GPIO_W_Pin2);
	GPIO_SetBits(GPIO_D, c3);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W, GPIO_W_Pin2);
	
	GPIO_ResetBits(GPIO_W, GPIO_W_Pin3);
	GPIO_SetBits(GPIO_D, c4);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W, GPIO_W_Pin3);
}

void displayStrEx(uint8_t c1,uint8_t c2,uint8_t c3,uint8_t c4)
{
	GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin0);
	GPIO_SetBits(GPIO_D, c1);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin0);
	
	GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin1);
	GPIO_SetBits(GPIO_D, c2);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin1);
	
	GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin2);
	GPIO_SetBits(GPIO_D, c3);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin2);
	
	GPIO_ResetBits(GPIO_W_EX, GPIO_W_EX_Pin3);
	GPIO_SetBits(GPIO_D, c4);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D, 0xFF);
	GPIO_SetBits(GPIO_W_EX, GPIO_W_EX_Pin3);
}

uint16_t pos_table[] = {GPIO_W_EX_Pin0,GPIO_W_EX_Pin1,GPIO_W_EX_Pin2,GPIO_W_EX_Pin3};
void displayEX_dot(uint8_t pos)
{
	GPIO_ResetBits(GPIO_W_EX, pos_table[pos]);
	GPIO_SetBits(GPIO_D_Dot, GPIO_D_Dot_Pin);
	for(int j=0;j<0xfff;j++);
	GPIO_ResetBits(GPIO_D_Dot, GPIO_D_Dot_Pin);
	GPIO_SetBits(GPIO_W_EX, pos_table[pos]);
}

void display_real(double real)
{
	displayEX(real);		// 高位显示整数部分
	displayEX_dot(3);		// 高位显示小数点
	display((real-(double)(int)real)*10000.0);	// 低位显示小数部分
}
