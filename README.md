# STM32 sample codes

## Description

Use stdPeriph to implement simple functions

Use codes fom STM32Cube Firmware to implement SDIO, SPI-SD and USB funcitons.These code need HAL/LL (not stdPeriph) Lib so I need a new keil project.

## Tips

- BOOT1=X BOOT0=0：从用户闪存（flash）启动，一般使用这种模式
- BOOT1=1 BOOT0=1：从内置SRAM启动，这种模式可以用于调试
- BOOT1=0 BOOT0=1：从系统存储器启动，这种可以用于调试
- 通常情况下不要使用 BOOT0 和 BOOT1 占用的 GPIO 引脚，因为不能直接使用，必须进行额外的设置才能使用
- SysTick初始化时默认配置的优先级是最低的，如果在中断处理中用到了延时，要配置定时器的优先级高于其他中断的优先级。用其他定时器做延时的时候也是一样。

**keil**

- 只要用到了串口重定向 `printf` 就一定要勾选 usb microlib，否则运行、调试时会出现问题
- 进行模拟调试时 Dialog DLL 设置为 DARMSTM.DLL，Parameter 设置为 `-pSTM32F103ZE`（根据芯片型号而定）
- 进行真机调试时，Options for Target 对话框，Debug标签页，调试工具下面的 Load Application at Startup 要勾选，C/C++ 标签页的 Optimization 为 0，且 BOOT0 为 1 才能调试。
- 使用 ST-Link 进行烧录时，使用 ST-Link 上的 3V3 给单片机供电可能会供电不足导致中途失败或根本无法使用。可以用 CH341A 上的 3V3 供电。
- 在 Windows 上进行格式化后，使用 FatFs 遍历目录会失败（一直卡在 f_opendir 这个函数这里），使用 FatFs 进行格式化不会出现这个情况
- Keil 有个 BUG：当设置 5 个以上断点会无法进入 main，移除一些断点重新进入调试即可。如果程序跳入了 Hard Fault，注意是否有堆栈溢出，或者烧录的代码和调试中的代码不一致。
- Keil 编译器有时有个 BUG：`while(var==VAL);` 必须写成 `while(var==VAL){}`。
- 注意检查线路是否连接正确，分析清楚再开始操作