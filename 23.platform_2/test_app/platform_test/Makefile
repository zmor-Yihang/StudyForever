# Makefile for platform LED test application

# 编译器设置
CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -Wextra -std=c99

# 交叉编译工具链路径
TOOLCHAIN_PATH = /home/zmor/Linux/rk3568_linux_5.10/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin
export PATH := $(TOOLCHAIN_PATH):$(PATH)

# 目录设置
SRC_DIR = .
LIB_DIR = ../../lib/libled
BUILD_DIR = build

# 源文件
SOURCES = $(SRC_DIR)/test_platform_led.c
LIB_SOURCES = $(LIB_DIR)/led_on.c $(LIB_DIR)/led_off.c $(LIB_DIR)/led_toggle.c $(LIB_DIR)/led_set.c $(LIB_DIR)/led_get.c

# 目标文件
TARGET = $(BUILD_DIR)/test_platform_led
LIB_OBJECTS = $(LIB_SOURCES:$(LIB_DIR)/%.c=$(BUILD_DIR)/%.o)

# 头文件目录
INCLUDES = -I$(LIB_DIR)

# 默认目标
all: $(TARGET)

# 创建构建目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 编译目标程序
$(TARGET): $(BUILD_DIR) $(LIB_OBJECTS) $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) $(LIB_OBJECTS) -o $@

# 编译库对象文件
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 清理编译生成的文件
clean:
	rm -rf $(BUILD_DIR)

# 安装目标（可选）
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# 卸载目标（可选）
uninstall:
	rm -f /usr/local/bin/test_platform_led

# 显示帮助信息
help:
	@echo "可用目标:"
	@echo "  all       - 编译测试程序（默认）"
	@echo "  clean     - 清理编译生成的文件"
	@echo "  install   - 安装测试程序到系统"
	@echo "  uninstall - 从系统卸载测试程序"
	@echo "  help      - 显示此帮助信息"

.PHONY: all clean install uninstall help