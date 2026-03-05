#include "libtimer.h"

void timer_close(int fd)
{
    int ret = ioctl(fd, TIMER_STOP);
    if (ret < 0)
    {
        printf("TIMER STOP ERROR!");
        exit(-1);
    }
    printf("成功停止定时器\n");
}

/*void test_timer_stop(int fd)
{
    printf("\n=== 测试停止定时器 (TIMER_STOP) ===\n");

    int ret = ioctl(fd, TIMER_STOP);
    if (ret < 0)
    {
        perror("TIMER_STOP 失败");
        return;
    }
    printf("成功停止定时器\n");
}*/