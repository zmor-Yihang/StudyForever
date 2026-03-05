#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include "test_ioctl_timer.h"

#define DEVICE_PATH "/dev/llseek_device"

void test_timer_start(int fd)
{
    printf("\n=== 测试启动定时器 (TIMER_START) ===\n");

    int ret = ioctl(fd, TIMER_START);
    if (ret < 0)
    {
        perror("TIMER_START 失败");
        return;
    }
    printf("成功启动定时器\n");
}

void test_timer_set_period(int fd)
{
    printf("\n=== 测试设置定时器周期 (TIMER_SET_PERIOD) ===\n");

    // 设置定时器周期为2000毫秒
    int period = 2000;
    int ret = ioctl(fd, TIMER_SET_PERIOD, &period);
    if (ret < 0)
    {
        perror("TIMER_SET_PERIOD 失败");
        return;
    }
    printf("成功设置定时器周期为 %d 毫秒\n", period);
    
    // 设置定时器周期为500毫秒
    period = 500;
    ret = ioctl(fd, TIMER_SET_PERIOD, &period);
    if (ret < 0)
    {
        perror("TIMER_SET_PERIOD 失败");
        return;
    }
    printf("成功设置定时器周期为 %d 毫秒\n", period);
}

void test_timer_stop(int fd)
{
    printf("\n=== 测试停止定时器 (TIMER_STOP) ===\n");

    int ret = ioctl(fd, TIMER_STOP);
    if (ret < 0)
    {
        perror("TIMER_STOP 失败");
        return;
    }
    printf("成功停止定时器\n");
}

int main()
{
    int fd;

    printf("=== IOCTL 定时器测试程序 ===\n");

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
    test_timer_start(fd);
    sleep(1); // 等待1秒观察定时器效果
    
    test_timer_set_period(fd);
    sleep(3); // 等待3秒观察定时器效果
    
    test_timer_stop(fd);
    sleep(1); // 等待1秒确认定时器已停止

    // 关闭设备
    close(fd);
    printf("\n定时器测试完成\n");

    return EXIT_SUCCESS;
}