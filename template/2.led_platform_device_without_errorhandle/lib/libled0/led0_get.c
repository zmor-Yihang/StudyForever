#include "libled0.h"

int led0_get(int fd, int *state)
{
    return ioctl(fd, LED0_GET, state);
}