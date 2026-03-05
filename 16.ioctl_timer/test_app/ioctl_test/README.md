# IOCTL 测试程序

这个测试程序用于测试字符设备驱动中的ioctl函数功能。

## 功能测试

测试程序包含以下ioctl命令的测试：

1. **IOCTL_RESET_BUFFER** - 重置缓冲区内容
2. **IOCTL_GET_BUFFER_SIZE** - 获取缓冲区大小
3. **IOCTL_CLEAR_BUFFER** - 清空缓冲区内容
4. **IOCTL_SET_POSITION** - 设置文件位置
5. **IOCTL_GET_POSITION** - 获取当前文件位置
6. **IOCTL_FILL_BUFFER** - 用指定字符填充缓冲区
7. **无效ioctl命令测试** - 测试驱动对无效命令的处理

## 编译和运行

### 编译

```bash
make
```

### 运行

```bash
# 确保驱动已加载
sudo insmod LLSEEK_ZMOR.ko

# 确保设备节点存在并有权限
ls -l /dev/llseek_device
sudo chmod 666 /dev/llseek_device

# 运行测试程序
./test_ioctl
```

### 清理

```bash
make clean
```

## 注意事项

1. 在运行测试程序之前，请确保LLSEEK_ZMOR驱动模块已经加载到内核中
2. 确保设备节点 `/dev/llseek_device` 存在并且有读写权限
3. 测试程序会执行一系列测试，每个测试都会输出相应的结果
4. 如果某个ioctl命令尚未在驱动中实现，测试程序可能会返回错误

## 预期输出

测试程序会输出每个测试的结果，包括：
- ioctl命令的执行状态
- 相关的缓冲区内容
- 错误信息（如果有）

## 驱动实现参考

为了使这个测试程序正常工作，驱动中的ioctl函数需要实现以下命令：

```c
#define IOCTL_MAGIC 'Z'
#define IOCTL_RESET_BUFFER _IO(IOCTL_MAGIC, 0)
#define IOCTL_GET_BUFFER_SIZE _IOR(IOCTL_MAGIC, 1, int)
#define IOCTL_CLEAR_BUFFER _IO(IOCTL_MAGIC, 2)
#define IOCTL_SET_POSITION _IOW(IOCTL_MAGIC, 3, int)
#define IOCTL_GET_POSITION _IOR(IOCTL_MAGIC, 4, int)
#define IOCTL_FILL_BUFFER _IOW(IOCTL_MAGIC, 5, char*)
```

每个命令的具体实现可以根据驱动需求进行调整。