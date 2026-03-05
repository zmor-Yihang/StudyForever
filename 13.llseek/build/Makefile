# 从 Kbuild 文件中引入项目配置
include Kbuild

# 设置目标架构为ARM64
ARCH := arm64

# 设置交叉编译工具链前缀，用于在x86主机上编译ARM64代码
CROSS_COMPILE := /home/zmor/Linux/rk3568_linux_5.10/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-rockchip1031-linux-gnu-

# 设置内核源码目录路径，指向RK3568开发板的内核源码
KDIR := /home/zmor/Linux/rk3568_linux_5.10/kernel

# 获取当前工作目录（build目录）
PWD ?= $(shell pwd)
# 源文件所在目录（user目录）
SRC_DIR := $(PWD)/../user

# 设置输出目录
OUTPUT_DIR := $(PWD)/arm64

# 编译目标：编译内核模块
all:
	@echo "创建输出目录: $(OUTPUT_DIR)"
	@mkdir -p $(OUTPUT_DIR)
	@echo "开始编译ARM64内核模块..."
	@make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules
	@echo "移动编译输出到 $(OUTPUT_DIR) 目录..."
	@mv $(PWD)/*.o $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(SRC_DIR)/*.o $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.ko $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.mod $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.mod.c $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/*.mod.o $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/Module.symvers $(OUTPUT_DIR)/ 2>/dev/null || true
	@mv $(PWD)/modules.order $(OUTPUT_DIR)/ 2>/dev/null || true
	@echo "清理中间文件..."
	@rm -f $(PWD)/.*.cmd
	@rm -f $(SRC_DIR)/.*.cmd
	@echo "编译完成！输出文件在 $(OUTPUT_DIR) 目录中"

# 清理目标：删除编译生成的文件
clear:
	@echo "清理ARM64编译输出..."
	@rm -rf $(OUTPUT_DIR)
	@rm -rf $(SRC_DIR)/.*.cmd $(SRC_DIR)/*.o
	@rm -f $(PWD)/.*.cmd $(PWD)/*.o $(PWD)/*.ko $(PWD)/*.mod $(PWD)/*.mod.c $(PWD)/Module.symvers $(PWD)/modules.order
	@echo "清理完成！"
