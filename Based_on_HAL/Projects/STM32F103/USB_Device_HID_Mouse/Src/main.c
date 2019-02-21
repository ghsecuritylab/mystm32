/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Src/main.c 
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    12-May-2017
  * @brief   USB device HID application main file.
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

/* Includes ------------------------------------------------------------------ */
#include "main.h"

/** @addtogroup STM32F1xx_HAL_Validation
  * @{
  */

/** @addtogroup STANDARD_CHECK
  * @{
  */

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
USBD_HandleTypeDef USBD_Device;

/* Private function prototypes ----------------------------------------------- */
void SystemClock_Config(void);
static void Error_Handler(void);
uint8_t USB_HID_Ready(void);
unsigned char KEY_PAGE_ASCII(char c);

/* Private functions --------------------------------------------------------- */

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. 
   */
  HAL_Init();

  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  USART_CONFIG();
	
  printf("please insert the device into your PC\n");

/* Configure Key push-button for remote wakeup */
  //BSP_PB_Init(BUTTON_MODE_EXTI);

  /* Init Device Library */
  USBD_Init(&USBD_Device, &HID_Desc, 0);

  /* Register the HID class */
  USBD_RegisterClass(&USBD_Device, &USBD_HID);

  /* Start Device Process */
  USBD_Start(&USBD_Device);

  printf("waiting...\n");
  while (!USB_HID_Ready())
  {
  }
  printf("demo started!\n");
  
#if 1
	  
	TM_USB_HIDDEVICE_Mouse_t mouse = {
	  TM_USB_HIDDEVICE_Button_Released,
	  TM_USB_HIDDEVICE_Button_Released,
	  TM_USB_HIDDEVICE_Button_Released,
	  0,0,0
	};
	while(1) {
	  HAL_Delay(1000);
	  USB_HIDDEVICE_MouseSend(&mouse);
	}

#else
  TM_USB_HIDDEVICE_Keyboard_t keyboard = {
	TM_USB_HIDDEVICE_Button_Released,
	TM_USB_HIDDEVICE_Button_Released,
	TM_USB_HIDDEVICE_Button_Released,
	TM_USB_HIDDEVICE_Button_Released,
	TM_USB_HIDDEVICE_Button_Released,
	TM_USB_HIDDEVICE_Button_Released,
	TM_USB_HIDDEVICE_Button_Released,
	TM_USB_HIDDEVICE_Button_Released,
	0,0,0,0,0,0
  };
  /* Win+R */
  keyboard.L_GUI = TM_USB_HIDDEVICE_Button_Pressed;
  keyboard.Key1 = KEY_PAGE_R;
  USB_HIDDEVICE_KeyboardSend(&keyboard);
  HAL_Delay(60);
  /* Key Release */
  keyboard.L_GUI = TM_USB_HIDDEVICE_Button_Released;
  keyboard.Key1 = KEY_PAGE_NONE;
  USB_HIDDEVICE_KeyboardSend(&keyboard);
  HAL_Delay(200);

  /* notepad */
  char buff1[] = "notepad";
  for (uint8_t i=0; i < sizeof(buff1)-1; ++i)
  {
    /* key pressed */
    keyboard.Key1 = KEY_PAGE_ASCII(buff1[i]);
    USB_HIDDEVICE_KeyboardSend(&keyboard);
    HAL_Delay(50);

    /* Key Release */
    keyboard.Key1 = KEY_PAGE_NONE;
    USB_HIDDEVICE_KeyboardSend(&keyboard);
    HAL_Delay(200);
  }

  /* Enter */
  keyboard.Key1 = KEY_PAGE_ENTER;
  USB_HIDDEVICE_KeyboardSend(&keyboard);
  HAL_Delay(50);
  /* Key Release */
  keyboard.Key1 = KEY_PAGE_NONE;
  USB_HIDDEVICE_KeyboardSend(&keyboard);
  HAL_Delay(200);
  
  /* notepad */
  char buff2[] = "hello,world!";
  for (uint8_t i=0; i < sizeof(buff2)-1; ++i)
  {
    /* key pressed */
    keyboard.Key1 = KEY_PAGE_ASCII(buff2[i]);
    USB_HIDDEVICE_KeyboardSend(&keyboard);
    HAL_Delay(50);

    /* Key Release */
    keyboard.Key1 = KEY_PAGE_NONE;
    USB_HIDDEVICE_KeyboardSend(&keyboard);
    HAL_Delay(200);
  }
