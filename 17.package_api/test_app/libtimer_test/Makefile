CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = test_libtimer
SOURCE = test_libtimer.c
LIB_PATH = ../../lib/libtimer
INCLUDE_PATH = ../../lib/libtimer
LIB_SOURCES = $(LIB_PATH)/timer_open.c $(LIB_PATH)/timer_close.c $(LIB_PATH)/timer_set_period.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE) $(LIB_SOURCES)
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -o $(TARGET) $(SOURCE) $(LIB_SOURCES)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

help:
	@echo "使用方法:"
	@echo "make         - 编译测试程序"
	@echo "make clean   - 清理编译文件"
	@echo "make install - 安装到系统路径"
	@echo "make uninstall - 从系统路径卸载"
	@echo ""
	@echo "注意: 运行测试前请确保:"
	@echo "1. 定时器驱动已加载"
	@echo "2. 设备节点 /dev/timer_device 存在"
	@echo "3. 有访问设备节点的权限"