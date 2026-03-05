#include "libtimer.h"

void timer_open(int fd)
{
    int ret = ioctl(fd, TIMER_START);

    if (ret < 0)
    {
        printf("TIEMR START ERROR!\n");
        exit(-1);
    }
    printf("成功开启定时器");
}
