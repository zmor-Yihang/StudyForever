#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEVICE_PATH "/dev/llseek_device"
#define BUFFER_SIZE 1024

void test_basic_operations(int fd)
{
    char write_buf[] = "Hello, this is a test string for llseek device!";
    char read_buf[BUFFER_SIZE] = {0};
    ssize_t bytes_written, bytes_read;
    off_t new_pos;
    
    printf("\n=== 基本读写测试 ===\n");
    
    // 写入数据
    bytes_written = write(fd, write_buf, strlen(write_buf));
    if (bytes_written < 0) {
        perror("写入失败");
        return;
    }
    printf("写入 %zd 字节: %s\n", bytes_written, write_buf);
    
    // 重置文件位置到开头
    new_pos = lseek(fd, 0, SEEK_SET);
    if (new_pos < 0) {
        perror("lseek 失败");
        return;
    }
    printf("文件位置重置到: %ld\n", new_pos);
    
    // 读取数据
    bytes_read = read(fd, read_buf, bytes_written);
    if (bytes_read < 0) {
        perror("读取失败");
        return;
    }
    read_buf[bytes_read] = '\0';
    printf("读取 %zd 字节: %s\n", bytes_read, read_buf);
}

void test_seek_operations(int fd)
{
    char write_buf[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char read_buf[BUFFER_SIZE] = {0};
    ssize_t bytes_written, bytes_read;
    off_t new_pos;
    
    printf("\n=== Seek 操作测试 ===\n");
    
    // 写入数据
    bytes_written = write(fd, write_buf, strlen(write_buf));
    printf("写入 %zd 字节\n", bytes_written);
    
    // 测试 SEEK_SET
    new_pos = lseek(fd, 10, SEEK_SET);
    printf("SEEK_SET 到位置 10: %ld\n", new_pos);
    
    // 读取5个字节
    bytes_read = read(fd, read_buf, 5);
    read_buf[bytes_read] = '\0';
    printf("从位置10读取5字节: %s\n", read_buf);
    
    // 测试 SEEK_CUR
    new_pos = lseek(fd, 5, SEEK_CUR);
    printf("SEEK_CUR 从当前位置向前5字节: %ld\n", new_pos);
    
    // 读取5个字节
    bytes_read = read(fd, read_buf, 5);
    read_buf[bytes_read] = '\0';
    printf("从新位置读取5字节: %s\n", read_buf);
    
    // 测试 SEEK_END
    new_pos = lseek(fd, -5, SEEK_END);
    printf("SEEK_END 到文件末尾前5字节: %ld\n", new_pos);
    
    // 读取最后5个字节
    bytes_read = read(fd, read_buf, 5);
    read_buf[bytes_read] = '\0';
    printf("读取最后5字节: %s\n", read_buf);
}

void test_boundary_conditions(int fd)
{
    char write_buf[BUFFER_SIZE * 2]; // 超过缓冲区大小的数据
    char read_buf[BUFFER_SIZE] = {0};
    ssize_t bytes_written, bytes_read;
    off_t new_pos;
    int i;
    
    printf("\n=== 边界条件测试 ===\n");
    
    // 初始化写入缓冲区
    for (i = 0; i < BUFFER_SIZE * 2; i++) {
        write_buf[i] = 'A' + (i % 26);
    }
    
    // 尝试写入超过缓冲区大小的数据
    bytes_written = write(fd, write_buf, BUFFER_SIZE * 2);
    printf("尝试写入 %d 字节，实际写入 %zd 字节\n", BUFFER_SIZE * 2, bytes_written);
    
    // 重置位置并读取全部数据
    new_pos = lseek(fd, 0, SEEK_SET);
    bytes_read = read(fd, read_buf, BUFFER_SIZE);
    printf("从开头读取 %zd 字节\n", bytes_read);
    
    // 尝试seek到超出缓冲区的位置
    new_pos = lseek(fd, BUFFER_SIZE + 10, SEEK_SET);
    if (new_pos < 0) {
        printf("尝试seek到超出缓冲区的位置失败，这是正确的\n");
    } else {
        printf("seek到位置 %ld\n", new_pos);
    }
    
    // 尝试从超出缓冲区的位置读取
    bytes_read = read(fd, read_buf, 10);
    printf("从超出缓冲区的位置尝试读取，返回 %zd 字节\n", bytes_read);
}

int main()
{
    int fd;
    
    printf("=== LLSEEK 设备驱动测试程序 ===\n");
    
    // 打开设备
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("无法打开设备 " DEVICE_PATH);
        printf("请确保:\n");
        printf("1. 驱动模块已加载 (sudo insmod LLSEEK_ZMOR.ko)\n");
        printf("2. 设备节点已创建 (ls -l /dev/llseek_dev)\n");
        printf("3. 有权限访问设备 (sudo chmod 666 /dev/llseek_dev)\n");
        return EXIT_FAILURE;
    }
    
    printf("成功打开设备 %s\n", DEVICE_PATH);
    
    // 执行测试
    test_basic_operations(fd);
    test_seek_operations(fd);
    test_boundary_conditions(fd);
    
    // 关闭设备
    close(fd);
    printf("\n测试完成\n");
    
    return EXIT_SUCCESS;
}