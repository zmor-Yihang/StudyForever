#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include "ioctl_cmds.h"

#define DEVICE_PATH "/dev/llseek_device"
#define BUFFER_SIZE 1024

void test_reset_buffer(int fd)
{
    printf("\n=== 测试重置缓冲区 (IOCTL_RESET_BUFFER) ===\n");

    // 先写入一些数据
    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);
    char test_data[] = "Test data for reset buffer";
    write(fd, test_data, strlen(test_data));
    printf("写入测试数据: %s\n", test_data);

    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);

    // 读取数据验证
    char read_buf[BUFFER_SIZE] = {0};
    read(fd, read_buf, strlen(test_data));
    printf("读取到的数据: %s\n", read_buf);

    // 调用ioctl重置缓冲区
    int ret = ioctl(fd, IOCTL_RESET_BUFFER);
    if (ret < 0)
    {
        perror("IOCTL_RESET_BUFFER 失败");
        return;
    }
    printf("成功重置缓冲区\n");

    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);

    // 再次读取数据验证缓冲区已清空
    memset(read_buf, 0, sizeof(read_buf));
    read(fd, read_buf, strlen(test_data));
    printf("重置后读取到的数据: %s\n", read_buf);
}

void test_get_buffer_size(int fd)
{
    printf("\n=== 测试获取缓冲区大小 (IOCTL_GET_BUFFER_SIZE) ===\n");

    int buffer_size = 0;
    int ret = ioctl(fd, IOCTL_GET_BUFFER_SIZE, &buffer_size);
    if (ret < 0)
    {
        perror("IOCTL_GET_BUFFER_SIZE 失败");
        return;
    }
    printf("获取到的缓冲区大小: %d 字节\n", buffer_size);
}

void test_clear_buffer(int fd)
{
    printf("\n=== 测试清空缓冲区 (IOCTL_CLEAR_BUFFER) ===\n");

    // 先写入一些数据
    lseek(fd, 0, SEEK_SET);
    char test_data[] = "Test data for clear buffer";
    write(fd, test_data, strlen(test_data));
    printf("写入测试数据: %s\n", test_data);

    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);

    // 读取数据验证
    char read_buf[BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(fd, read_buf, strlen(test_data));
    read_buf[bytes_read] = '\0';
    printf("读取到的数据: %s\n", read_buf);

    // 调用ioctl清空缓冲区
    int ret = ioctl(fd, IOCTL_CLEAR_BUFFER);
    if (ret < 0)
    {
        perror("IOCTL_CLEAR_BUFFER 失败");
        return;
    }
    printf("成功清空缓冲区\n");

    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);

    // 再次读取数据验证缓冲区已清空
    memset(read_buf, 0, sizeof(read_buf));
    bytes_read = read(fd, read_buf, strlen(test_data));
    read_buf[bytes_read] = '\0';
    printf("清空后读取到的数据: '%s' (长度: %zd)\n", read_buf, bytes_read);
    printf("注意: 缓冲区已被清空，所以读取到的数据为空是正确的\n");
}

void test_set_position(int fd)
{
    printf("\n=== 测试设置文件位置 (IOCTL_SET_POSITION) ===\n");

    // 先写入一些数据
    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);
    char test_data[] = "0123456789";
    write(fd, test_data, strlen(test_data));
    printf("写入测试数据: %s\n", test_data);

    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);

    // 使用ioctl设置位置到5
    int position = 5;
    int ret = ioctl(fd, IOCTL_SET_POSITION, &position);
    if (ret < 0)
    {
        perror("IOCTL_SET_POSITION 失败");
        return;
    }
    printf("通过ioctl设置文件位置到: %d\n", position);

    // 验证当前位置
    off_t current_pos = lseek(fd, 0, SEEK_CUR);
    printf("当前文件位置: %ld\n", current_pos);

    // 读取数据验证位置
    char read_buf[BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(fd, read_buf, 5);
    read_buf[bytes_read] = '\0';
    printf("从新位置读取%zd字节数据: %s\n", bytes_read, read_buf);
}

