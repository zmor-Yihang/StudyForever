CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = test_llseek
SOURCE = test_llseek.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

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