#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t translate[] = 
{
	1,		2,		3,		'U',
	4,		5,		6,		'D',
	7,		8,		9,		'!',
	'.',	0,		'=',	'B'
};

void FATFS_EXAMPLE(void);
void USB_EXAMPLE(void);
int main()
{
	USART_CONFIG();
	while(1)
	{
		getchar();
		printf("START!!\r\n");
		
		FATFS_EXAMPLE();
		//USB_EXAMPLE();
		while(1){}
	}
	return 0;
}
