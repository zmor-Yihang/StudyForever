#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_PATH_LED0 "/dev/cdev_led0"
#define DEVICE_PATH_LED1 "/dev/cdev_led1"
#define INVALID_DEVICE_PATH "/dev/nonexistent_device"

void test_normal_operation();
void test_invalid_device_path();
void test_device_open_close();
void test_read_write_errors();
void test_null_buffer_access();

int main() {
    printf("==========================================\n");
    printf("字符设备驱动错误处理测试程序\n");
    printf("==========================================\n\n");

    // 测试正常的设备操作
    printf("1. 测试正常设备操作...\n");
    test_normal_operation();
    
    // 测试无效设备路径
    printf("\n2. 测试无效设备路径...\n");
    test_invalid_device_path();
    
    // 测试设备打开和关闭
    printf("\n3. 测试设备打开和关闭...\n");
    test_device_open_close();
    
    // 测试读写错误
    printf("\n4. 测试读写错误...\n");
    test_read_write_errors();
    
    // 测试空缓冲区访问
    printf("\n5. 测试空缓冲区访问...\n");
    test_null_buffer_access();
    
    printf("\n==========================================\n");
    printf("所有错误处理测试完成!\n");
    printf("==========================================\n");
    
    return 0;
}

void test_normal_operation() {
    int fd;
    char buffer[64];
    ssize_t bytes_read, bytes_written;
    
    // 打开设备0
    fd = open(DEVICE_PATH_LED0, O_RDWR);
    if (fd == -1) {
        printf("  [FAIL] 无法打开设备 %s: %s\n", DEVICE_PATH_LED0, strerror(errno));
        return;
    }
    printf("  [PASS] 成功打开设备 %s\n", DEVICE_PATH_LED0);
    
    // 读取初始状态
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        printf("  [FAIL] 读取设备失败: %s\n", strerror(errno));
    } else {
        buffer[bytes_read] = '\0';
        printf("  [PASS] 成功读取设备数据: %s\n", buffer);
    }
    
    // 写入新数据
    const char* test_data = "on";
    bytes_written = write(fd, test_data, strlen(test_data));
    if (bytes_written == -1) {
        printf("  [FAIL] 写入设备失败: %s\n", strerror(errno));
    } else {
        printf("  [PASS] 成功写入设备数据: %s (%zd 字节)\n", test_data, bytes_written);
    }
    
    // 再次读取验证
    lseek(fd, 0, SEEK_SET); // 重置文件位置指针
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        printf("  [FAIL] 二次读取设备失败: %s\n", strerror(errno));
    } else {
        buffer[bytes_read] = '\0';
        printf("  [PASS] 验证读取数据: %s\n", buffer);
    }
    
    // 关闭设备
    if (close(fd) == -1) {
        printf("  [FAIL] 关闭设备失败: %s\n", strerror(errno));
    } else {
        printf("  [PASS] 成功关闭设备\n");
    }
}

void test_invalid_device_path() {
    int fd;
    
    // 尝试打开不存在的设备
    fd = open(INVALID_DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        printf("  [PASS] 正确拒绝打开不存在的设备 %s: %s\n", INVALID_DEVICE_PATH, strerror(errno));
    } else {
        printf("  [FAIL] 错误地打开了不存在的设备\n");
        close(fd);
    }
}

void test_device_open_close() {
    int fd1, fd2;
    
    // 打开同一设备两次
    fd1 = open(DEVICE_PATH_LED0, O_RDWR);
    if (fd1 == -1) {
        printf("  [FAIL] 无法打开设备 %s: %s\n", DEVICE_PATH_LED0, strerror(errno));
        return;
    }
    printf("  [PASS] 第一次成功打开设备 %s\n", DEVICE_PATH_LED0);
    
    fd2 = open(DEVICE_PATH_LED0, O_RDWR);
    if (fd2 == -1) {
        printf("  [FAIL] 无法再次打开设备 %s: %s\n", DEVICE_PATH_LED0, strerror(errno));
    } else {
        printf("  [PASS] 第二次成功打开设备 %s\n", DEVICE_PATH_LED0);
        close(fd2);
    }
    
    // 关闭第一个文件描述符
    if (close(fd1) == -1) {
        printf("  [FAIL] 关闭设备失败: %s\n", strerror(errno));
    } else {
        printf("  [PASS] 成功关闭设备\n");
    }
}

void test_read_write_errors() {
    int fd;
    char buffer[64];
    ssize_t result;
    
    // 打开设备
    fd = open(DEVICE_PATH_LED1, O_RDWR);
    if (fd == -1) {
        printf("  [FAIL] 无法打开设备 %s: %s\n", DEVICE_PATH_LED1, strerror(errno));
        return;
    }
    printf("  [PASS] 成功打开设备 %s\n", DEVICE_PATH_LED1);
    
    // 测试写入超长数据
    char long_data[1000];
    memset(long_data, 'A', sizeof(long_data) - 1);
    long_data[sizeof(long_data) - 1] = '\0';
    
    result = write(fd, long_data, sizeof(long_data) - 1);
    if (result == -1) {
        printf("  [INFO] 写入超长数据失败（预期行为）: %s\n", strerror(errno));
    } else {
        printf("  [INFO] 写入超长数据成功，实际写入 %zd 字节\n", result);
    }
    
    // 测试读取大量数据
    result = read(fd, buffer, sizeof(buffer));
    if (result == -1) {
        printf("  [FAIL] 读取设备失败: %s\n", strerror(errno));
    } else {
        printf("  [PASS] 成功读取 %zd 字节数据\n", result);
    }
    
    // 关闭设备
    if (close(fd) == -1) {
        printf("  [FAIL] 关闭设备失败: %s\n", strerror(errno));
    } else {
        printf("  [PASS] 成功关闭设备\n");
    }
}

void test_null_buffer_access() {
    int fd;
    ssize_t result;
    
    // 打开设备
    fd = open(DEVICE_PATH_LED0, O_RDWR);
    if (fd == -1) {
        printf("  [FAIL] 无法打开设备 %s: %s\n", DEVICE_PATH_LED0, strerror(errno));
        return;
    }
    printf("  [PASS] 成功打开设备 %s\n", DEVICE_PATH_LED0);
    
    // 注意：在用户空间程序中直接测试内核空间的空指针访问比较困难
    // 我们可以通过其他方式间接测试错误处理
    
    // 测试零长度读写
    result = write(fd, "test", 0);
    if (result == -1) {
        printf("  [INFO] 零长度写入失败: %s\n", strerror(errno));
    } else {
        printf("  [INFO] 零长度写入返回 %zd\n", result);
    }
    
    result = read(fd, NULL, 10);
    if (result == -1) {
        printf("  [INFO] 读取到空缓冲区失败（预期行为）: %s\n", strerror(errno));
    } else {
        printf("  [WARN] 读取到空缓冲区竟然成功了，返回 %zd\n", result);
    }
    
    // 关闭设备
    if (close(fd) == -1) {
        printf("  [FAIL] 关闭设备失败: %s\n", strerror(errno));
    } else {
        printf("  [PASS] 成功关闭设备\n");
    }
}