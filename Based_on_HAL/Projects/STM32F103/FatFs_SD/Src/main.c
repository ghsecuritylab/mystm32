/**
  ******************************************************************************
  * @file    FatFs/FatFs_uSD/Src/main.c 
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    12-May-2017
  * @brief   Main program body
  *          This sample code shows how to use FatFs with uSD card drive.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
BYTE work[FF_MAX_SS];   // 用于格式化的工作区，值越大格式化越快

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #define GETCHAR_PROTOTYPE int __io_getchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

void SystemClock_Config(void);
static void Error_Handler(uint8_t id);

/* Private functions ---------------------------------------------------------*/

uint16_t _strlen(char* s)
{
    uint16_t i=0;
    while (s[i])++i;
    return i;
}
// 递归扫描 path 路径下的文件
FRESULT scan_files (char path[FF_LFN_BUF + 1])
{ 
  FRESULT result;
  FILINFO fno;
  DIR dir;
  uint16_t i;

  // 输出目录名
  printf("%s\n", path);
  
  // 打开目录
  result = f_opendir(&dir, path); 
  if (result != FR_OK)
    return result;

  for (;;)
  {
    // 读取目录下的下一个文件
    result = f_readdir(&dir, &fno);

    // 为空时表示所有项目读取完毕，跳出
    if (result != FR_OK || fno.fname[0] == '\0')
      break;

    // 点表示当前目录，跳过
    if (fno.fname[0] == '.') continue;

    // 目录，递归读取
    if (fno.fattrib & AM_DIR)
    {
      i = _strlen(path);
      
      // 合成完整目录名
      sprintf(path + i, "/%s", fno.fname);
      // 递归遍历
      result = scan_files(path);
      
      path[i] = '\0';
      
      if (result != FR_OK)
        continue;
    }
    else
    {
      // 输出文件名
      printf("%s/%s\n", path, fno.fname);
    }
  }
  f_closedir(&dir);
  return result;
}

#define MAKE_DATE(y,m,d)    ((WORD)(((y - 1980) << 9) | m << 5 | d))
#define MAKE_TIME(h,m,s)    ((WORD)(h << 11 | m << 5 | s / 2U))
#define GET_YEAR(d)         ((d >> 9) + 1980)
#define GET_MONTH(d)        (d >> 5 & 0x0F)
#define GET_DAY(d)          (d & 0x1F)
#define GET_HOUR(t)         (t >> 11)
#define GET_MINUTE(t)       (t >> 5 & 0x3F)
#define GET_SECONDS(t)      ((t & 0x1F)*2U)
#if _FS_NORTC==0
// 返回当前时间戳
DWORD get_fattime(void) {
	return	((DWORD)(2018 - 1980) << 25)	// Year 25:31
			| ((DWORD)1 << 21)				// Month 21:24
			| ((DWORD)1 << 16)				// Mday 16:20
			| ((DWORD)0 << 11)				// Hour 11:15
			| ((DWORD)0 << 5)				// Min 5:10
			| ((DWORD)0 / 2U);				// Sec 0:4
}
#endif

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F107xC HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();
 
  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) : BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = no parity
      - BaudRate    = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  BSP_COM_Init(COM1, &UartHandle);

  /* Output a message on Hyperterminal using printf function */
  printf("\n\r UART Printf Example: retarget the C library printf function to the UART\n\r");

  /* Infinite loop */
  //getchar();


  FRESULT res;                                          /* FatFs function common result code */
  uint32_t byteswritten, bytesread;                     /* File write/read counts */
  uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
  uint8_t rtext[100];                                   /* File read buffer */

  /*##-1- Link the micro SD disk I/O driver ##################################*/
  if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    /*##-2- Register the file system object to the FatFs module ##############*/
    res=f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
    if(res != FR_OK && res!= FR_NO_FILESYSTEM)
    {
      printf("RESULT=%d\r\n",res);

      #ifndef SD_MODE_SPI
      extern SD_HandleTypeDef uSdHandle;
      printf("ERROR=%d,STATUS=%d\r\n",uSdHandle.ErrorCode,uSdHandle.State);
      BSP_SD_CardInfo info;
      BSP_SD_GetCardInfo(&info);
      printf("BlockNbr=%u\r\n",info.BlockNbr);
      printf("BlockSize=%u\r\n",info.BlockSize);
      printf("CardType=%u\r\n",info.CardType);
      printf("CardVersion=%u\r\n",info.CardVersion);
      printf("Class=%u\r\n",info.Class);
      printf("RelCardAdd=%d\r\n",info.RelCardAdd);
      #endif 
      /* FatFs Initialization Error */
      Error_Handler(2);
    }
    else
    {
      printf("mount_ok\r\n");
      /*##-3- Create a FAT file system (format) on the logical drive #########*/
      /* WARNING: Formatting the uSD card will delete all content on the device */
        // 注意：在 Windows 上进行格式化后，使用 FatFs 遍历目录
        // 会失败（一直卡在 f_opendir 这个函数这里）
        // 使用 FatFs 进行格式化不会出现这个情况
      if(res == FR_NO_FILESYSTEM && 
          (res=f_mkfs((TCHAR const*)SDPath, FM_FAT32, 0, work, sizeof work)) != FR_OK)
      {
          printf("mkfs_err=%d",res);
        /* FatFs Format Error */
        Error_Handler(3);
      }
      else
      {
        /*##-4- Create and Open a new text file object with write access #####*/
        if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
          /* 'STM32.TXT' file Open for write Error */
          Error_Handler(4);
        }
        else
        {
          /*##-5- Write data to the text file ################################*/
          res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

          /*##-6- Close the open text file #################################*/
          if (f_close(&MyFile) != FR_OK )
          {
            Error_Handler(5);
          }
          
          if((byteswritten == 0) || (res != FR_OK))
          {
            /* 'STM32.TXT' file Write or EOF Error */
            Error_Handler(6);
          }
          else
          {      
            /*##-7- Open the text file object with read access ###############*/
            if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK)
            {
              /* 'STM32.TXT' file Open for read Error */
              Error_Handler(7);
            }
            else
            {
              /*##-8- Read data from the text file ###########################*/
              res = f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);
              
              if((bytesread == 0) || (res != FR_OK))
              {
                /* 'STM32.TXT' file Read or EOF Error */
                Error_Handler(8);
              }
              else
              {
                /*##-9- Close the open text file #############################*/
                f_close(&MyFile);
                
                /*##-10- Compare read data with the expected data ############*/
                if((bytesread != byteswritten))
                {                
                  /* Read data is different from the expected data */
                  Error_Handler(9);
                }
                else
                {
                  /* Success of the demo: no error occurrence */
                }
              }
            }
          }
        }
      }
      // 遍历目录
      char path[FF_LFN_BUF + 1] = "0:/";
      FRESULT result = scan_files(path);
      if (result != FR_OK)
        printf("FAILED_TO_LIST_FILES=%d\n", result);
    }
  } else printf("Link FAILED\r\n");
  
  /*##-11- Unlink the RAM disk I/O driver ####################################*/
  FATFS_UnLinkDriver(SDPath);

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. read a character to the USART1 and Loop until the end of transmission */
  uint8_t ch;
  HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

  return ch;
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
//-------------------------STM32F107xx---------------
  *            HSE Frequency(Hz)              = 25000000
  *            HSE PREDIV1                    = 5
  *            HSE PREDIV2                    = 5
  *            PLL2MUL                        = 8
