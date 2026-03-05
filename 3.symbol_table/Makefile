# 设置源文件名和目标模块名
TARGET_NAME := symboltable
SOURCE_FILE := $(TARGET_NAME).c
KBUILD_EXTRA_SYMBOLS := /home/zmor/nuaa/2.param/build_x86/Module.symvers

# 设置目标架构为ARM64
export ARCH=arm64
# 设置交叉编译工具链前缀，用于在x86主机上编译ARM64代码
export CROSS_COMPILE=/home/zmor/Linux/rk3568_linux_5.10/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-rockchip1031-linux-gnu-

# 告诉内核构建系统要编译源文件，生成内核模块
obj-m += $(TARGET_NAME).o

# 设置内核源码目录路径，指向RK3568开发板的内核源码
KDIR := /home/zmor/Linux/rk3568_linux_5.10/kernel

# 获取当前工作目录（即驱动源码所在目录）
PWD ?= $(shell pwd)

# 设置输出目录
OUTPUT_DIR := $(PWD)/build

# 添加C99标准支持，覆盖内核默认的gnu89标准
ccflags-y := -std=gnu99

# 编译目标：编译内核模块
# -C：切换到内核源码目录执行make
# M=$(PWD)：告诉内核构建系统驱动源码的位置
# modules：编译模块（.ko文件）
all:
	@echo "创建输出目录: $(OUTPUT_DIR)"
	@mkdir -p $(OUTPUT_DIR)
	@echo "开始编译内核模块..."
	make -C $(KDIR) M=$(PWD) modules
	@echo "移动编译输出到 $(OUTPUT_DIR) 目录..."
	@mv $(PWD)/*.o $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.ko $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.mod $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.mod.c $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.mod.o $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/Module.symvers $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/modules.order $(OUTPUT_DIR)/ 2>/dev/null || true
	@echo "清理中间文件..."
	@rm -f $(PWD)/.*.cmd
	@echo "编译完成！输出文件在 $(OUTPUT_DIR) 目录中"

# 清理目标：删除编译生成的文件
clean:
	@echo "清理编译输出..."
	@rm -rf $(OUTPUT_DIR)
	@make -C $(KDIR) M=$(PWD) clean
	@echo "清理完成！"

# 显示编译输出目录
show:
	@echo "编译输出目录: $(OUTPUT_DIR)"
	@echo "当前源文件: $(PWD)/$(SOURCE_FILE)"
	@echo "目标模块名: $(TARGET_NAME).ko"
	@ls -la $(OUTPUT_DIR) 2>/dev/null || echo "目录不存在，请先编译"