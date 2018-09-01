# Keil Project based on HAL

## Description

使用 STM32CubeMX 固件库修改而来，基于 HAL/LL 而不是 stdPeriph

- STM3210E-EVAL - Evaluation board with STM32F103Zx MCU
- STM3210C-EVAL - Evaluation board with STM32F107VC MCU

#### DO NOT MODIFY FILES EXCEPT IN `Src`,`Inc` or `BSP`.

## Applications

#### SD 卡文件系统（SPI SD+FatFs）

File|Description
-|-
FatFs/FatFs_uSD/Inc/stm32f1xx_hal_conf.h                 | HAL configuration file
FatFs/FatFs_uSD/Inc/stm32f1xx_it.h                       | Interrupt handlers header file
FatFs/FatFs_uSD/Inc/main.h                               | Header for main.c module
FatFs/FatFs_uSD/Inc/ffconf.h                             | FAT file system module configuration file   
FatFs/FatFs_uSD/Src/stm32f1xx_it.c                       | Interrupt handlers
FatFs/FatFs_uSD/Src/main.c                               | Main program
FatFs/FatFs_uSD/Src/system_stm32f1xx.c                   | STM32F1xx system source file

- 使用 STM3210C 的 SPI 方式读写 SD 卡
- This example runs on STM32F107xC devices（已被移植到 STM32F103xx）
- 接线方式见 `stm32f10c_eval.h` 的 SPIx 和 SD 部分

#### SD 卡读卡器（USB MSC Device+SPI SD）

File|Description
-|-
USB_Device/MSC_Standalone/Src/main.c                    | Main program
USB_Device/MSC_Standalone/Src/system_stm32f1xx.c        | STM32F1xx system clock configuration file
USB_Device/MSC_Standalone/Src/stm32f1xx_it.c            | Interrupt handlers
USB_Device/MSC_Standalone/Src/usbd_conf.c               | General low level driver configuration
USB_Device/MSC_Standalone/Src/usbd_desc.c               | USB device MSC descriptor
USB_Device/MSC_Standalone/Src/ubsd_storage.c            | Media Interface Layer
USB_Device/MSC_Standalone/Inc/main.h                    | Main program header file
USB_Device/MSC_Standalone/Inc/stm32f1xx_it.h            | Interrupt handlers header file
USB_Device/MSC_Standalone/Inc/stm32f1xx_hal_conf.h      | HAL configuration file
USB_Device/MSC_Standalone/Inc/usbd_conf.h               | USB device driver Configuration file
USB_Device/MSC_Standalone/Inc/usbd_desc.h               | USB device MSC descriptor header file
USB_Device/MSC_Standalone/Inc/ubsd_storage.h            | Media Interface Layer header file

- 使用 STM3210C 的 SPI 方式读写 SD 卡，根据 STM3210E 的文件略作改动
- This example runs on STM32F107xC devices（已被移植到 STM32F103xx）
- USB 接线方式：DM/DP = PA11/PA12
- 先运行程序，再将 USB 插入

#### USB Flash Disk Reader（USB MSC Host）

File|Description
-|-
USB_Host/MSC_Standalone/Src/main.c                 | Main program
USB_Host/MSC_Standalone/Src/system_stm32f1xx.c     | STM32F1xx system clock configuration file
USB_Host/MSC_Standalone/Src/stm32f1xx_it.c         | Interrupt handlers
USB_Host/MSC_Standalone/Src/menu.c                 | MSC State Machine
USB_Host/MSC_Standalone/Src/usbh_conf.c            | General low level driver configuration
USB_Host/MSC_Standalone/Src/explorer.c             | Explore the USB flash disk content
USB_Host/MSC_Standalone/Src/file_operations.c      | Write/read file on the disk
USB_Host/MSC_Standalone/Src/usbh_diskio.c          | USB diskio interface for FatFs
USB_Host/MSC_Standalone/Inc/main.h                 | Main program header file
USB_Host/MSC_Standalone/Inc/stm32f1xx_it.h         | Interrupt handlers header file
USB_Host/MSC_Standalone/Inc/stm32f1xx_hal_conf.h   | HAL configuration file
USB_Host/MSC_Standalone/Inc/usbh_conf.h            | USB Host driver Configuration file
USB_Host/MSC_Standalone/Inc/ffconf.h               | FatFs Module Configuration file

- 使用 STM3210C 的 MSC Host
- This example runs on STM32F107xx/STM32F105xx devices.（已被移植到 STM32F103xx）

#### HID Host

File|Description
-|-
USB_Host/HID_Standalone/Src/main.c                 | Main program
USB_Host/HID_Standalone/Src/system_stm32f1xx.c     | STM32F1xx system clock configuration file
USB_Host/HID_Standalone/Src/stm32f1xx_it.c         | Interrupt handlers
USB_Host/HID_Standalone/Src/menu.c                 | HID State Machine
USB_Host/HID_Standalone/Src/usbh_conf.c            | General low level driver configuration
USB_Host/HID_Standalone/Src/mouse.c                | HID mouse functions file
USB_Host/HID_Standalone/Src/keybaord.c             | HID keyboard functions file
USB_Host/HID_Standalone/Inc/main.h                 | Main program header file
USB_Host/HID_Standalone/Inc/stm32f1xx_it.h         | Interrupt handlers header file
USB_Host/HID_Standalone/Inc/lcd_log_conf.h         | LCD log configuration file
USB_Host/HID_Standalone/Inc/stm32f1xx_hal_conf.h   | HAL configuration file
USB_Host/HID_Standalone/Inc/usbh_conf.h            | USB Host driver Configuration file

- 使用 STM3210C 的 HID Host
- This example runs on STM32F107xx/STM32F105xx devices.（已被移植到 STM32F103xx）

#### HID Device

File|Description
-|-
USB_Device/HID_Standalone/Src/main.c               |   Main program
USB_Device/HID_Standalone/Src/system_stm32f1xx.c   |   STM32F1xx system clock configuration file
USB_Device/HID_Standalone/Src/stm32f1xx_it.c       |   Interrupt handlers
USB_Device/HID_Standalone/Src/usbd_conf.c          |   General low level driver configuration
USB_Device/HID_Standalone/Src/usbd_desc.c          |   USB device HID descriptor
USB_Device/HID_Standalone/Inc/main.h               |   Main program header file
USB_Device/HID_Standalone/Inc/stm32f1xx_it.h       |   Interrupt handlers header file
USB_Device/HID_Standalone/Inc/stm32f1xx_hal_conf.h |   HAL configuration file
USB_Device/HID_Standalone/Inc/usbd_conf.h          |   USB device driver Configuration file
USB_Device/HID_Standalone/Inc/usbd_desc.h          |   USB device HID descriptor header file

- 使用 STM3210E 的 HID Device
- This example runs on STM32F102xx and STM32F103xx devices.