void test_get_position(int fd)
{
    printf("\n=== 测试获取文件位置 (IOCTL_GET_POSITION) ===\n");

    // 先写入一些数据
        // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);
    char test_data[] = "0123456789";
    write(fd, test_data, strlen(test_data));
    printf("写入测试数据: %s\n", test_data);


    // 使用lseek设置位置
    lseek(fd, 2, SEEK_SET);
    printf("使用lseek设置文件位置到: 2\n");

    // 使用ioctl获取当前位置
    int position = 0;
    int ret = ioctl(fd, IOCTL_GET_POSITION, &position);
    if (ret < 0)
    {
        perror("IOCTL_GET_POSITION 失败");
        return;
    }
    printf("通过ioctl获取到的文件位置: %d\n", position);

    // 验证当前位置
    off_t current_pos = lseek(fd, 0, SEEK_CUR);
    printf("当前文件位置: %ld\n", current_pos);

    // 读取数据验证位置
    char read_buf[BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(fd, read_buf, 5);
    read_buf[bytes_read] = '\0';
    printf("从当前位置读取%zd字节数据: %s\n", bytes_read, read_buf);
}

void test_fill_buffer(int fd)
{
    printf("\n=== 测试填充缓冲区 (IOCTL_FILL_BUFFER) ===\n");

    // 使用ioctl填充缓冲区
    char fill_char = 'A';
    int ret = ioctl(fd, IOCTL_FILL_BUFFER, &fill_char);
    if (ret < 0)
    {
        perror("IOCTL_FILL_BUFFER 失败");
        return;
    }
    printf("使用字符 '%c' 填充缓冲区\n", fill_char);

    // 重置文件位置到开头
    lseek(fd, 0, SEEK_SET);

    // 读取数据验证
    char read_buf[21] = {0}; // 只读取前20个字符用于验证
    read(fd, read_buf, 20);
    printf("读取前20个字符: %s\n", read_buf);
}

void test_transf_struct(int fd)
{
    printf("\n=== 测试传输结构体 (IOCTL_TRANSF_STRUCT) ===\n");

    // 使用ioctl获取结构体数据
    trandata_t data_received;
    int ret = ioctl(fd, IOCTL_TRANSF_STRUCT, &data_received);
    if (ret < 0)
    {
        perror("IOCTL_TRANSF_STRUCT 失败");
        return;
    }
    
    printf("成功接收到结构体数据:\n");
    printf("  cmd: %s\n", data_received.cmd);
    printf("  length: %d\n", data_received.length);
}

void test_invalid_ioctl(int fd)
{
    printf("\n=== 测试无效ioctl命令 ===\n");

    // 尝试使用无效的ioctl命令
    int ret = ioctl(fd, _IO('Z', 99), NULL);
    if (ret < 0)
    {
        printf("无效ioctl命令返回错误，这是正确的: %s\n", strerror(errno));
    }
    else
    {
        printf("警告: 无效ioctl命令没有返回错误\n");
    }
}

int main()
{
    int fd;

    printf("=== IOCTL 设备驱动测试程序 ===\n");

    // 打开设备
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0)
    {
        perror("无法打开设备 " DEVICE_PATH);
        printf("请确保:\n");
        printf("1. 驱动模块已加载 (sudo insmod LLSEEK_ZMOR.ko)\n");
        printf("2. 设备节点已创建 (ls -l /dev/llseek_device)\n");
        printf("3. 有权限访问设备 (sudo chmod 666 /dev/llseek_device)\n");
        return EXIT_FAILURE;
    }

    printf("成功打开设备 %s\n", DEVICE_PATH);

    // 执行测试
    test_get_buffer_size(fd);
    test_fill_buffer(fd);
    test_reset_buffer(fd);
    test_clear_buffer(fd);
    test_set_position(fd);
    test_get_position(fd);
    test_transf_struct(fd);
    test_invalid_ioctl(fd);

    // 关闭设备
    close(fd);
    printf("\n测试完成\n");

    return EXIT_SUCCESS;
}