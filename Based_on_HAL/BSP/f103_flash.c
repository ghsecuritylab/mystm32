#include "f103_flash.h"

/**
  * @brief  Initializes the FLASH SPI and put it into StandBy State (Ready for 
  *         data transfer).
  */
HAL_StatusTypeDef FLASH_SPI_IO_Init(void)
{
  HAL_StatusTypeDef Status = HAL_OK;
  
  GPIO_InitTypeDef  gpioinitstruct = {0};

  /* EEPROM_CS_GPIO Periph clock enable */
  FLASH_SPI_CS_GPIO_CLK_ENABLE();

  /* Configure EEPROM_CS_PIN pin: EEPROM SPI CS pin */
  gpioinitstruct.Pin    = FLASH_SPI_CS_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(FLASH_SPI_CS_GPIO_PORT, &gpioinitstruct);

  /* SPI FLASH Config */
  Status = SPIx_Init();
  
  /* FLASH chip select high */
  FLASH_SPI_CS_HIGH();
  
  return Status;
}

/**
  * @brief  Write a byte on the FLASH SPI.
  * @param  Data: byte to send.
  */
uint8_t FLASH_SPI_IO_WriteByte(uint8_t Data)
{
  /* Send the byte */
  return (SPIx_Write(Data));
}

/**
  * @brief  Read a byte from the FLASH SPI.
  * @retval uint8_t (The received byte).
  */
uint8_t FLASH_SPI_IO_ReadByte(void)
{
  uint8_t data = 0;
  
  /* Get the received data */
  data = SPIx_Read();

  /* Return the shifted data */
  return data;
}

/**
  * @brief  Initializes peripherals used by the Serial FLASH device.
  * @retval FLASH_OK (0x00) if operation is correctly performed, else 
  *         return FLASH_ERROR (0x01).
  */
uint8_t BSP_SERIAL_FLASH_Init(void)
{
  if(FLASH_SPI_IO_Init() != HAL_OK)
  {
    return FLASH_ERROR;
  }
  else
  {
    return FLASH_OK;
  }
}


