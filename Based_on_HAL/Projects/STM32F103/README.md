# Applications

## ADC_DAC (not implemented)

- 读取模拟量，转换为数字量，再将数字量输出为模拟量
- 移植自 f103 的 Examples\ 的 DAC 和 ADC 工程

## ETH_ENC28J60 (not implemented)

- 使用 ENC28J60 MAC层 芯片结合 LwIP 实现 Ethernet TCP/UDP 通信。移植自 f107 的 LwIP_TCP/UDP_Echo_Client/Server 工程。

## FatFs_SD

- 带文件系统的 SD 卡读写示例，支持 SPI 和 SDIO 两种方式
- 原来的 f107 只有 SPI 方式，f103 只有 SDIO 方式，把 BSP 进行移植得到了 f103 的 SPI 方式
- 定义全局宏 `SD_MODE_SPI` 使用 SPI 方式，否则使用 SDIO 方式

## SPI FLASH

- 使用 SPI 读写 Flash 芯片（基于 M25Pxx 芯片代码修改，同时支持 W25Qxx）

## TIM PWM

- 使用芯片 TIM 外设输出 PWM 波

## USB Device HID MouseKeyboard

- 给 PC 模拟一个鼠标，使用 rand() 函数控制，因此鼠标是随机移动的
- 给 PC 模拟一个键盘，插入计算机后会弹出记事本并输入内容
- 移植自官方例程。修改了 usbd_hid.c 文件的 USBD_HID_CfgDesc 部分和 `HID_MOUSE_ReportDesc` 部分。

## USB Device MSC SD

- SD 卡读卡器
- 定义全局宏 SD_MODE_SPI 使用 SPI 方式，否则使用 SDIO 方式
- USB 接线方式：DM/DP = PA11/PA12，DP 需要接 >1K 电阻上拉
- 先运行程序，再将 USB 插入
