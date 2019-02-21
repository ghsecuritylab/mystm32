/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    12-May-2017
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "f103_usart.h"
#include "usbd_core.h"
#include "stm32f1xx_hal_pcd.h"
#include "usbd_desc.h"
#include "usbd_hid.h"
#include "usbd_hid_keys.h"
#include "time.h"

/* Exported types ------------------------------------------------------------*/


/**
 * @brief  Button status enumeration
 */
typedef enum {
	TM_USB_HIDDEVICE_Button_Released = 0x00, /*!< Button is not pressed */
	TM_USB_HIDDEVICE_Button_Pressed = 0x01   /*!< Button is pressed */
} TM_USB_HIDDEVICE_Button_t;

/**
 * @brief  Mouse structure, to work with mouse
 */
typedef struct {
	TM_USB_HIDDEVICE_Button_t LeftButton;   /*!< Detect if left button is pressed and set this to send command to computer, This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t RightButton;  /*!< Detect if right button is pressed and set this to send command to computer, This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t MiddleButton; /*!< Detect if middle button is pressed and set this to send command to computer, This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	int8_t XAxis;                           /*!< Mouse X axis */
	int8_t YAxis;                           /*!< Mouse Y axis */
	int8_t Wheel;                           /*!< Mouse wheel rotation */
} TM_USB_HIDDEVICE_Mouse_t;

/**
 * @brief  Game pad structure for 2 game pads available
 */
typedef struct {
	TM_USB_HIDDEVICE_Button_t Button1;  /*!< Game pad button 1 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button2;  /*!< Game pad button 2 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button3;  /*!< Game pad button 3 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button4;  /*!< Game pad button 4 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button5;  /*!< Game pad button 5 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button6;  /*!< Game pad button 6 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button7;  /*!< Game pad button 7 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button8;  /*!< Game pad button 8 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button9;  /*!< Game pad button 9 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button10; /*!< Game pad button 10 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button11; /*!< Game pad button 11 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button12; /*!< Game pad button 12 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button13; /*!< Game pad button 13 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button14; /*!< Game pad button 14 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button15; /*!< Game pad button 15 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t Button16; /*!< Game pad button 16 status. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	int8_t LeftXAxis;                   /*!< Left joystick X axis */
	int8_t LeftYAxis;                   /*!< Left joystick Y axis */
	int8_t RightXAxis;                  /*!< Right joystick X axis */
	int8_t RightYAxis;                  /*!< Right joystick Y axis */
} TM_USB_HIDDEVICE_Gamepad_t;

/**
 * @brief  Keyboard structure
 * @note   Keyboard has special 8 buttons (CTRL, ALT, SHIFT, GUI (or WIN), all are left and right).
 *         It also supports 6 others keys to be pressed at same time, let's say Key1 = 'a', Key2 = 'b', and you will get "ab" result on the screen.
 *         If key is not used, then 0x00 value should be set!
 */
typedef struct {
	TM_USB_HIDDEVICE_Button_t L_CTRL;  /*!< Left CTRL button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t L_ALT;   /*!< Left ALT button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t L_SHIFT; /*!< Left SHIFT button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t L_GUI;   /*!< Left GUI (Win) button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t R_CTRL;  /*!< Right CTRL button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t R_ALT;   /*!< Right ALT button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t R_SHIFT; /*!< Right SHIFT button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	TM_USB_HIDDEVICE_Button_t R_GUI;   /*!< Right GUI (Win) button. This parameter can be a value of @ref TM_USB_HIDDEVICE_Button_t enumeration */
	uint8_t Key1;                      /*!< Key used with keyboard. This can be whatever. Like numbers, letters, everything. */
	uint8_t Key2;                      /*!< Key used with keyboard. This can be whatever. Like numbers, letters, everything. */
	uint8_t Key3;                      /*!< Key used with keyboard. This can be whatever. Like numbers, letters, everything. */
	uint8_t Key4;                      /*!< Key used with keyboard. This can be whatever. Like numbers, letters, everything. */
	uint8_t Key5;                      /*!< Key used with keyboard. This can be whatever. Like numbers, letters, everything. */
	uint8_t Key6;                      /*!< Key used with keyboard. This can be whatever. Like numbers, letters, everything. */
} TM_USB_HIDDEVICE_Keyboard_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void USB_HIDDEVICE_KeyboardSend(TM_USB_HIDDEVICE_Keyboard_t* keyboard);
void USB_HIDDEVICE_MouseSend(TM_USB_HIDDEVICE_Mouse_t* mouse);
void USB_HIDDEVICE_GamepadSend(TM_USB_HIDDEVICE_Gamepad_t* gamepad);
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
