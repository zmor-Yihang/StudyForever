#!/bin/bash

# LED测试脚本

DEVICE="/dev/cdev_led0"

# 默认使用x86版本
ARCH="x86"
PROGRAM="./led_test_x86"

# 检查命令行参数
if [ $# -gt 0 ]; then
    if [ "$1" = "arm64" ]; then
        ARCH="arm64"
        PROGRAM="./led_test_arm64"
    elif [ "$1" = "x86" ]; then
        ARCH="x86"
        PROGRAM="./led_test_x86"
    else
        echo "Error: Unknown architecture '$1'. Use 'arm64' or 'x86'."
        exit 1
    fi
fi

echo "LED Driver Test Script ($ARCH version)"
echo "=============================="

# 检查设备是否存在
if [ ! -e "$DEVICE" ]; then
    echo "Error: Device $DEVICE not found!"
    echo "Please make sure the driver is loaded."
    exit 1
fi

# 检查测试程序是否存在
if [ ! -x "$PROGRAM" ]; then
    echo "Warning: Test program $PROGRAM not found or not executable!"
    if [ "$ARCH" = "arm64" ]; then
        echo "Attempting to compile ARM64 version..."
        make
        if [ $? -ne 0 ]; then
            echo "Error: Failed to compile ARM64 version!"
            echo "Please check the compilation errors above."
            exit 1
        fi
    else
        echo "Attempting to compile x86 version..."
        make -f Makefilex86
        if [ $? -ne 0 ]; then
            echo "Error: Failed to compile x86 version!"
            echo "Please check the compilation errors above."
            exit 1
        fi
    fi
    
    # 再次检查程序是否存在
    if [ ! -x "$PROGRAM" ]; then
        echo "Error: Compilation completed but $PROGRAM still not found or not executable!"
        exit 1
    fi
    echo "Compilation successful!"
fi

echo "Device: $DEVICE"
echo "Program: $PROGRAM"
echo ""

# 显示帮助信息
show_help() {
    echo "Available commands:"
    echo "  on      - Turn LED on"
    echo "  off     - Turn LED off"
    echo "  toggle  - Toggle LED state"
    echo "  help    - Show this help"
    echo "  quit    - Exit script"
    echo ""
}

# 主循环
show_help
while true; do
    read -p "Enter command (on/off/toggle/help/quit): " cmd
    
    case $cmd in
        on|off|toggle)
            echo "Executing: $PROGRAM $cmd"
            $PROGRAM $cmd
            echo ""
            ;;
        help)
            show_help
            ;;
        quit)
            echo "Exiting..."
            break
            ;;
        *)
            echo "Unknown command: $cmd"
            show_help
            ;;
    esac
done