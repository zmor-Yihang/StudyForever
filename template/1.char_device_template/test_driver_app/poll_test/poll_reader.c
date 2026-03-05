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
    
    // 打开设备文件
    fd = open(DEVICE_PATH, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("无法打开设备文件");
        exit(EXIT_FAILURE);
    }
    
    // 设置poll结构
    fds[0].fd = fd;
    fds[0].events = POLLIN; // 等待可读事件
    
    printf("poll IO多路复用读程序已启动，等待LED状态变化...\n");
    printf("按Ctrl+C退出程序\n");
    printf("提示: 写入'on'、'off'或'toggle'命令来控制LED并触发读取事件\n");
    
    while (1) {
        // 调用poll等待事件
        ret = poll(fds, 1, TIMEOUT);
        
        if (ret == -1) {
            perror("poll调用失败");
            close(fd);
            exit(EXIT_FAILURE);
        }
        else if (ret == 0) {
            printf("超时，没有数据可读\n");
            continue;
        }
        
        // 检查是否有数据可读
        if (fds[0].revents & POLLIN) {
            memset(buffer, 0, BUFFER_SIZE);
            ret = read(fd, buffer, BUFFER_SIZE - 1);
            
            if (ret == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf("暂时没有数据可读\n");
                    continue;
                } else {
                    perror("读取数据失败");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
            }
            else if (ret == 0) {
                printf("设备文件已关闭\n");
                break;
            }
            
            printf("读取到 %d 字节: %s\n", ret, buffer);
            printf("LED状态已更新\n");
        }
        
        // 检查是否有错误
        if (fds[0].revents & POLLERR) {
            fprintf(stderr, "设备文件发生错误\n");
            break;
        }
        
        // 检查是否已挂断(仅对某些设备有效)
        if (fds[0].revents & POLLHUP) {
            printf("设备文件已挂断\n");
            break;
        }
    }
    
    close(fd);
    printf("程序结束\n");
    return 0;
}