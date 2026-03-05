# 字符设备驱动阻塞IO测试

本目录包含用于验证字符设备驱动阻塞IO机制的测试程序。

## 文件说明

- `reader.c`: 从设备读取数据的程序，如果没有数据可读将阻塞等待
- `writer.c`: 向设备写入数据的程序，写入数据后会唤醒阻塞的读取进程
- `Makefile`: 用于编译ARM64版本测试程序的构建文件
- `Makefilex86`: 用于编译x86版本测试程序的构建文件

## 编译方法

### ARM64版本：
```bash
make
```

### x86版本：
```bash
make -f Makefilex86
```

编译完成后将生成两个可执行文件：
- `reader_arm64` 或 `reader_x86`: 读取测试程序
- `writer_arm64` 或 `writer_x86`: 写入测试程序

## 使用方法

1. 确保内核模块已加载且设备节点 `/dev/cdev_led0` 存在

2. 打开两个终端窗口：

   终端1 (读取端 - 将阻塞等待)：
   ```bash
   sudo ./reader_arm64
   # 或者对于x86版本：
   sudo ./reader_x86
   ```

   终端2 (写入端 - 唤醒读取进程)：
   ```bash
   sudo ./writer_arm64 "on"
   # 或者对于x86版本：
   sudo ./writer_x86 "on"
   ```

3. 预期行为：
   - 读取程序执行后将阻塞等待
   - 当写入程序执行并发送数据时，将：
     - 向设备写入数据
     - 在驱动中设置标志位
     - 唤醒等待的读取进程
   - 读取进程随后将读取到数据并退出

## 清理编译文件

### ARM64版本：
```bash
make clean
```

### x86版本：
```bash
make -f Makefilex86 clean
```

## 自定义消息

写入程序接受可选的命令行参数来发送不同的消息：

```bash
sudo ./writer_arm64 "off"
sudo ./writer_arm64 "toggle"
```
注意：字符驱动char_dev_register.c在原子操作时设置了open_count为2，这是为了确保在测试过程中可以并发访问设备。