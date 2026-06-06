# Driver 目录结构说明

本文说明 `headers` 与 `common` 两个目录在 TI DSP2833x 工程中的包含关系和层级关系。

## 总体分层

```text
User 应用代码
  ├─ Driver/headers
  │   ↓
  │   DSP2833x 芯片寄存器与外设硬件
  │
  └─ Driver/common
      ↓
      Driver/headers
      ↓
      DSP2833x 芯片寄存器与外设硬件
```

含义如下：

- `headers` 是底层芯片描述层，用户代码和 `common` 都可能直接依赖它。
- `common` 是基于芯片描述层封装出来的公共初始化、默认中断、外设初始化和示例工程支持层。
- 用户代码可以通过 `DSP28x_Project.h` 同时拿到 `common` 和 `headers` 的能力，也可以直接通过 `DSP2833x_Device.h` 使用 `headers` 中的芯片寄存器定义。

## headers 目录

```text
Driver/headers
  ├─ include
  │   ├─ DSP2833x_Device.h
  │   ├─ DSP2833x_Adc.h
  │   ├─ DSP2833x_Gpio.h
  │   ├─ DSP2833x_EPwm.h
  │   ├─ DSP2833x_PieCtrl.h
  │   ├─ DSP2833x_PieVect.h
  │   └─ ... 其他外设寄存器头文件
  │
  └─ source
      └─ DSP2833x_GlobalVariableDefs.c
```

`headers/include` 的职责是描述芯片本身：

```text
外设寄存器结构体
寄存器位定义
寄存器地址映射
全局寄存器变量声明
```

其中核心入口是：

```text
DSP2833x_Device.h
```

它会集中包含各个外设的寄存器头文件：

```text
DSP2833x_Device.h
  ├─ DSP2833x_Adc.h
  ├─ DSP2833x_DevEmu.h
  ├─ DSP2833x_CpuTimers.h
  ├─ DSP2833x_ECan.h
  ├─ DSP2833x_ECap.h
  ├─ DSP2833x_DMA.h
  ├─ DSP2833x_EPwm.h
  ├─ DSP2833x_EQep.h
  ├─ DSP2833x_Gpio.h
  ├─ DSP2833x_I2c.h
  ├─ DSP2833x_Mcbsp.h
  ├─ DSP2833x_PieCtrl.h
  ├─ DSP2833x_PieVect.h
  ├─ DSP2833x_Spi.h
  ├─ DSP2833x_Sci.h
  ├─ DSP2833x_SysCtrl.h
  ├─ DSP2833x_XIntrupt.h
  └─ DSP2833x_Xintf.h
```

`headers/source/DSP2833x_GlobalVariableDefs.c` 的作用是给这些寄存器映射变量提供实际定义。简单说：

```text
头文件负责声明有哪些寄存器对象
DSP2833x_GlobalVariableDefs.c 负责真正生成这些寄存器对象
```

所以 `headers` 可以理解为“芯片寄存器基础层”。

## common 目录

```text
Driver/common
  ├─ include
  │   ├─ DSP28x_Project.h
  │   ├─ DSP2833x_Examples.h
  │   ├─ DSP2833x_GlobalPrototypes.h
  │   ├─ DSP2833x_DefaultIsr.h
  │   ├─ DSP2833x_EPwm_defines.h
  │   ├─ DSP2833x_Dma_defines.h
  │   └─ DSP2833x_I2c_defines.h
  │
  └─ source
      ├─ DSP2833x_SysCtrl.c
      ├─ DSP2833x_Gpio.c
      ├─ DSP2833x_PieCtrl.c
      ├─ DSP2833x_PieVect.c
      ├─ DSP2833x_DefaultIsr.c
      ├─ DSP2833x_CpuTimers.c
      ├─ DSP2833x_EPwm.c
      ├─ DSP2833x_DMA.c
      └─ ... 其他公共外设初始化文件
```

`common` 的职责是提供工程公共能力：

```text
系统时钟初始化
外设时钟初始化
GPIO 初始化
PIE 中断初始化
默认中断服务
CPU Timer 初始化
Flash / RAM 辅助逻辑
外设示例宏
公共函数声明
```

其中最高层的公共入口是：

```text
DSP28x_Project.h
```

它的包含关系是：

```text
DSP28x_Project.h
  ├─ DSP2833x_Device.h
  └─ DSP2833x_Examples.h
```

也就是说，用户代码如果包含 `DSP28x_Project.h`，就会同时拿到：

```text
芯片寄存器定义
公共初始化接口
示例工程配置
延时宏
默认中断声明
PWM / DMA / I2C 示例宏
```

## DSP2833x_Device.h 与 DSP2833x_Examples.h 的关系

这两个文件职责不同。

```text
DSP2833x_Device.h
  ↓
描述芯片硬件：寄存器、外设结构体、地址映射

DSP2833x_Examples.h
  ↓
描述示例工程公共配置：时钟配置、延时宏、公共初始化函数、默认 ISR、示例宏
```

`DSP2833x_Examples.h` 又继续包含：

```text
DSP2833x_Examples.h
  ├─ DSP2833x_GlobalPrototypes.h
  ├─ DSP2833x_EPwm_defines.h
  ├─ DSP2833x_Dma_defines.h
  ├─ DSP2833x_I2c_defines.h
  └─ DSP2833x_DefaultIsr.h
```

对应作用：

- `DSP2833x_GlobalPrototypes.h`：公共初始化函数声明。
- `DSP2833x_EPwm_defines.h`：ePWM 示例宏。
- `DSP2833x_Dma_defines.h`：DMA 示例宏。
- `DSP2833x_I2c_defines.h`：I2C 示例宏。
- `DSP2833x_DefaultIsr.h`：默认中断服务声明。

## 实际包含路径

常见用户代码包含方式：

```text
User/main.c
  ↓
DSP28x_Project.h
  ├─ DSP2833x_Device.h
  │   └─ headers/include 下的所有外设寄存器头文件
  │
  └─ DSP2833x_Examples.h
      ├─ DSP2833x_GlobalPrototypes.h
      ├─ DSP2833x_EPwm_defines.h
      ├─ DSP2833x_Dma_defines.h
      ├─ DSP2833x_I2c_defines.h
      └─ DSP2833x_DefaultIsr.h
```

很多 `common/source` 下的源文件会直接包含：

```text
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
```

原因是这些公共驱动文件既需要访问寄存器，又需要使用公共配置、初始化函数声明、延时宏或默认中断声明。

## 层级关系总结

```text
第 1 层：硬件描述层
  Driver/headers/include/*.h
  负责芯片寄存器、外设结构体、地址映射。

第 2 层：寄存器变量定义层
  Driver/headers/source/DSP2833x_GlobalVariableDefs.c
  负责生成实际的寄存器映射变量。

第 3 层：公共驱动实现层
  Driver/common/source/*.c
  负责系统、GPIO、中断、外设等初始化实现。

第 4 层：公共工程入口层
  Driver/common/include/DSP28x_Project.h
  统一包含 DSP2833x_Device.h 和 DSP2833x_Examples.h。

第 5 层：用户应用层
  User/*.c
  使用 Driver 提供的寄存器定义和公共初始化能力。
```
