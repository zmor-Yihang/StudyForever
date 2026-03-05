#include "libled0.h"

int led0_set(int fd, int state)
{
    return ioctl(fd, LED0_SET, &state);
}