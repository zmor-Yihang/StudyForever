# LED平台设备测试程序

这个测试程序用于测试LED平台设备和驱动模块，使用libled库提供的API进行LED控制。

## 前置条件

1. 确保已经编译并加载了LED平台设备模块：
   ```bash
   cd /home/zmor/nuaa/22.platform_1/build
   make
   sudo insmod arm64/LED_PLATFORM_DEVICE.ko
   ```

2. 确保已经编译并加载了LED平台驱动模块：
   ```bash
   cd /home/zmor/nuaa/23.platform_2/build
   make
   sudo insmod arm64/PLATFORM_DRIVER.ko
   ```

3. 确保设备节点已创建且有访问权限：
   ```bash
   ls -l /dev/platform_device
   sudo chmod 666 /dev/platform_device
   ```

## 编译测试程序

```bash
cd /home/zmor/nuaa/23.platform_2/test_app/platform_test
make
```

编译成功后，测试程序将生成在`build`目录中。

## 使用方法

### 1. 命令行模式

```bash
# 点亮LED
./build/test_platform_led on

# 熄灭LED
./build/test_platform_led off

# 切换LED状态
./build/test_platform_led toggle

# 获取LED当前状态
./build/test_platform_led get

# 设置LED状态（0=熄灭，1=点亮）
./build/test_platform_led set 0
./build/test_platform_led set 1

# 运行完整测试序列
./build/test_platform_led test
```

### 2. 交互式模式

不提供任何命令行参数将进入交互式模式：

```bash
./build/test_platform_led
```

在交互式模式中，可以输入以下命令：
- `on` - 点亮LED
- `off` - 熄灭LED
- `toggle` - 切换LED状态
- `get` - 获取LED当前状态
- `set 0` - 设置LED为熄灭状态
- `set 1` - 设置LED为点亮状态
- `quit` 或 `q` - 退出交互模式

## 测试功能

测试程序使用libled库提供的以下API：

1. `led_on(int fd)` - 点亮LED
2. `led_off(int fd)` - 熄灭LED
3. `led_toggle(int fd)` - 切换LED状态
4. `led_set(int fd, int state)` - 设置LED状态
5. `led_get(int fd, int *state)` - 获取LED当前状态

完整测试序列(`test`命令)将依次测试所有这些功能，并在每个步骤后验证LED状态。

## 故障排除

1. **无法打开设备**
   - 检查设备节点是否存在：`ls -l /dev/platform_device`
   - 检查模块是否已加载：`lsmod | grep -E "(LED_PLATFORM_DEVICE|PLATFORM_DRIVER)"`
   - 检查设备权限：`sudo chmod 666 /dev/platform_device`

2. **LED操作失败**
   - 检查内核日志：`dmesg | tail`
   - 确保平台设备模块先于平台驱动模块加载

3. **编译错误**
   - 确保libled库文件存在：`ls -l /home/zmor/nuaa/23.platform_2/lib/libled/`
   - 确保编译器已安装：`gcc --version`

## 清理

清理编译生成的文件：
```bash
make clean
```

卸载内核模块：
```bash
sudo rmmod PLATFORM_DRIVER
sudo rmmod LED_PLATFORM_DEVICE
```