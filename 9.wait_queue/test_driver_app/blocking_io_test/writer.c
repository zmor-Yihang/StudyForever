#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/cdev_led0"

int main(int argc, char *argv[]) {
    int fd;
    ssize_t bytes_written;
    char *message = "on";
    
    // 如果提供了命令行参数，则使用它作为要写入的消息
    if (argc > 1) {
        message = argv[1];
    }
    
    printf("Blocking IO Writer Test\n");
    printf("Opening device: %s\n", DEVICE_PATH);
    
    // 打开设备文件
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }
    
    printf("Device opened successfully. Writing message: %s\n", message);
    
    // 写入数据
    bytes_written = write(fd, message, strlen(message));
    if (bytes_written < 0) {
        perror("Write failed");
        close(fd);
        return -1;
    }
    
    printf("Wrote %zd bytes to device\n", bytes_written);
    
    // 关闭设备文件
    close(fd);
    printf("Device closed.\n");
    
    return 0;
}