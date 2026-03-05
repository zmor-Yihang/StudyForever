#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/led0"

int main() {
    int fd;
    char buffer[256];
    ssize_t bytes_read;
    
    printf("Blocking IO Reader Test\n");
    printf("Opening device: %s\n", DEVICE_PATH);
    
    // 打开设备文件
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }
    
    printf("Device opened successfully. Waiting for data...\n");
    
    // 读取数据（这将阻塞直到有数据可读）
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
        close(fd);
        return -1;
    }
    
    // 添加字符串终止符
    buffer[bytes_read] = '\0';
    
    printf("Received %zd bytes: %s\n", bytes_read, buffer);
    
    /**/
    // 关闭设备文件
    close(fd);
    printf("Device closed.\n");
    
    return 0;
}