# Keil Project based on HAL

## Description

使用 STM32CubeMX 固件库修改而来，基于 HAL/LL 而不是 stdPeriph

- STM3210E-EVAL - Evaluation board with STM32F103 MCU
- STM3210C-EVAL - Evaluation board with STM32F107 MCU

#### DO NOT MODIFY FILES EXCEPT IN `Src`,`Inc` or `BSP`.

## Applications

#### SD 卡文件系统（SD+FatFs）

- 接线方式见 `stm32f10e_eval.h` 的 SPIx 和 SD 部分
- 定义全局宏 SD_MODE_SPI 使用 SPI 方式，否则使用 SDIO 方式

#### SD 卡读卡器（USB MSC Device+SPI SD）

- 定义全局宏 SD_MODE_SPI 使用 SPI 方式，否则使用 SDIO 方式
- USB 接线方式：DM/DP = PA11/PA12
- 先运行程序，再将 USB 插入

#### USB Flash Disk Reader（USB MSC Host）

- 所谓 OTG 就是说产品既可以作为设备端又可以作为主机端
- 只有 STM32F107 等支持 OTG 功能的 MCU 才可以用作 USB Host，STM32F103 只能用作 Device

#### HID Host

- 只有 STM32F107 等支持 OTG 功能的 MCU 才可以用作 USB Host，STM32F103 只能用作 Device

#### HID Keyboard 

- 模拟一个键盘