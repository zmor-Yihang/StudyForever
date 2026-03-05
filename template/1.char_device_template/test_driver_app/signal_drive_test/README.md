# 信号驱动IO测试程序

本目录包含用于测试信号驱动IO的两个程序：signal_reader和signal_writer。

## 文件说明

- `signal_reader.c`: 信号驱动IO读取程序，通过SIGIO信号接收设备数据
- `signal_writer.c`: 信号驱动IO写入程序，向设备发送命令
- `Makefile`: 编译脚本
- `README.md`: 本说明文件

## 编译方法

```bash
make
```

## 使用方法

1. 确保字符设备驱动已加载并创建了设备文件`/dev/cdev_led0`
2. 在一个终端中运行读取程序：
   ```bash
   ./signal_reader
   ```
3. 在另一个终端中运行写入程序：
   ```bash
   ./signal_writer
   ```
4. 在写入程序中输入命令（on/off/toggle）控制LED，读取程序将通过信号接收到状态变化

## 工作原理

### 信号驱动IO读取程序 (signal_reader)

1. 打开设备文件
2. 设置文件所有者为当前进程（`F_SETOWN`）
3. 启用异步通知（设置`FASYNC`标志）
4. 注册SIGIO信号处理函数
5. 调用`pause()`等待信号
6. 当收到SIGIO信号时，读取设备数据

### 信号驱动IO写入程序 (signal_writer)

1. 打开设备文件
2. 从标准输入读取命令
3. 将命令写入设备文件
4. 写入操作会触发驱动程序发送SIGIO信号给读取程序

## 清理

```bash
make clean
```

## 注意事项

- 信号驱动IO依赖于字符设备驱动中的`fasync`函数实现
- 如果设备驱动不支持信号驱动IO，程序将无法正常工作
- 在使用前请确保设备驱动已正确加载并支持异步通知