#!/bin/bash

echo "=================================="
echo "字符设备驱动错误处理测试脚本"
echo "=================================="

# 检查驱动是否已加载
echo "检查驱动模块是否已加载..."
if lsmod | grep -q "char_dev_register"; then
    echo "✓ 字符设备驱动已加载"
else
    echo "✗ 字符设备驱动未加载，正在尝试加载..."
    cd ..
    sudo make -f Makefilex86 install
    if [ $? -eq 0 ]; then
        echo "✓ 驱动模块加载成功"
    else
        echo "✗ 驱动模块加载失败"
        exit 1
    fi
fi

# 检查设备节点是否存在
echo "检查设备节点..."
if [ -e "/dev/cdev_led0" ] && [ -e "/dev/cdev_led1" ]; then
    echo "✓ 设备节点已创建"
    ls -l /dev/cdev_led*
else
    echo "✗ 设备节点缺失"
    exit 1
fi

# 运行错误处理测试
echo "运行错误处理测试..."
cd test_driver_app
sudo ./error_handling_test

echo "测试完成!"