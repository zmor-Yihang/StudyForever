#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define DEVICE_PATH "/dev/cdev_led0" // 设备文件路径

int main(void)
{
    int fd;
    char buffer[BUFFER_SIZE];
    int bytes_written;
    
    // 打开设备文件
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd == -1) {
        perror("无法打开设备文件");
        exit(EXIT_FAILURE);
    }
    
    printf("信号驱动IO写程序已启动\n");
    printf("输入'on'、'off'或'toggle'命令控制LED，按Enter发送，输入'quit'退出程序\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        // 从标准输入读取数据
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        // 检查是否要退出
        if (strncmp(buffer, "quit", 4) == 0) {
            break;
        }
        
        // 去除换行符
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // 检查是否是有效命令
        if (strcmp(buffer, "on") != 0 && strcmp(buffer, "off") != 0 && strcmp(buffer, "toggle") != 0) {
            printf("无效命令! 请输入'on'、'off'或'toggle'\n");
            continue;
        }
        
        // 写入数据
        bytes_written = write(fd, buffer, strlen(buffer));
        
        if (bytes_written == -1) {
            perror("写入数据失败");
            close(fd);
            exit(EXIT_FAILURE);
        }
        
        printf("成功发送 '%s' 命令到设备\n", buffer);
    }
    
    close(fd);
    printf("程序结束\n");
    return 0;
}