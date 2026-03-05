# LED Platform Device 测试程序

## 简介

本测试程序用于验证libled库的功能，包括LED的开关、切换、设置和获取状态等操作。

## 文件说明

- `test_led_platform.c`: 测试程序主文件
- `Makefile`: 用于ARM64平台编译
- `Makefilex86`: 用于x86平台编译
- `README.md`: 说明文档

## 编译方法

### ARM64平台编译
```bash
make -f Makefile
```

### x86平台编译
```bash
make -f Makefilex86
```

## 运行方法

编译完成后，将生成可执行文件`test_led_platform`，运行方法如下：

```bash
sudo ./test_led_platform
```

## 功能测试

测试程序会依次执行以下操作：

1. 打开设备文件 `/dev/platform_device`
2. 测试LED开启功能
3. 测试LED关闭功能
4. 测试LED切换功能（两次）
5. 测试LED设置功能（开启和关闭）
6. 测试LED状态获取功能
7. 关闭设备文件

## 注意事项

1. 运行测试程序需要root权限
2. 确保内核模块已经加载并且设备文件 `/dev/platform_device` 存在
3. 在实际硬件上运行时，可以看到LED灯的变化效果