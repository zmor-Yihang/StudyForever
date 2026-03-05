#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024
#define DEVICE_PATH "/dev/led0" // 设备文件路径

// 全局变量，用于信号处理函数中访问文件描述符
volatile sig_atomic_t fd = -1;
volatile sig_atomic_t got_signal = 0;

// 信号处理函数
void io_signal_handler(int signo)
{
    (void)signo; // 避免未使用参数警告
    got_signal = 1;
}

int main(void)
{
    struct sigaction sa;
    char buffer[BUFFER_SIZE];
    int ret;
    int flags;
    
    // 打开设备文件
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd == -1) {
        perror("无法打开设备文件");
        exit(EXIT_FAILURE);
    }
    
    // 设置文件所有者为当前进程，以便接收信号
    if (fcntl(fd, F_SETOWN, getpid()) == -1) {
        perror("无法设置文件所有者");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // 获取当前文件状态标志
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("无法获取文件状态标志");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // 设置FASYNC标志，启用异步通知
    if (fcntl(fd, F_SETFL, flags | FASYNC) == -1) {
        perror("无法设置FASYNC标志");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // 设置信号处理函数
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = io_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // 重启被中断的系统调用
    
    // 捕获SIGIO信号
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("无法设置SIGIO信号处理");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    printf("信号驱动IO读程序已启动，等待LED状态变化...\n");
    printf("按Ctrl+C退出程序\n");
    printf("提示: 写入'on'、'off'或'toggle'命令来控制LED并触发信号\n");
    
    while (1) {
        // 如果没有收到信号，就休眠等待
        if (!got_signal) {
            pause(); // 暂停进程，等待信号
            continue;
        }
        
        // 重置信号标志
        got_signal = 0;
        
        // 读取数据
        memset(buffer, 0, BUFFER_SIZE);
        ret = read(fd, buffer, BUFFER_SIZE - 1);
        
        if (ret == -1) {
            perror("读取数据失败");
            close(fd);
            exit(EXIT_FAILURE);
        }
        else if (ret == 0) {
            printf("设备文件已关闭\n");
            break;
        }
        
        printf("收到信号，读取到 %d 字节: %s\n", ret, buffer);
        printf("LED状态已更新\n");
    }
    
    close(fd);
    printf("程序结束\n");
    return 0;
}