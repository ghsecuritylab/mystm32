
uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 0;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 1;
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  * @retval None
  */
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint16_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++)
  {
    pBuffer[index] = index + Offset;
  }
}

/**
  * @brief  Checks if a buffer has all its values are equal to zero.
  * @param  pBuffer: buffer to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer values are zero
  *         FAILED: At least one value from pBuffer buffer is different from zero.
  */
uint8_t eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    /* In some SD Cards the erased state is 0xFF, in others it's 0x00 */
    if ((*pBuffer != 0xFF) && (*pBuffer != 0x00))
    {
      return 0;
    }

    pBuffer++;
  }

  return 1;
}

#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      10  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)
uint8_t Buffer_Block_Tx[BLOCK_SIZE], Buffer_Block_Rx[BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];

#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      10  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)
uint8_t Buffer_Block_Tx[BLOCK_SIZE], Buffer_Block_Rx[BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
int main()
{
	USART_CONFIG();
	getchar();

	volatile uint8_t EraseStatus = 0, TransferStatus1 = 0, TransferStatus2 = 0;
	SD_Error Status = SD_OK;
	
	if((Status = SD_Init()) != SD_OK)
	{
		printf("SD_INIT_FAILED\n");
	}
	else
	{
		printf("SD_INIT_OK\n");
		if (Status == SD_OK)
		  {
			/* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
		printf("SD_Erase\n");
			Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS));
		  }

		printf("SD_Erase_out,Status=%d\n",Status);
		  if (Status == SD_OK)
		  {
			  printf("SD_ReadMultiBlocks\n");
			Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);

			/* Check if the Transfer is finished */
			Status = SD_WaitReadOperation();

			/* Wait until end of DMA transfer */
			while(SD_GetStatus() != SD_TRANSFER_OK);
		  }

			  printf("SD_ReadMultiBlocks_out,Status=%d\n",Status);
		  /* Check the correctness of erased blocks */
		  if (Status == SD_OK)
		  {
			EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
		  }
		  
		  if(EraseStatus == 1)
		  {
			printf("SD_ERASE_CHECK_OK!\n");
		  }
		  else
		  {
			printf("SD_ERASE_CHECK_FAILED\n");
			  
		  }
		  
		    Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE, 0x320F);

		  if (Status == SD_OK)
		  {
			/* Write block of 512 bytes on address 0 */
			Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE);
			  	if (Status != SD_OK)
				{
					printf("SD_WriteBlock_FAILED,Status=%d\n",Status);
					while(1);
				}
			/* Check if the Transfer is finished */
			Status = SD_WaitWriteOperation();
			if (Status != SD_OK)
				{
					printf("SD_WaitWriteOperation_FAILED,Status=%d\n",Status);
					while(1);
				}
			while(SD_GetStatus() != SD_TRANSFER_OK);
		  }

		  if (Status == SD_OK)
		  {
			/* Read block of 512 bytes from address 0 */
			Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE);
			  if (Status != SD_OK)
				{
					printf("SD_ReadBlock_FAILED,Status=%d\n",Status);
					while(1);
				}
			/* Check if the Transfer is finished */
			Status = SD_WaitReadOperation();
			  if (Status != SD_OK)
				{
					printf("SD_WaitReadOperation_FAILED,Status=%d\n",Status);
					while(1);
				}
			while(SD_GetStatus() != SD_TRANSFER_OK);
		  }

		  /* Check the correctness of written data */
		  if (Status == SD_OK)
		  {
			TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE);
		  }
		  
		  if(TransferStatus1 == 1)
		  {
			printf("Single_block_OK!\n");
		  }
		  else
		  {
			printf("Single_block_FAILED!,Status=%d\n",Status);
			  while(1);
		  }
		  
		  Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x0);

		  if (Status == SD_OK)
		  {
			/* Write multiple block of many bytes on address 0 */
			Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
			/* Check if the Transfer is finished */
			Status = SD_WaitWriteOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		  }

		  if (Status == SD_OK)
		  {
			/* Read block of many bytes from address 0 */
			Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
			/* Check if the Transfer is finished */
			Status = SD_WaitReadOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		  }

		  /* Check the correctness of written data */
		  if (Status == SD_OK)
		  {
			TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
		  }
		  
		  if(TransferStatus2 == 1)
		  {
			printf("Multi block_OK!");

		  }
		  else
		  {
			printf("Multi block_failed!");
			  while(1);
		  }
	}
}