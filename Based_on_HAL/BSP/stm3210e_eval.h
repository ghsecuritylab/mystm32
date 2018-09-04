/**
  ******************************************************************************
  * @file    stm3210e_eval.h
  * @author  MCD Application Team
  * @version V7.0.0
  * @date    14-April-2017
  * @brief   This file contains definitions for STM3210E_EVAL's LEDs, 
  *          push-buttons and COM ports hardware resources.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM3210E_EVAL_H
#define __STM3210E_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif 

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM3210E_EVAL
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/** @addtogroup STM3210E_EVAL_Common
  * @{
  */ 

/** @defgroup STM3210E_EVAL_Exported_Types STM3210E EVAL Exported Types
  * @{
  */

/**
 * @brief LED Types Definition
 */
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,

  LED_GREEN  = LED1,
  LED_ORANGE = LED2,
  LED_RED    = LED3,
  LED_BLUE   = LED4

} Led_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1,
  BUTTON_MODE_EVT  = 2

} ButtonMode_TypeDef;

/**
 * @brief COM Types Definition
 */
typedef enum 
{
  COM1 = 0,
  COM2 = 1

} COM_TypeDef;  

/**
  * @}
  */ 

/** @defgroup STM3210E_EVAL_Exported_Constants STM3210E EVAL Exported Constants
  * @{
  */ 

/** 
  * @brief  Define for STM3210E_EVAL board  
  */ 
#if !defined (USE_STM3210E_EVAL)
 #define USE_STM3210E_EVAL
#endif
  
/** @defgroup STM3210E_EVAL_LED STM3210E EVAL LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_PIN_6             /* PF.06*/
#define LED1_GPIO_PORT                   GPIOF
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOF_CLK_DISABLE()
  
#define LED2_PIN                         GPIO_PIN_7             /* PF.07*/
#define LED2_GPIO_PORT                   GPIOF
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOF_CLK_DISABLE()


#define LED3_PIN                         GPIO_PIN_8            /* PF.08*/
#define LED3_GPIO_PORT                   GPIOF
#define LED3_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOF_CLK_DISABLE()


#define LED4_PIN                         GPIO_PIN_9            /* PF.09*/
#define LED4_GPIO_PORT                   GPIOF
#define LED4_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED4_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOF_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__LED__) do { if ((__LED__) == LED1) LED1_GPIO_CLK_ENABLE(); else\
                                           if ((__LED__) == LED2) LED2_GPIO_CLK_ENABLE(); else \
                                           if ((__LED__) == LED3) LED3_GPIO_CLK_ENABLE(); else\
                                           if ((__LED__) == LED4) LED4_GPIO_CLK_ENABLE();} while(0)

#define LEDx_GPIO_CLK_DISABLE(__LED__)   (((__LED__) == LED1) ? LED1_GPIO_CLK_DISABLE() :\
                                          ((__LED__) == LED2) ? LED2_GPIO_CLK_DISABLE() :\
                                          ((__LED__) == LED3) ? LED3_GPIO_CLK_DISABLE() :\
                                          ((__LED__) == LED4) ? LED4_GPIO_CLK_DISABLE() : 0 )

/**
  * @}
  */

#define KEY_BUTTON_PIN                      GPIO_PIN_8             /* PG.08*/
#define KEY_BUTTON_GPIO_PORT                GPIOG
#define KEY_BUTTON_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOG_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOG_CLK_DISABLE()
#define KEY_BUTTON_EXTI_IRQn                EXTI9_5_IRQn

/** @addtogroup STM3210E_EVAL_COM
  * @{
  */
#define COMn                             2

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK_ENABLE()           __HAL_RCC_USART1_CLK_ENABLE()
#define EVAL_COM1_CLK_DISABLE()          __HAL_RCC_USART1_CLK_DISABLE()

#define EVAL_COM1_TX_PIN                 GPIO_PIN_9             /* PA.09*/
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_COM1_TX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()

#define EVAL_COM1_RX_PIN                 GPIO_PIN_10             /* PA.10*/
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_COM1_RX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()

#define EVAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */ 
#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK_ENABLE()           __HAL_RCC_USART2_CLK_ENABLE()
#define EVAL_COM2_CLK_DISABLE()          __HAL_RCC_USART2_CLK_DISABLE()

#define EVAL_COM2_TX_PIN                 GPIO_PIN_2             /* PA.02*/
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_COM2_TX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()

#define EVAL_COM2_RX_PIN                 GPIO_PIN_3             /* PA.03*/
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_COM2_RX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()

#define EVAL_COM2_IRQn                   USART2_IRQn

#define COMx_CLK_ENABLE(__INDEX__)              do { if((__INDEX__) == COM1) EVAL_COM1_CLK_ENABLE(); else\
                                                     if((__INDEX__) == COM2) EVAL_COM2_CLK_ENABLE();} while(0)

#define COMx_CLK_DISABLE(__INDEX__)             (((__INDEX__) == COM1) ? EVAL_COM1_CLK_DISABLE() :\
                                                 ((__INDEX__) == COM2) ? EVAL_COM2_CLK_DISABLE() : 0)

