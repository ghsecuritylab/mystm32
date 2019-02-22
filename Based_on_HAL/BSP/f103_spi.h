#include "stm32f1xx_hal.h"
HAL_StatusTypeDef  SPIx_Init(void);
uint8_t            SPIx_Write(uint8_t Value);
uint8_t            SPIx_Read(void);
void               SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void               SPIx_WriteData(const uint8_t *Data, uint16_t DataLength);
void               SPIx_ReadData(const uint8_t *Data, uint16_t DataLength);
void               SPIx_Error (void);
void               SPIx_MspInit(SPI_HandleTypeDef *hspi);
#define EVAL_SPIx                               SPI3
#define EVAL_SPIx_CLK_ENABLE()                  __HAL_RCC_SPI3_CLK_ENABLE()

#define EVAL_SPIx_SCK_GPIO_PORT                 GPIOB             /* PB.3*/
#define EVAL_SPIx_SCK_PIN                       GPIO_PIN_3
#define EVAL_SPIx_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define EVAL_SPIx_SCK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()

#define EVAL_SPIx_MISO_MOSI_GPIO_PORT           GPIOB
#define EVAL_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define EVAL_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOB_CLK_DISABLE()
#define EVAL_SPIx_MISO_PIN                      GPIO_PIN_4        /* PB.4*/
#define EVAL_SPIx_MOSI_PIN                      GPIO_PIN_5        /* PB.5*/

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define EVAL_SPIx_TIMEOUT_MAX                   1000
