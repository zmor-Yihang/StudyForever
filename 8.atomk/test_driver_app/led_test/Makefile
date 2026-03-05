# 设置目标架构为ARM64
export ARCH=arm64
# 设置交叉编译工具链，与主项目保持一致
export CROSS_COMPILE=/home/zmor/Linux/rk3568_linux_5.10/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-rockchip1031-linux-gnu-

# 编译器设置
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = led_test_arm64
SOURCE = led_test.c

# 默认目标
all: $(TARGET)

# 编译目标文件
$(TARGET): $(SOURCE)
	@echo "编译ARM64版本的测试程序..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "编译完成！输出文件: $@"

# 清理生成的文件
clean:
	@echo "清理ARM64编译输出..."
	@rm -f $(TARGET)
	@echo "清理完成！"

.PHONY: all clean
