#include "f103_spi.h"

/**
  * @brief  M25P FLASH SPI Chip Select macro definition 
  */
#define FLASH_SPI_CS_LOW()       HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)
#define FLASH_SPI_CS_HIGH()      HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)

/**
  * @brief  M25P FLASH SPI Control Interface pins
  */
#define FLASH_SPI_CS_PIN                           GPIO_PIN_1        /* PB.01*/
#define FLASH_SPI_CS_GPIO_PORT                     GPIOB
#define FLASH_SPI_CS_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define FLASH_SPI_CS_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()

/**
  * @brief  M25P,W25Q FLASH SPI supported commands
  */  
#define FLASH_SPI_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define FLASH_SPI_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define FLASH_SPI_CMD_WREN           0x06  /*!< Write enable instruction */
#define FLASH_SPI_CMD_READ           0x03  /*!< Read from Memory instruction */
#define FLASH_SPI_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define FLASH_SPI_CMD_RDID           0x9F  /*!< Read identification */
#ifdef FLASH_SPI_W25Qxx
#define FLASH_SPI_CMD_SE             0x20  /*!< 4K Sector Erase instruction */
#define FLASH_SPI_CMD_BE             0xD8  /*!< 64K Block Erase instruction */
#else
#define FLASH_SPI_CMD_SE             0xD8  /*!< Sector Erase instruction */
#endif
#define FLASH_SPI_CMD_CE             0xC7  /*!< Chip Erase instruction */
#define FLASH_SPI_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag (Busy) */

#define FLASH_SPI_DUMMY_BYTE         0xFF
//#define FLASH_SPI_DUMMY_BYTE         0xA5

#define FLASH_SPI_PAGESIZE           256
#define FLASH_SPI_SECTORSIZE         4096

#define FLASH_SPI_M25P128_ID         0x202018
#define FLASH_SPI_M25P64_ID          0x202017
#define FLASH_SPI_W25X16_ID          0xEF3015
#define FLASH_SPI_W25Q16_ID          0xEF4015
#define FLASH_SPI_W25Q64_ID          0xEF4017
#define FLASH_SPI_W25Q128_ID         0xEF4018

/**
  * @brief  W25X FLASH SPI supported commands
  */  
#define W25X_WriteDisable     0x04
#define W25X_FastReadData     0x0B
#define W25X_FastReadDual     0x3B
#define W25X_BlockErase       0xD8
#define W25X_SectorErase      0x20
#define W25X_PowerDown        0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID         0xAB
#define W25X_ManufactDeviceID 0x90
	 
#define FLASH_OK              0x00
#define FLASH_ERROR           0x01

uint8_t  BSP_SERIAL_FLASH_Init(void);
uint8_t  BSP_SERIAL_FLASH_EraseSector(uint32_t SectorAddr);
uint8_t  BSP_SERIAL_FLASH_EraseBulk(void);
uint8_t  BSP_SERIAL_FLASH_WritePage(uint32_t uwStartAddress, uint8_t* pData, uint32_t uwDataSize);
uint8_t  BSP_SERIAL_FLASH_WriteData(uint32_t uwStartAddress, uint8_t* pData, uint32_t uwDataSize);
uint8_t  BSP_SERIAL_FLASH_ReadData( uint32_t uwStartAddress, uint8_t* pData, uint32_t uwDataSize);
uint32_t BSP_SERIAL_FLASH_ReadID(void);

/* Link function for EEPROM peripheral over SPI */
HAL_StatusTypeDef         FLASH_SPI_IO_Init(void);
uint8_t                   FLASH_SPI_IO_WriteByte(uint8_t Data);
uint8_t                   FLASH_SPI_IO_ReadByte(void);
HAL_StatusTypeDef         FLASH_SPI_IO_ReadData(uint32_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
void                      FLASH_SPI_IO_WriteEnable(void);
HAL_StatusTypeDef         FLASH_SPI_IO_WaitForWriteEnd(void);
uint32_t                  FLASH_SPI_IO_ReadID(void);
