#include "libled0.h"

int led0_toggle(int fd)
{
    return ioctl(fd, LED0_TOGGLE);
}