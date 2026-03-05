# LED Platform Device 驱动程序 - 变量与函数参考手册

## 目录结构
```
2.led_platform_device/
├── user/                 # 驱动和设备核心代码
│   ├── led0_platform_driver.h
│   ├── led0_platform_driver.c
│   ├── led0_platform_device.h
│   └── led0_platform_device.c
├── lib/
│   └── libled0/          # 用户空间库函数
│       ├── libled0.h
│       ├── led0_on.c
│       ├── led0_off.c
│       ├── led0_toggle.c
│       ├── led0_set.c
│       └── led0_get.c
└── test_led/             # 测试程序
    └── test_led_platform.c
```

## 一、宏定义常量

### 1.1 魔数和IOCTL命令
| 名称 | 值 | 描述 |
|------|----|------|
| `MAGICNUM` | 'Z' | IOCTL命令魔数 |
| `LED0_ON` | `_IO(MAGICNUM, 0)` | 点亮LED的IOCTL命令 |
| `LED0_OFF` | `_IO(MAGICNUM, 1)` | 熄灭LED的IOCTL命令 |
| `LED0_TOGGLE` | `_IO(MAGICNUM, 2)` | 切换LED状态的IOCTL命令 |
| `LED0_SET` | `_IOW(MAGICNUM, 3, int)` | 设置LED状态的IOCTL命令 |
| `LED0_GET` | `_IOR(MAGICNUM, 4, int)` | 获取LED状态的IOCTL命令 |

### 1.2 缓冲区大小
| 名称 | 值 | 描述 |
|------|----|------|
| `BUFFER_SIZE` | 512 | 字符设备缓冲区大小 |
| `REG_SIZE` | 4 | 寄存器大小（字节） |

### 1.3 地址和位掩码
| 名称 | 值 | 描述 |
|------|----|------|
| `LED_BIT_MASK` | `(1 << 15)` | GPIO_B7对应的位掩码 |
| `PMU_GRF_MUX_CONTROL_REG_BASE_ADDR` | 0xFDC20000 | PMU GRF复用功能控制寄存器基地址 |
| `GPIO_B_H_MUX_CONTROL_REG_OFFSET` | 0x000C | GPIO_B_4567复用功能控制寄存器偏移地址 |
| `GPIO_0_BASE_ADDR` | 0xFDD60000 | GPIO_0寄存器基地址 |
| `GPIO0_AB_DIRECTION_CONTROL_REG_OFFSET` | 0x0008 | GPIO_x_A/B方向控制寄存器偏移地址 |
| `GPIO0_DATA_REG_OFFSET` | 0x0000 | GPIO_x_A/B数据寄存器偏移地址 |
| `LED_MUX_CONTROL_REG` | PMU_GRF_MUX_CONTROL_REG_BASE_ADDR + GPIO_B_H_MUX_CONTROL_REG_OFFSET | LED复用控制寄存器地址 |
| `LED_DIRECTION_CONTROL_REG` | GPIO_0_BASE_ADDR + GPIO0_AB_DIRECTION_CONTROL_REG_OFFSET | LED方向控制寄存器地址 |
| `LED_DATA_REG` | GPIO_0_BASE_ADDR + GPIO0_DATA_REG_OFFSET | LED数据寄存器地址 |

## 二、数据结构

### 2.1 字符设备处理结构体
```c
typedef struct {
    char name[20];                /* 设备名称 */
    dev_t device_number;          /* 设备号 */
    uint32_t device_major_number; /* 主设备号 */
    uint32_t device_minor_number; /* 次设备号 */
    struct cdev char_device_t;    /* 字符设备结构体 */
    struct class *device_class_p; /* 设备类指针 */
    struct device *device_node_p; /* 设备节点指针 */
    void __iomem *vir_mux_control;       // 复用控制寄存器虚拟地址
    void __iomem *vir_direction_control; // 方向控制寄存器虚拟地址
    void __iomem *vir_data;              // 数据寄存器虚拟地址
    char kernel_buffer[BUFFER_SIZE];     /* 字符设备缓冲区 */
} char_device_handle;
```

