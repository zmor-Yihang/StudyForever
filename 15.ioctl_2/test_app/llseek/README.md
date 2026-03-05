# LLSEEK 设备驱动测试程序

## 概述

这个测试程序用于测试 llseek 设备驱动的功能，包括基本的读写操作、seek 操作和边界条件测试。

## 编译

在 test_app/llseek 目录下运行：

```bash
make
```

## 使用方法

1. 首先加载驱动模块：

```bash
sudo insmod /path/to/LLSEEK_ZMOR.ko
```

2. 检查设备节点是否创建：

```bash
ls -l /dev/llseek_dev
```

3. 如果设备节点存在但没有权限，可以修改权限：

```bash
sudo chmod 666 /dev/llseek_dev
```

4. 运行测试程序：

```bash
./test_llseek
```

## 测试内容

测试程序包含三个主要测试部分：

### 1. 基本读写测试
- 写入一个测试字符串
- 重置文件位置到开头
- 读取并验证写入的数据

### 2. Seek 操作测试
- 测试 SEEK_SET：绝对定位
- 测试 SEEK_CUR：相对当前位置定位
- 测试 SEEK_END：相对文件末尾定位

### 3. 边界条件测试
- 尝试写入超过缓冲区大小的数据
- 尝试 seek 到超出缓冲区的位置
- 尝试从超出缓冲区的位置读取

## 预期输出

测试程序会输出每个测试步骤的结果，包括：
- 写入/读取的字节数
- seek 操作后的位置
- 读取到的数据内容
- 错误信息（如果有）

## 清理

卸载驱动模块：

```bash
sudo rmmod LLSEEK_ZMOR
```

清理编译文件：

```bash
make clean
```