# LED驱动测试程序

这个目录包含了用于测试LED驱动的用户空间应用程序。

## 文件说明

- `led_test.c`: C语言编写的LED测试程序
- `Makefile`: 用于编译ARM64版本的测试程序
- `Makefilex86`: 用于编译x86版本的测试程序
- `test_led_arm64.sh`: ARM64版本的交互式测试脚本
- `test_led_x86.sh`: x86版本的交互式测试脚本
- `README.md`: 本说明文件

## 编译测试程序

### ARM64版本（用于RK3568开发板）

```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/led_test
make
```

### x86版本（用于本地测试）

```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/led_test
make -f Makefilex86
```

## 使用方法

### 方法1: 直接运行测试程序

#### ARM64版本
```bash
# 点亮LED
sudo ./led_test_arm64 on

# 熄灭LED
sudo ./led_test_arm64 off

# 切换LED状态
sudo ./led_test_arm64 toggle
```

#### x86版本
```bash
# 点亮LED
sudo ./led_test_x86 on

# 熄灭LED
sudo ./led_test_x86 off

# 切换LED状态
sudo ./led_test_x86 toggle
```

### 方法2: 使用交互式脚本

#### ARM64版本
```bash
# 运行交互式测试脚本
sudo ./test_led_arm64.sh
```

#### x86版本
```bash
# 运行交互式测试脚本
sudo ./test_led_x86.sh
```

脚本会提示您输入命令，支持以下命令：
- `on`: 点亮LED
- `off`: 熄灭LED
- `toggle`: 切换LED状态
- `help`: 显示帮助信息
- `quit`: 退出脚本

## 注意事项

1. 驱动必须已经加载，设备节点`/dev/cdev_led0`必须存在
2. 运行测试程序需要root权限（使用sudo）
3. 如果遇到权限问题，请确保您的用户属于适当的组或使用sudo

## 清理

要清理编译生成的文件：

### ARM64版本
```bash
make clean
```

### x86版本
```bash
make -f Makefilex86 clean
```