/**
  * @brief  Read data from FLASH SPI driver
  * @param  MemAddress: Internal memory address
  * @param  pBuffer: Pointer to data buffer
  * @param  BufferSize: Amount of data to be read
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef FLASH_SPI_IO_ReadData(uint32_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  SPIx_Write(FLASH_SPI_CMD_READ);

  /*!< Send ReadAddr high nibble address byte to read from */
  SPIx_Write((MemAddress & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  SPIx_Write((MemAddress& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  SPIx_Write(MemAddress & 0xFF);

  while (BufferSize--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = SPIx_Write(FLASH_SPI_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();

  return HAL_OK;
}

/**
  * @brief  Select the FLASH SPI and send "Write Enable" instruction
  */
void FLASH_SPI_IO_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();
  
  /*!< Send "Write Enable" instruction */
  SPIx_Write(FLASH_SPI_CMD_WREN);
  
  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_HIGH();
  
    /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();
}

/**
  * @brief  Wait response from the FLASH SPI and Deselect the device
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef FLASH_SPI_IO_WaitForWriteEnd(void)
{
  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();
  
  uint8_t flashstatus = 0;

  /*!< Send "Read Status Register" instruction */
  SPIx_Write(FLASH_SPI_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = SPIx_Write(FLASH_SPI_DUMMY_BYTE);

  }
  while ((flashstatus & FLASH_SPI_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();
  
  return HAL_OK;
}

/**
  * @brief  Reads FLASH SPI identification.
  * @retval FLASH identification
  */
uint32_t FLASH_SPI_IO_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  FLASH_SPI_IO_WaitForWriteEnd();

  /*!< Select the FLASH: Chip Select low */
  FLASH_SPI_CS_LOW();

  /*!< Send "RDID " instruction */
  SPIx_Write(FLASH_SPI_CMD_RDID);

  /*!< Read a byte from the FLASH */
  Temp0 = SPIx_Write(FLASH_SPI_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = SPIx_Write(FLASH_SPI_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = SPIx_Write(FLASH_SPI_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval FLASH_OK (0x00) if operation is correctly performed, else 
  *         return FLASH_ERROR (0x01).
  */
uint8_t BSP_SERIAL_FLASH_EraseSector(uint32_t SectorAddr)
{
  /*!< Sector Erase */
  /*!< Select the FLASH  and send "Write Enable" instruction */
  FLASH_SPI_IO_WriteEnable();
  
  /*!< Send Sector Erase instruction */
  FLASH_SPI_IO_WriteByte(FLASH_SPI_CMD_SE);
  /*!< Send SectorAddr high nibble address byte */
  FLASH_SPI_IO_WriteByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  FLASH_SPI_IO_WriteByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  FLASH_SPI_IO_WriteByte(SectorAddr & 0xFF);
	
  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();
	
  /*!< Wait the end of Flash writing and Deselect the FLASH*/
  if(FLASH_SPI_IO_WaitForWriteEnd()!= HAL_OK)
  {
    return FLASH_ERROR;
  }
  else
  {
    return FLASH_OK;
  }
}

/**
  * @brief  Erases the entire FLASH.
  * @retval FLASH_OK (0x00) if operation is correctly performed, else 
  *         return FLASH_ERROR (0x01).
  */
uint8_t BSP_SERIAL_FLASH_EraseBulk(void)
{
  /*!< Bulk Erase */
  /*!< Select the FLASH  and send "Write Enable" instruction */
  FLASH_SPI_IO_WriteEnable();
  
  /*!< Send Bulk Erase instruction  */
  FLASH_SPI_IO_WriteByte(FLASH_SPI_CMD_CE);

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();

  /*!< Wait the end of Flash writing and Deselect the FLASH*/
  if(FLASH_SPI_IO_WaitForWriteEnd()!= HAL_OK)
  {
    return FLASH_ERROR;
  }
  else
  {
    return FLASH_OK;
  }
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size (FLASH_SPI_PAGESIZE).
  * @param  pData: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  uwStartAddress: FLASH's internal address to write to.
  * @param  uwDataSize: number of bytes to write to the FLASH, must be equal
  *         or less than "FLASH_SPI_PAGESIZE" value.
  * @retval FLASH_OK (0x00) if operation is correctly performed, else 
  *         return FLASH_ERROR (0x01).
  */
uint8_t BSP_SERIAL_FLASH_WritePage(uint32_t uwStartAddress, uint8_t* pData, uint32_t uwDataSize)
{
  /*!< Select the FLASH  and send "Write Enable" instruction */
  FLASH_SPI_IO_WriteEnable();

  /*!< Send "Write to Memory " instruction */
  FLASH_SPI_IO_WriteByte(FLASH_SPI_CMD_WRITE);
  /*!< Send uwStartAddress high nibble address byte to write to */
  FLASH_SPI_IO_WriteByte((uwStartAddress & 0xFF0000) >> 16);
  /*!< Send uwStartAddress medium nibble address byte to write to */
  FLASH_SPI_IO_WriteByte((uwStartAddress & 0xFF00) >> 8);
  /*!< Send uwStartAddress low nibble address byte to write to */
  FLASH_SPI_IO_WriteByte(uwStartAddress & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (uwDataSize--)
  {
    /*!< Send the current byte */
    FLASH_SPI_IO_WriteByte(*pData);
    /*!< Point on the next byte to be written */
    pData++;
  }
  
  /*!< Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_HIGH();
  
  /*!< Wait the end of Flash writing */
  if (FLASH_SPI_IO_WaitForWriteEnd()!= HAL_OK)
  {
    return FLASH_ERROR;
  }
  else
  {
    return FLASH_OK;
  }
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pData: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  uwStartAddress: FLASH's internal address to write to.
  * @param  uwDataSize: number of bytes to write to the FLASH.
  * @retval FLASH_OK (0x00) if operation is correctly performed, else 
  *         return FLASH_ERROR (0x01).
  */
uint8_t BSP_SERIAL_FLASH_WriteData(uint32_t uwStartAddress, uint8_t* pData, uint32_t uwDataSize)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0, status = 0;

  Addr = uwStartAddress % FLASH_SPI_PAGESIZE;
  count = FLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  uwDataSize / FLASH_SPI_PAGESIZE;
  NumOfSingle = uwDataSize % FLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< uwStartAddress is FLASH_SPI_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< uwDataSize < FLASH_SPI_PAGESIZE */
    {
      status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, uwDataSize);
    }
    else /*!< uwDataSize > FLASH_SPI_PAGESIZE */
    {
      while (NumOfPage--)
      {
        status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, FLASH_SPI_PAGESIZE);
        uwStartAddress +=  FLASH_SPI_PAGESIZE;
        pData += FLASH_SPI_PAGESIZE;
      }

      status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, NumOfSingle);
    }
  }
  else /*!< uwStartAddress is not FLASH_SPI_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< uwDataSize < FLASH_SPI_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (uwDataSize + uwStartAddress) > FLASH_SPI_PAGESIZE */
      {
        temp = NumOfSingle - count;

        status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, count);
        uwStartAddress +=  count;
        pData += count;

        status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, temp);
      }
      else
      {
        status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, uwDataSize);
      }
    }
    else /*!< uwDataSize > BSP_SERIAL_FLASH_PAGESIZE */
    {
      uwDataSize -= count;
      NumOfPage =  uwDataSize / FLASH_SPI_PAGESIZE;
      NumOfSingle = uwDataSize % FLASH_SPI_PAGESIZE;

      status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, count);
      uwStartAddress +=  count;
      pData += count;

      while (NumOfPage--)
      {
        status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, FLASH_SPI_PAGESIZE);
        uwStartAddress +=  FLASH_SPI_PAGESIZE;
        pData += FLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        status = BSP_SERIAL_FLASH_WritePage(uwStartAddress, pData, NumOfSingle);
      }
    }
  }
  if (status!= HAL_OK)
  {
    return FLASH_ERROR;
  }
  else
  {
    return FLASH_OK;
  }
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pData: pointer to the buffer that receives the data read from the FLASH.
  * @param  uwStartAddress: FLASH's internal address to read from.
  * @param  uwDataSize: number of bytes to read from the FLASH.
  * @retval FLASH_OK (0x00) if operation is correctly performed, else 
  *         return FLASH_ERROR (0x01).
  */
uint8_t BSP_SERIAL_FLASH_ReadData(uint32_t uwStartAddress, uint8_t* pData, uint32_t uwDataSize)
{
  if(FLASH_SPI_IO_ReadData(uwStartAddress, pData, uwDataSize)!= HAL_OK)
  {
    return FLASH_ERROR;
  }
  else
  {
    return FLASH_OK;
  }
}


/**
  * @brief  Reads FLASH identification.
  * @retval FLASH identification
  */
uint32_t BSP_SERIAL_FLASH_ReadID(void)
{
  return(FLASH_SPI_IO_ReadID());
}

