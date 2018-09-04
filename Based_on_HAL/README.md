# Keil Project based on HAL

## Description

使用 STM32CubeMX 固件库修改而来，基于 HAL/LL 而不是 stdPeriph

- STM3210E-EVAL - Evaluation board with STM32F103 MCU
- STM3210C-EVAL - Evaluation board with STM32F107 MCU

#### DO NOT MODIFY FILES EXCEPT IN `Src`,`Inc` or `BSP`.

## Applications

#### SPI FLASH

- 使用 SPI 读写 Flash 芯片（基于 M25Pxx 芯片代码修改，同时支持 W25Qxx）

#### SPI/SDIO SD + FatFs

- 带文件系统的 SD 卡读写示例
- 原来的 f107 只有 SPI 方式，f103 只有 SDIO 方式，把 BSP 进行移植得到了 f103 的 SPI 方式
- 接线方式见 `stm32f10e_eval.h` 的 SPIx 和 SD 部分
- 定义全局宏 SD_MODE_SPI 使用 SPI 方式，否则使用 SDIO 方式

#### USB Device MSC SD

- SD 卡读卡器
- 定义全局宏 SD_MODE_SPI 使用 SPI 方式，否则使用 SDIO 方式
- USB 接线方式：DM/DP = PA11/PA12
- 先运行程序，再将 USB 插入

#### USB Host MSC FlashDisk(not complemented)

- U盘读写器 USB Flash Disk Reader/Writer
- 所谓 OTG 就是说产品既可以作为设备端又可以作为主机端
- 只有 STM32F107 等支持 OTG 功能的 MCU 才可以用作 USB Host，STM32F103 只能用作 Device

#### USB Host HID Keyboard(not complemented)

- 接收键盘的按键内容

#### USB Device HID Mouse 

- 给 PC 模拟一个鼠标，使用 rand() 函数控制，因此鼠标是随机移动的