//-------------------------STM32F103xx---------------
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
//------------------------------------------------
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Configure PLLs ------------------------------------------------------*/
  /* PLL2 configuration: PLL2CLK = (HSE / HSEPrediv2Value) * PLL2MUL = (25 / 5) * 8 = 40 MHz */
  /* PREDIV1 configuration: PREDIV1CLK = PLL2CLK / HSEPredivValue = 40 / 5 = 8 MHz */
  /* PLL configuration: PLLCLK = PREDIV1CLK * PLLMUL = 8 * 9 = 72 MHz */ 

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType        = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState              = RCC_HSE_ON;
#if defined(STM32F105xC) || defined(STM32F107xC)
  oscinitstruct.HSEPredivValue        = RCC_HSE_PREDIV_DIV5;
  oscinitstruct.Prediv1Source         = RCC_PREDIV1_SOURCE_PLL2;
  oscinitstruct.PLL2.PLL2State        = RCC_PLL2_ON;
  oscinitstruct.PLL2.PLL2MUL          = RCC_PLL2_MUL8;
  oscinitstruct.PLL2.HSEPrediv2Value  = RCC_HSE_PREDIV2_DIV5;
#elif defined(STM32F102xx) || defined(STM32F103xx)
  oscinitstruct.HSEPredivValue        = RCC_HSE_PREDIV_DIV1;
#endif
  oscinitstruct.PLL.PLLState          = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource         = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL            = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    //while(1);
    Error_Handler(0);
  }

  /* USB clock selection */
  RCC_PeriphCLKInitTypeDef rccperiphclkinit = { 0 };
  rccperiphclkinit.PeriphClockSelection = RCC_PERIPHCLK_USB;

#if defined(STM32F105xC) || defined(STM32F107xC)
  rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV3;
#elif defined(STM32F102xx) || defined(STM32F103xx)
  rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
#endif
  HAL_RCCEx_PeriphCLKConfig(&rccperiphclkinit);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                             RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    //while(1); 
    Error_Handler(0);
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  id: error id
  * @retval None
  */
static void Error_Handler(uint8_t id)
{
  printf("an error corrupted=%d\r\n",id);
  while(1)
  {
    /* Toggle LED_RED fast */
    //BSP_LED_Toggle(LED_RED);
    HAL_Delay(40);
  }
}

  
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}

#endif


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
