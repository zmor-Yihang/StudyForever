# 设置目标架构为ARM64
export ARCH=arm64
# 设置交叉编译工具链，与主项目保持一致
export CROSS_COMPILE=/home/zmor/Linux/rk3568_linux_5.10/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-rockchip1031-linux-gnu-

# 编译器设置
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# 目标文件
TARGETS = reader_arm64 writer_arm64

# 源文件
SRCS = reader.c writer.c

# 对象文件
OBJS = reader_arm64.o writer_arm64.o

.PHONY: all clean

all: $(TARGETS)

# 编译reader程序
reader_arm64: reader.c
	@echo "编译ARM64版本的reader程序..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "编译完成！输出文件: $@"

# 编译writer程序
writer_arm64: writer.c
	@echo "编译ARM64版本的writer程序..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "编译完成！输出文件: $@"

# 清理生成的文件
clean:
	@echo "清理ARM64编译输出..."
	rm -f $(OBJS) $(TARGETS)
	@echo "清理完成！"

# 安装到系统（需要root权限）
install: all
	cp reader_arm64 /usr/local/bin/
	cp writer_arm64 /usr/local/bin/

# 卸载
uninstall:
	rm -f /usr/local/bin/reader_arm64
	rm -f /usr/local/bin/writer_arm64