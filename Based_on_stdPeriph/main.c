#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	// USART1 config 115200 8-N-1
	USART_CONFIG();
	while(1)
	{
		printf("OVER!!\r\n");
		while(1){}
	}
	return 0;
}
