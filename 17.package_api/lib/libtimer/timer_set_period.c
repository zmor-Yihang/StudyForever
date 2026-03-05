#include "libtimer.h"

int timer_set_period(int fd, int period)
{
    int ret = ioctl(fd, TIMER_SET_PERIOD, &period);
    if (ret < 0)
    {
        printf("设置定时器周期失败: %d 毫秒 (错误码: %d)\n", period, ret);
        return -1;
    }
    printf("成功设置定时器周期为 %d 毫秒\n", period);
    return 0;
}