#define COMx_TX_GPIO_CLK_ENABLE(__INDEX__)      do { if((__INDEX__) == COM1) EVAL_COM1_TX_GPIO_CLK_ENABLE(); else\
                                                     if((__INDEX__) == COM2) EVAL_COM2_TX_GPIO_CLK_ENABLE();} while(0)

#define COMx_TX_GPIO_CLK_DISABLE(__INDEX__)     (((__INDEX__) == COM1) ? EVAL_COM1_TX_GPIO_CLK_DISABLE() :\
                                                 ((__INDEX__) == COM2) ? EVAL_COM2_TX_GPIO_CLK_DISABLE() : 0)

#define COMx_RX_GPIO_CLK_ENABLE(__INDEX__)      do { if((__INDEX__) == COM1) EVAL_COM1_RX_GPIO_CLK_ENABLE(); else\
                                                     if((__INDEX__) == COM2) EVAL_COM2_RX_GPIO_CLK_ENABLE();} while(0)

#define COMx_RX_GPIO_CLK_DISABLE(__INDEX__)     (((__INDEX__) == COM1) ? EVAL_COM1_RX_GPIO_CLK_DISABLE() :\
                                                 ((__INDEX__) == COM2) ? EVAL_COM2_RX_GPIO_CLK_DISABLE() : 0)

/**
  * @}
  */ 

/** @defgroup STM3210E_EVAL_BUS STM3210E EVAL BUS
  * @{
  */

/* Exported constant IO ------------------------------------------------------*/
/*##################### I2Cx ###################################*/
/* User can use this section to tailor I2Cx instance used and associated 
   resources */
/* Definition for I2Cx Pins */
#define EVAL_I2Cx_SCL_PIN                       GPIO_PIN_6        /* PB.06*/
#define EVAL_I2Cx_SCL_GPIO_PORT                 GPIOB
#define EVAL_I2Cx_SDA_PIN                       GPIO_PIN_7        /* PB.07*/
#define EVAL_I2Cx_SDA_GPIO_PORT                 GPIOB

/* Definition for I2Cx clock resources */
#define EVAL_I2Cx                               I2C1
#define EVAL_I2Cx_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define EVAL_I2Cx_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define EVAL_I2Cx_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE() 

#define EVAL_I2Cx_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define EVAL_I2Cx_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()
    
/* Definition for I2Cx's NVIC */
#define EVAL_I2Cx_EV_IRQn                       I2C1_EV_IRQn
#define EVAL_I2Cx_EV_IRQHandler                 I2C1_EV_IRQHandler
#define EVAL_I2Cx_ER_IRQn                       I2C1_ER_IRQn
#define EVAL_I2Cx_ER_IRQHandler                 I2C1_ER_IRQHandler

/* I2C clock speed configuration (in Hz) 
   WARNING: 
   Make sure that this define is not already declared in other files (ie. 
   stm3210e_eval.h file). It can be used in parallel by other modules. */
#ifndef BSP_I2C_SPEED
 #define BSP_I2C_SPEED                            100000
#endif /* I2C_SPEED */


/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define EVAL_I2Cx_TIMEOUT_MAX                   3000

/*##################### SPIx ###################################*/
#define EVAL_SPIx                               SPI1
#define EVAL_SPIx_CLK_ENABLE()                  __HAL_RCC_SPI1_CLK_ENABLE()

#define EVAL_SPIx_SCK_GPIO_PORT                 GPIOA             /* PA.05*/
#define EVAL_SPIx_SCK_PIN                       GPIO_PIN_5
#define EVAL_SPIx_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_SPIx_SCK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()

#define EVAL_SPIx_MISO_MOSI_GPIO_PORT           GPIOA
#define EVAL_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define EVAL_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()
#define EVAL_SPIx_MISO_PIN                      GPIO_PIN_6       /* PA.06*/
#define EVAL_SPIx_MOSI_PIN                      GPIO_PIN_7       /* PA.07*/
/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define EVAL_SPIx_TIMEOUT_MAX                   1000

/**
  * @}
  */ 

/** @defgroup STM3210E_EVAL_COMPONENT STM3210E EVAL COMPONENT
  * @{
  */

/*##################### FLASH SPI ###################################*/
/**
  * @brief  M25P FLASH SPI Chip Select macro definition 
  */
#define FLASH_SPI_CS_LOW()       HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)
#define FLASH_SPI_CS_HIGH()      HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)

/**
  * @brief  M25P FLASH SPI Control Interface pins
  */
#define FLASH_SPI_CS_PIN                           GPIO_PIN_2        /* PB.02*/
#define FLASH_SPI_CS_GPIO_PORT                     GPIOB
#define FLASH_SPI_CS_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define FLASH_SPI_CS_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()

/**
  * @brief  M25P FLASH SPI supported commands
  */  
#define FLASH_SPI_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define FLASH_SPI_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define FLASH_SPI_CMD_WREN           0x06  /*!< Write enable instruction */
#define FLASH_SPI_CMD_READ           0x03  /*!< Read from Memory instruction */
#define FLASH_SPI_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define FLASH_SPI_CMD_RDID           0x9F  /*!< Read identification */
#define FLASH_SPI_CMD_SE             0xD8  /*!< Sector Erase instruction */
#define FLASH_SPI_CMD_BE             0xC7  /*!< Bulk Erase instruction */

