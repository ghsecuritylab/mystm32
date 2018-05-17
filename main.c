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

void OLED_EXAMPLE2(void);
int main()
{
	USART_CONFIG();
	while(1)
	{
		while (getchar() != '@');
		
		OLED_EXAMPLE2();
	}
	return 0;
}
