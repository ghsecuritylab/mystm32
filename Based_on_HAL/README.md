# Keil Project based on HAL

File|Description
-|-
Src/main.c                 |Main program
Src/system_stm32f1xx.c     |STM32F1xx system clock configuration file
Src/stm32f1xx_it.c         |Interrupt handlers 
Src/stm32f1xx_hal_msp.c    |HAL MSP module
Inc/main.h                 |Main program header file  
Inc/stm32f1xx_hal_conf.h   |HAL Configuration file
Inc/stm32f1xx_it.h         |Interrupt handlers header file

- STM3210E-EVAL - Evaluation board with STM32F103Zx MCU
- STM3210C-EVAL - Evaluation board with STM32F107VC MCU

### DO NOT MODIFY FILES EXCEPT IN `Src`,`Inc` or `BSP`.

## Application

- SPI SD 读写程序，接线方式见 `stm32f10c_eval.h` 的 SPIx 和 SD 部分
- USB 接线方式：DM/DP = PA11/PA12
- 先运行程序，再将 USB 插入