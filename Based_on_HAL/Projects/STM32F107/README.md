# Applications

## USB Host MSC FlashDisk (not implemented)

- U盘读写器 USB Flash Disk Reader/Writer
- 所谓 OTG 就是说产品既可以作为设备端又可以作为主机端
- 只有 STM32F107 等支持 OTG 功能的 MCU 才可以用作 USB Host，STM32F103 只能用作 Device
- STM32F107 使用芯片 OTG 功能实现

## USB Host HID MouseKeyboard (not implemented)

- 接收鼠标或键盘的按键消息
- STM32F107 使用芯片 OTG 功能实现，STM32F103 使用 CH375 实现