#endif
  
  /* Infinite loop */
  while (1)
  {
  }
}

uint8_t USB_HID_Ready(void)
{
	return (USBD_Device.dev_state == USBD_STATE_CONFIGURED) &&
		(((USBD_HID_HandleTypeDef *)USBD_Device.pClassData)->state == HID_IDLE);
}

/* convert ascii character to KEY_PAGE_xx for a-zA-Z0-9 */
unsigned char KEY_PAGE_ASCII(char c)
{
	if (c>='a' && c<='z')
	{
		c -= 'a';
		c += KEY_PAGE_A;
	}
	else if (c>='A' && c<='Z')
	{
		c -= 'A';
		c += KEY_PAGE_A;
	}
	else if (c>='1' && c<='9')
	{
		c -= '1';
		c += KEY_PAGE_1;
	}
	else if (c=='0')
	{
		c = KEY_PAGE_0;
	}
	return c;
}

void USB_HIDDEVICE_KeyboardSend(TM_USB_HIDDEVICE_Keyboard_t* keyboard)
{
	uint8_t HID_Buffer[9] = {0}; /* 9 bytes long report */

	/* Report ID */
	//HID_Buffer[0] = 0x01; /* Keyboard */

	/* Control buttons */
	HID_Buffer[1] = 0;
	HID_Buffer[1] |= keyboard->L_CTRL 	<< 0;	/* Bit 0 */
	HID_Buffer[1] |= keyboard->L_SHIFT 	<< 1;	/* Bit 1 */
	HID_Buffer[1] |= keyboard->L_ALT 	<< 2;	/* Bit 2 */
	HID_Buffer[1] |= keyboard->L_GUI 	<< 3;	/* Bit 3 */
	HID_Buffer[1] |= keyboard->R_CTRL 	<< 4;	/* Bit 4 */
	HID_Buffer[1] |= keyboard->R_SHIFT 	<< 5;	/* Bit 5 */
	HID_Buffer[1] |= keyboard->R_ALT 	<< 6;	/* Bit 6 */
	HID_Buffer[1] |= keyboard->R_GUI 	<< 7;	/* Bit 7 */

	/* Padding */
	HID_Buffer[2] = 0x00;

	/* Keys */
	HID_Buffer[3] = keyboard->Key1;
	HID_Buffer[4] = keyboard->Key2;
	HID_Buffer[5] = keyboard->Key3;
	HID_Buffer[6] = keyboard->Key4;
	HID_Buffer[7] = keyboard->Key5;
	HID_Buffer[8] = keyboard->Key6;

	/* Send to USB gamepad data */
    USBD_HID_SendReport(&USBD_Device, HID_Buffer+1, sizeof HID_Buffer-1);
}

