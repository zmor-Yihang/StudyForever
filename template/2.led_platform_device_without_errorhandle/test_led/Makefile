# ARM64平台编译配置

# 交叉编译工具链
CROSS_COMPILE := /home/zmor/Linux/rk3568_linux_5.10/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-rockchip1031-linux-gnu-

# 编译器和链接器
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

# 编译选项
CFLAGS := -Wall -g -I../lib/libled0 -I../user

# 目标文件
TARGET := test_led_platform
SRCS := test_led_platform.c ../lib/libled0/led0_on.c ../lib/libled0/led0_off.c ../lib/libled0/led0_toggle.c ../lib/libled0/led0_set.c ../lib/libled0/led0_get.c
OBJS := $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ 

# 编译源文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理目标
clean:
	rm -f $(OBJS) $(TARGET)

# 安装到目标目录
install: $(TARGET)
	cp $(TARGET) /home/zmor/nuaa/template/2.led_platform_device/build/arm64/

.PHONY: all clean install