### 2.2 平台设备资源数组
```c
struct resource led0_resources[3]
```

### 2.3 平台设备结构体
```c
struct platform_device led0_platform_device_t
```

### 2.4 文件操作结构体
```c
struct file_operations led0_file_operations_t
```

### 2.5 平台驱动结构体
```c
struct platform_driver led0_driver_t
```

## 三、全局变量

| 名称 | 类型 | 描述 |
|------|------|------|
| `led0_h` | `char_device_handle` | 字符设备处理实例 |
| `led0_resources` | `struct resource[3]` | 平台设备使用的资源数组 |
| `led0_platform_device_t` | `struct platform_device` | 平台设备结构体实例 |
| `led0_file_operations_t` | `struct file_operations` | 文件操作结构体实例 |
| `led0_driver_t` | `struct platform_driver` | 平台驱动结构体实例 |

## 四、内核空间函数

### 4.1 平台驱动函数
| 函数名 | 参数 | 返回值 | 描述 |
|--------|------|--------|------|
| `led0_platform_driver_probe` | `struct platform_device *pdev` | `int` | 平台驱动探测函数 |
| `led0_platform_driver_remove` | `struct platform_device *pdev` | `int` | 平台驱动移除函数 |
| `led0_platform_driver_init` | 无 | `int` | 驱动模块初始化函数 |
| `led0_platform_driver_exit` | 无 | `void` | 驱动模块退出函数 |

### 4.2 设备操作函数
| 函数名 | 参数 | 返回值 | 描述 |
|--------|------|--------|------|
| `led0_platform_driver_open` | `struct inode *inode, struct file *filp` | `int` | 设备打开函数 |
| `led0_platform_driver_release` | `struct inode *inode, struct file *filp` | `int` | 设备释放函数 |
| `led0_platform_driver_write` | `struct file *filp, const char __user *buf, size_t count, loff_t *f_pos` | `ssize_t` | 设备写入函数 |
| `led0_platform_driver_read` | `struct file *filp, char __user *buf, size_t count, loff_t *f_pos` | `ssize_t` | 设备读取函数 |
| `led0_platform_driver_llseek` | `struct file *filp, loff_t offset, int whence` | `loff_t` | 设备定位函数 |
| `led0_platform_driver_ioctl` | `struct file *filp, unsigned int cmd, unsigned long arg` | `long` | 设备IO控制函数 |

### 4.3 平台设备函数
| 函数名 | 参数 | 返回值 | 描述 |
|--------|------|--------|------|
| `led0_platform_device_release` | `struct device *dev` | `void` | 设备释放回调函数 |
| `led_platform_device_init` | 无 | `int` | 设备模块初始化函数 |
| `led_platform_device_exit` | 无 | `void` | 设备模块退出函数 |

## 五、用户空间库函数

### 5.1 LED控制函数
| 函数名 | 参数 | 返回值 | 描述 |
|--------|------|--------|------|
| `led0_on` | `int fd` | `int` | 点亮LED |
| `led0_off` | `int fd` | `int` | 熄灭LED |
| `led0_toggle` | `int fd` | `int` | 切换LED状态 |
| `led0_set` | `int fd, int state` | `int` | 设置LED状态 |
| `led0_get` | `int fd, int *state` | `int` | 获取LED状态 |

## 六、测试程序函数
| 函数名 | 参数 | 返回值 | 描述 |
|--------|------|--------|------|
| `main` | 无 | `int` | 测试程序主函数 |

## 七、设备节点
设备注册后将在 `/dev/` 目录下创建名为 `led0` 的设备节点。

## 八、使用方法
1. 编译驱动模块并加载到内核
2. 编译用户空间库和测试程序
3. 运行测试程序验证功能
4. 使用库函数开发自己的应用程序
