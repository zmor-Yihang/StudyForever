#!/bin/bash

# LED驱动重新加载脚本
# 功能：卸载已加载的模块 -> 清理 -> 编译 -> 重新加载

PROJECT_NAME="LED"
DEVICE_NAME="cdev_led0"
MODULE_FILE="build_x86/${PROJECT_NAME}.ko"

echo "========================================="
echo "  LED驱动重新加载脚本"
echo "========================================="

# 检查是否以root权限运行
if [ "$EUID" -ne 0 ]; then
  echo "警告: 此脚本需要root权限才能正确卸载/加载内核模块"
  echo "建议使用 sudo 运行此脚本"
  echo ""
fi

# 1. 卸载已加载的模块
echo "步骤1: 检查并卸载已加载的${PROJECT_NAME}模块..."
if lsmod | grep -q "^${PROJECT_NAME} "; then
    echo "发现已加载的${PROJECT_NAME}模块，正在卸载..."
    if sudo rmmod ${PROJECT_NAME}; then
        echo "成功卸载${PROJECT_NAME}模块"
    else
        echo "错误: 无法卸载${PROJECT_NAME}模块"
        exit 1
    fi
else
    echo "未发现已加载的${PROJECT_NAME}模块"
fi

# 2. 清理之前的编译结果
echo ""
echo "步骤2: 清理之前的编译结果..."
if make -f Makefilex86 clean; then
    echo "清理完成"
else
    echo "警告: 清理过程中出现错误"
fi

# 3. 重新编译模块
echo ""
echo "步骤3: 重新编译内核模块..."
if make -f Makefilex86 all; then
    echo "编译成功完成"
else
    echo "错误: 编译失败"
    exit 1
fi

# 检查模块文件是否存在
if [ ! -f "${MODULE_FILE}" ]; then
    echo "错误: 模块文件 ${MODULE_FILE} 未找到"
    exit 1
fi

# 4. 加载新编译的模块
echo ""
echo "步骤4: 加载新编译的模块..."
if sudo insmod ${MODULE_FILE}; then
    echo "成功加载${PROJECT_NAME}模块"
else
    echo "错误: 无法加载${PROJECT_NAME}模块"
    exit 1
fi

# 5. 验证模块是否加载成功
echo ""
echo "步骤5: 验证模块状态..."
if lsmod | grep -q "^${PROJECT_NAME} "; then
    echo "验证成功: ${PROJECT_NAME}模块已正常加载"
    
    # 显示模块信息
    MODULE_INFO=$(lsmod | grep "^${PROJECT_NAME} ")
    echo "模块信息: $MODULE_INFO"
    
    # 显示最近的内核日志
    echo ""
    echo "最近的内核日志:"
    dmesg | tail -10
    
    # 检查设备节点是否创建
    DEVICE_PATH="/dev/${DEVICE_NAME}"
    echo ""
    if [ -e "$DEVICE_PATH" ]; then
        echo "设备节点 $DEVICE_PATH 已创建"
        ls -l $DEVICE_PATH
    else
        echo "注意: 设备节点 $DEVICE_PATH 尚未创建"
        echo "可能需要手动创建或检查驱动初始化代码"
    fi
else
    echo "验证失败: ${PROJECT_NAME}模块未正常加载"
    exit 1
fi

echo ""
echo "========================================="
echo "  驱动重新加载完成!"
echo "========================================="