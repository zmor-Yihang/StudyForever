# 原子性防止并发访问测试

本测试程序用于验证字符设备驱动的原子性保护机制，确保只有一个进程能够同时访问设备。

## 文件说明

- `atomk_test.c`: 测试程序源代码，包含多线程并发访问测试
- `Makefile`: 编译ARM64版本的测试程序
- `Makefilex86`: 编译x86版本的测试程序
- `test_atomk.sh`: ARM64版本的测试执行脚本
- `test_atomk_x86.sh`: x86版本的测试执行脚本
- `README.md`: 本说明文件

## 测试原理

字符设备驱动使用 `atomic64_t open_count` 来实现原子性保护：
- 当设备被打开时，`open_count` 递减
- 当设备被关闭时，`open_count` 递增
- 只有当 `open_count` 大于 0 时，才允许打开设备
- 这确保了同一时间只有一个进程能够打开设备

## 测试内容

1. **独占访问测试**:
   - 一个进程打开设备后，验证其他进程无法打开
   - 关闭设备后，验证可以重新打开

2. **多线程并发访问测试**:
   - 创建多个线程同时尝试访问设备
   - 统计成功和失败的访问次数
   - 计算操作的平均时间

## 编译测试程序

### ARM64版本（用于RK3568开发板）

```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/atomk_test
make
```

### x86版本（用于本地测试）

```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/atomk_test
make -f Makefilex86
```

## 使用方法

### 方法一：使用测试脚本（推荐）

#### ARM64版本
```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/atomk_test
sudo ./test_atomk.sh
```

#### x86版本
```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/atomk_test
sudo ./test_atomk_x86.sh
```

### 方法二：手动编译和运行

#### ARM64版本
```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/atomk_test

# 编译
make

# 运行测试
sudo ./atomk_test_arm64

# 清理编译文件
make clean
```

#### x86版本
```bash
cd /home/zmor/nuaa/8.atomk/test_driver_app/atomk_test

# 编译
make -f Makefilex86

# 运行测试
sudo ./atomk_test_x86

# 清理编译文件
make -f Makefilex86 clean
```

## 前置条件

1. 确保字符设备驱动已加载
2. 确保设备节点 `/dev/cdev_led0` 已创建
3. 确保有权限访问设备节点（可能需要使用sudo）

## 预期结果

- 独占访问测试：当一个进程打开设备后，其他进程尝试打开应该失败
- 多线程并发测试：应该有部分访问被拒绝，表明原子性保护有效

## 注意事项

1. 如果所有访问都成功，可能表示原子性保护未生效
2. 测试过程中会多次切换LED状态，实际使用时请注意
3. 如果权限不足，请使用sudo运行测试脚本

## 自定义测试

可以通过修改 `atomk_test.c` 中的以下参数来自定义测试：

```c
#define NUM_THREADS 10          // 线程数量
#define ITERATIONS_PER_THREAD 5 // 每个线程的迭代次数
```

修改后需要重新编译程序。