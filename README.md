# STM32F103 sample codes

## Description

Use stdPeriph to implement simple functions

Use codes fom STM32Cube Firmware to implement SDIO, SPI-SD and USB funcitons.These code need HAL/LL (not stdPeriph) Lib so I need a new keil project.

## Tips

- 只要用到了串口重定向（printf）就一定要勾选 usb microlib，否则运行、调试时会出现问题
- 进行模拟调试时 Dialog DLL 设置为 DARMSTM.DLL，Parameter 设置为 -pSTM32F103ZE （根据芯片型号而定）
- 使用 ST-Link 进行烧录时，使用 ST-Link 上的 3V3 给单片机供电可能会供电不足导致中途失败或根本无法使用。可以用 CH341A TTL 转串口上的 3V3 供电。
- 在 Windows 上进行格式化后，使用 FatFs 遍历目录会失败（一直卡在 f_opendir 这个函数这里），使用 FatFs 进行格式化不会出现这个情况