void USB_HIDDEVICE_MouseSend(TM_USB_HIDDEVICE_Mouse_t* mouse)
{
#define CURSOR_STEP     4
  uint8_t HID_Buffer[5];

  static __IO uint32_t counter = 0;
  /* check Joystick state every 50ms */
  if (counter++ == 100)
  {
    if (rand() > RAND_MAX/2)
    	mouse->XAxis = -CURSOR_STEP;
	else
		mouse->XAxis = CURSOR_STEP;

    if (rand() > RAND_MAX/2)
    	mouse->YAxis = -CURSOR_STEP;
	else
		mouse->YAxis = CURSOR_STEP;
	
	HID_Buffer[0] = 0x02;			// report ID = 2: mouse
    HID_Buffer[1] |= mouse->LeftButton << 0;		// buttons modifier bits(leftbutton-0)
    HID_Buffer[1] |= mouse->RightButton << 1;	// buttons modifier bits(rightbutton-1)
    HID_Buffer[1] |= mouse->MiddleButton << 2;	// buttons modifier bits(middlebutton-2)
    HID_Buffer[2] = mouse->XAxis;	// x
    HID_Buffer[3] = mouse->YAxis;	// y
    HID_Buffer[4] = mouse->Wheel;	// wheel
	
    /* send data though IN endpoint */
    if ((mouse->XAxis != 0) || (mouse->YAxis != 0))
    {
      USBD_HID_SendReport(&USBD_Device, HID_Buffer, sizeof HID_Buffer);
    }
    counter = 0;
  }
}

static TM_USB_HIDDEVICE_Gamepad_t gamepad = {
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  TM_USB_HIDDEVICE_Button_Released,
  0,0,0,0
};
void USB_HIDDEVICE_GamepadSend(TM_USB_HIDDEVICE_Gamepad_t* gamepad)
{
	uint8_t HID_Buffer[7]; /* 7 bytes long report */
	
	/* Report ID */
	HID_Buffer[0] = 0x03; /* gamepad id */
	
	/* Buttons pressed, byte 1 */
	HID_Buffer[1] = 0;
	HID_Buffer[1] |= gamepad->Button1 << 0;	/* Bit 0 */
	HID_Buffer[1] |= gamepad->Button2 << 1;	/* Bit 1 */
	HID_Buffer[1] |= gamepad->Button3 << 2;	/* Bit 2 */
	HID_Buffer[1] |= gamepad->Button4 << 3;	/* Bit 3 */
	HID_Buffer[1] |= gamepad->Button5 << 4;	/* Bit 4 */
	HID_Buffer[1] |= gamepad->Button6 << 5;	/* Bit 5 */
	HID_Buffer[1] |= gamepad->Button7 << 6;	/* Bit 6 */
	HID_Buffer[1] |= gamepad->Button8 << 7;	/* Bit 7 */
	
	/* Buttons pressed, byte 2 */
	HID_Buffer[2] = 0;
	HID_Buffer[2] |= gamepad->Button9 << 0;	/* Bit 0 */
	HID_Buffer[2] |= gamepad->Button10 << 1;	/* Bit 1 */
	HID_Buffer[2] |= gamepad->Button11 << 2;	/* Bit 2 */
	HID_Buffer[2] |= gamepad->Button12 << 3;	/* Bit 3 */
	HID_Buffer[2] |= gamepad->Button13 << 4;	/* Bit 4 */
	HID_Buffer[2] |= gamepad->Button14 << 5;	/* Bit 5 */
	HID_Buffer[2] |= gamepad->Button15 << 6;	/* Bit 6 */
	HID_Buffer[2] |= gamepad->Button16 << 7;	/* Bit 7 */
	
	/* Left joystick */
	HID_Buffer[3] = gamepad->LeftXAxis;
	HID_Buffer[4] = gamepad->LeftYAxis;
	
	/* Right joystick */
	HID_Buffer[5] = gamepad->RightXAxis;
	HID_Buffer[6] = gamepad->RightYAxis;
	
	/* Send to USB gamepad data */
    USBD_HID_SendReport(&USBD_Device, HID_Buffer, sizeof HID_Buffer);
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
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = { 0 };
  RCC_OscInitTypeDef oscinitstruct = { 0 };
  RCC_PeriphCLKInitTypeDef rccperiphclkinit = { 0 };

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;

  oscinitstruct.PLL.PLLState = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }

  /* USB clock selection */
  rccperiphclkinit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  HAL_RCCEx_PeriphCLKConfig(&rccperiphclkinit);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
   * clocks dividers */
  clkinitstruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
   * number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file, 
   * line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
