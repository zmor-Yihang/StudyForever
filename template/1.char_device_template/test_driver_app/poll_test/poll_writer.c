#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define TIMEOUT 10000 // 10秒超时
#define DEVICE_PATH "/dev/led0" // 设备文件路径

int main(void)
{
    int fd;
    struct pollfd fds[1];
    char buffer[BUFFER_SIZE];
    int ret;
    int bytes_written;
    
    // 打开设备文件
    fd = open(DEVICE_PATH, O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("无法打开设备文件");
        exit(EXIT_FAILURE);
    }
    
    // 设置poll结构
    fds[0].fd = fd;
    fds[0].events = POLLOUT; // 等待可写事件
    
    printf("poll IO多路复用写程序已启动\n");
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
        
        // 调用poll等待设备可写
        ret = poll(fds, 1, TIMEOUT);
        
        if (ret == -1) {
            perror("poll调用失败");
            close(fd);
            exit(EXIT_FAILURE);
        }
        else if (ret == 0) {
            printf("超时，设备不可写\n");
            continue;
        }
        
        // 检查是否可写
        if (fds[0].revents & POLLOUT) {
            bytes_written = write(fd, buffer, strlen(buffer));
            
            if (bytes_written == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf("设备暂时不可写\n");
                    continue;
                } else {
                    perror("写入数据失败");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
            }
            
            printf("成功发送 '%s' 命令到设备\n", buffer);
        }
        
        // 检查是否有错误
        if (fds[0].revents & POLLERR) {
            fprintf(stderr, "设备文件发生错误\n");
            break;
        }
    }
    
    close(fd);
    printf("程序结束\n");
    return 0;
}