#define FLASH_SPI_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */

#define FLASH_SPI_DUMMY_BYTE         0xA5
#define FLASH_SPI_PAGESIZE           0x100

#define FLASH_SPI_M25P128_ID         0x202018
#define FLASH_SPI_M25P64_ID          0x202017


/*##################### AUDIO ##########################*/
/**
  * @brief  AUDIO I2C Interface pins
  */
#define AUDIO_I2C_ADDRESS                     0x27
  
  /* Audio Reset Pin definition */
#define AUDIO_RESET_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define AUDIO_RESET_PIN                       GPIO_PIN_11
#define AUDIO_RESET_GPIO                      GPIOG

/**
  * @}
  */

/**
  * @}
  */ 



/** @addtogroup STM3210E_EVAL_Exported_Functions
  * @{
  */ 

#if defined(HAL_SRAM_MODULE_ENABLED)

static void     FSMC_BANK1NORSRAM4_WriteData(uint16_t Data);
static void     FSMC_BANK1NORSRAM4_WriteReg(uint8_t Reg);
static uint16_t FSMC_BANK1NORSRAM4_ReadData(uint8_t Reg);
static void     FSMC_BANK1NORSRAM4_Init(void);
static void     FSMC_BANK1NORSRAM4_MspInit(void);

/* LCD IO functions */
void            LCD_IO_Init(void);
void            LCD_IO_WriteData(uint16_t RegValue);
void            LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
void            LCD_IO_WriteReg(uint8_t Reg);
uint16_t        LCD_IO_ReadData(uint16_t Reg);
void            LCD_Delay (uint32_t delay);
#endif /*HAL_SRAM_MODULE_ENABLED*/

/* I2Cx bus function */
#ifdef HAL_I2C_MODULE_ENABLED
/* Link function for I2C EEPROM peripheral */
static void               I2Cx_Init(void);
static void               I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value);
static HAL_StatusTypeDef  I2Cx_WriteBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
static uint8_t            I2Cx_ReadData(uint16_t Addr, uint8_t Reg);
static HAL_StatusTypeDef  I2Cx_ReadBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
static HAL_StatusTypeDef  I2Cx_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);
static void               I2Cx_Error (void);
static void               I2Cx_MspInit(I2C_HandleTypeDef *hi2c);  

/* Link functions for Temperature Sensor peripheral */
void                      TSENSOR_IO_Init(void);
void                      TSENSOR_IO_Write(uint16_t DevAddress, uint8_t* pBuffer, uint8_t WriteAddr, uint16_t Length);
void                      TSENSOR_IO_Read(uint16_t DevAddress, uint8_t* pBuffer, uint8_t ReadAddr, uint16_t Length);
uint16_t                  TSENSOR_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);

/* Link function for Audio peripheral */
void                      AUDIO_IO_Init(void);
void                      AUDIO_IO_DeInit(void);
void                      AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t                   AUDIO_IO_Read(uint8_t Addr, uint8_t Reg);

#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_SPI_MODULE_ENABLED
/* SPIx bus function */
HAL_StatusTypeDef  SPIx_Init(void);
uint8_t            SPIx_Write(uint8_t Value);
uint8_t            SPIx_Read(void);
void               SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void               SPIx_WriteData(const uint8_t *Data, uint16_t DataLength);
void               SPIx_ReadData(const uint8_t *Data, uint16_t DataLength);
void               SPIx_Error (void);
void               SPIx_MspInit(SPI_HandleTypeDef *hspi);

/* Link function for EEPROM peripheral over SPI */
HAL_StatusTypeDef         FLASH_SPI_IO_Init(void);
uint8_t                   FLASH_SPI_IO_WriteByte(uint8_t Data);
uint8_t                   FLASH_SPI_IO_ReadByte(void);
HAL_StatusTypeDef         FLASH_SPI_IO_ReadData(uint32_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
void                      FLASH_SPI_IO_WriteEnable(void);
HAL_StatusTypeDef         FLASH_SPI_IO_WaitForWriteEnd(void);
uint32_t                  FLASH_SPI_IO_ReadID(void);
#endif /* HAL_SPI_MODULE_ENABLED */  

uint32_t                BSP_GetVersion(void);
void                    BSP_LED_Init(Led_TypeDef Led);
void                    BSP_LED_On(Led_TypeDef Led);
void                    BSP_LED_Off(Led_TypeDef Led);
void                    BSP_LED_Toggle(Led_TypeDef Led);
void                    BSP_PB_Init(ButtonMode_TypeDef Button_Mode);
GPIO_PinState           BSP_PB_GetState(void);
#ifdef HAL_UART_MODULE_ENABLED
void                    BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef* huart);
#endif /* HAL_UART_MODULE_ENABLED */


/**
  * @}
  */


/**
  * @}
  */
  
/**
  * @}
  */
  
/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif
  
#endif /* __STM3210E_EVAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

