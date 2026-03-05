#!/bin/bash

# 一键重新编译和安装字符设备驱动模块
# 该脚本会自动卸载现有模块，清理编译文件，重新编译并安装新模块

echo "==============================="
echo "  字符设备驱动一键重新安装脚本  "
echo "==============================="

# 检查是否以root权限运行
if [ "$EUID" -ne 0 ]; then
  echo "警告: 此脚本部分操作需要root权限，可能需要输入密码"
fi

# 1. 卸载现有模块
echo ""
echo "步骤 1: 卸载现有驱动模块..."
cd "$(dirname "$0")"
if lsmod | grep -q "char_dev_register"; then
    sudo rmmod char_dev_register
    if [ $? -eq 0 ]; then
        echo "  ✓ 驱动模块已成功卸载"
    else
        echo "  ✗ 卸载驱动模块时出错"
    fi
else
    echo "  - 驱动模块未加载，跳过卸载步骤"
fi

# 2. 清理编译文件
echo ""
echo "步骤 2: 清理旧的编译文件..."
make -f Makefilex86 clean >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✓ 编译文件清理完成"
else
    echo "  ! 清理过程中出现警告（可忽略）"
fi

# 3. 重新编译驱动
echo ""
echo "步骤 3: 重新编译驱动模块..."
make -f Makefilex86 all
if [ $? -eq 0 ]; then
    echo "  ✓ 驱动模块编译成功"
else
    echo "  ✗ 驱动模块编译失败"
    exit 1
fi

# 4. 安装新模块
echo ""
echo "步骤 4: 安装新的驱动模块..."
sudo make -f Makefilex86 install
if [ $? -eq 0 ]; then
    echo "  ✓ 驱动模块安装成功"
else
    echo "  ✗ 驱动模块安装失败"
    exit 1
fi

# 5. 验证设备节点
echo ""
echo "步骤 5: 验证设备节点..."
devices=("cdev_led0" "cdev_led1")
for device in "${devices[@]}"; do
    if [ -e "/dev/$device" ]; then
        echo "  ✓ 设备节点 /dev/$device 已创建"
        ls -l "/dev/$device"
    else
        echo "  ✗ 设备节点 /dev/$device 未找到"
    fi
done

# 6. 显示内核日志信息
echo ""
echo "步骤 6: 显示最近的内核日志..."
sudo dmesg | tail -5

echo ""
echo "==============================="
echo "  驱动模块已成功重新安装!       "
echo "  您现在可以运行测试程序了      "
echo "==============================="
echo "运行测试程序命令:"
echo "  sudo ./test_driver_app/test_driver"
echo ""