#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include "../../lib/libtimer/libtimer.h"

#define DEVICE_PATH "/dev/llseek_device"
#define TEST_PERIOD_1 1000  // 1秒
#define TEST_PERIOD_2 500   // 0.5秒
#define TEST_PERIOD_3 2000  // 2秒

void print_separator(const char* title)
{
    printf("\n");
    printf("========================================\n");
    printf("    %s\n", title);
    printf("========================================\n");
}

void test_timer_open(int fd)
{
    print_separator("测试开启定时器");
    
    printf("设备文件描述符: %d\n", fd);
    printf("调用 timer_open() 开启定时器...\n");
    
    timer_open(fd);
    printf("\n定时器已开启\n");
}

void test_timer_set_period(int fd)
{
    print_separator("测试设置定时器周期");
    
    // 测试设置周期为1秒
    printf("设置定时器周期为 %d 毫秒...\n", TEST_PERIOD_1);
    timer_set_period(fd, TEST_PERIOD_1);
    
    // 等待一段时间
    printf("等待3秒观察定时器效果...\n");
    sleep(3);
    
    // 测试设置周期为0.5秒
    printf("\n设置定时器周期为 %d 毫秒...\n", TEST_PERIOD_2);
    timer_set_period(fd, TEST_PERIOD_2);
    
    // 等待一段时间
    printf("等待2秒观察定时器效果...\n");
    sleep(2);
    
    // 测试设置周期为2秒
    printf("\n设置定时器周期为 %d 毫秒...\n", TEST_PERIOD_3);
    timer_set_period(fd, TEST_PERIOD_3);
    
    // 等待一段时间
    printf("等待4秒观察定时器效果...\n");
    sleep(4);
}

void test_timer_close(int fd)
{
    print_separator("测试停止定时器");
    
    printf("调用 timer_close() 停止定时器...\n");
    timer_close(fd);
    printf("\n定时器已停止\n");
}

void test_timer_operations_sequence(int fd)
{
    print_separator("测试定时器操作序列");
    
    printf("1. 开启定时器\n");
    timer_open(fd);
    
    printf("\n2. 设置周期为500毫秒\n");
    timer_set_period(fd, 500);
    
    printf("等待2秒...\n");
    sleep(2);
    
    printf("\n3. 重新设置周期为1000毫秒\n");
    timer_set_period(fd, 1000);
    
    printf("等待3秒...\n");
    sleep(3);
    
    printf("\n4. 停止定时器\n");
    timer_close(fd);
    
    printf("\n定时器操作序列测试完成\n");
}

void test_error_handling(int fd)
{
    int result;
    
    print_separator("测试错误处理");
    
    printf("注意: 以下测试可能会产生错误信息，这是正常的\n");
    
    // 测试无效的周期值
    printf("\n尝试设置无效的周期值 (-1):\n");
    result = timer_set_period(fd, -1);
    printf(result == 0 ? "成功设置\n" : "设置失败（符合预期）\n");
    
    // 测试极大的周期值
    printf("\n尝试设置极大的周期值 (999999):\n");
    result = timer_set_period(fd, 999999);
    printf(result == 0 ? "成功设置\n" : "设置失败（符合预期）\n");
    
    // 测试正常周期值
    printf("\n尝试设置正常的周期值 (500):\n");
    result = timer_set_period(fd, 500);
    printf(result == 0 ? "成功设置\n" : "设置失败\n");
}

int main()
{
    int fd;
    int choice = 0;
    
    printf("libtimer 库测试程序\n");
    printf("===================\n");
    
    // 打开设备文件
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        printf("无法打开设备 %s: %s\n", DEVICE_PATH, strerror(errno));
        printf("请确保定时器驱动已加载并且设备节点存在\n");
        return -1;
    }
    
    printf("成功打开设备: %s (文件描述符: %d)\n", DEVICE_PATH, fd);
    
    while (1) {
        printf("\n请选择测试项目:\n");
        printf("1. 测试开启定时器\n");
        printf("2. 测试设置定时器周期\n");
        printf("3. 测试停止定时器\n");
        printf("4. 测试定时器操作序列\n");
        printf("5. 测试错误处理\n");
        printf("6. 运行所有测试\n");
        printf("0. 退出\n");
        printf("请输入选择: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("输入无效，请重试\n");
            while (getchar() != '\n'); // 清除输入缓冲区
            continue;
        }
        
        switch (choice) {
            case 1:
                test_timer_open(fd);
                break;
            case 2:
                test_timer_set_period(fd);
                break;
            case 3:
                test_timer_close(fd);
                break;
            case 4:
                test_timer_operations_sequence(fd);
                break;
            case 5:
                test_error_handling(fd);
                break;
            case 6:
                printf("\n运行所有测试...\n");
                test_timer_open(fd);
                test_timer_set_period(fd);
                test_timer_close(fd);
                test_timer_operations_sequence(fd);
                test_error_handling(fd);
                printf("\n所有测试完成\n");
                break;
            case 0:
                printf("退出测试程序\n");
                close(fd);
                return 0;
            default:
                printf("无效选择，请重试\n");
                break;
        }
    }
    
    close(fd);
    return 0;
}