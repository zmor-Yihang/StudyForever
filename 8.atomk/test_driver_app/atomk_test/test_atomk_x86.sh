#!/bin/bash

# 原子性测试脚本 (x86版本)

DEVICE="/dev/cdev_led0"

# 默认使用x86版本
ARCH="x86"
PROGRAM="./atomk_test_x86"

# 检查命令行参数
if [ $# -gt 0 ]; then
    if [ "$1" = "arm64" ]; then
        ARCH="arm64"
        PROGRAM="./atomk_test_arm64"
    elif [ "$1" = "x86" ]; then
        ARCH="x86"
        PROGRAM="./atomk_test_x86"
    else
        echo "Error: Unknown architecture '$1'. Use 'arm64' or 'x86'."
        exit 1
    fi
fi

echo "原子性防止并发访问测试脚本 ($ARCH version)"
echo "======================================"

# 检查设备是否存在
if [ ! -e "$DEVICE" ]; then
    echo "错误: 设备节点 $DEVICE 不存在"
    echo "请确保驱动已加载并创建了设备节点"
    exit 1
fi

# 检查是否有权限访问设备
if [ ! -r "$DEVICE" ] || [ ! -w "$DEVICE" ]; then
    echo "警告: 可能没有权限访问设备节点 $DEVICE"
    echo "尝试使用sudo运行此脚本"
    
    # 尝试使用sudo
    if command -v sudo >/dev/null 2>&1; then
        echo "使用sudo重新运行..."
        exec sudo "$0" "$@"
    else
        echo "错误: sudo不可用，无法获取权限"
        exit 1
    fi
fi

# 检查测试程序是否存在
if [ ! -x "$PROGRAM" ]; then
    echo "警告: 测试程序 $PROGRAM 不存在或不可执行!"
    if [ "$ARCH" = "arm64" ]; then
        echo "尝试编译ARM64版本..."
        make
        if [ $? -ne 0 ]; then
            echo "错误: 编译ARM64版本失败!"
            echo "请检查上面的编译错误"
            exit 1
        fi
    else
        echo "尝试编译x86版本..."
        make -f Makefilex86
        if [ $? -ne 0 ]; then
            echo "错误: 编译x86版本失败!"
            echo "请检查上面的编译错误"
            exit 1
        fi
    fi
    
    # 再次检查程序是否存在
    if [ ! -x "$PROGRAM" ]; then
        echo "错误: 编译完成但 $PROGRAM 仍不存在或不可执行!"
        exit 1
    fi
    echo "编译成功!"
fi

echo "设备: $DEVICE"
echo "程序: $PROGRAM"
echo ""

# 运行测试
echo "运行原子性测试..."
echo "注意: 此测试将创建多个线程尝试同时访问设备"
echo ""

$PROGRAM

echo ""
echo "======================================"
echo "测试完成"
echo "======================================"

# 询问是否清理编译文件
read -p "是否清理编译文件? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    if [ "$ARCH" = "arm64" ]; then
        make clean
    else
        make -f Makefilex86 clean
    fi
    echo "编译文件已清理"
fi