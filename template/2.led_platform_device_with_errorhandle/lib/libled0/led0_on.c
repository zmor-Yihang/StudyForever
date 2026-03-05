#include "libled0.h"

int led0_on(int fd)
{
    return ioctl(fd, LED0_ON);
}