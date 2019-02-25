#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>

void NRF24L01_CAR_RX_EXAMPLE(void);
void NRF24L01_CAR_TX_EXAMPLE(void);

int main()
{
	// USART1 config 115200 8-N-1
	//USART_CONFIG();
	while(1)
	{
		NRF24L01_CAR_TX_EXAMPLE();
		//printf("OVER!!\r\n");
		while(1){}
	}
	return 0;
}
