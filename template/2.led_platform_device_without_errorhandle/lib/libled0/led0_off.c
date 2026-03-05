#include "libled0.h"

int led0_off(int fd)
{
    return ioctl(fd, LED0_OFF);
}