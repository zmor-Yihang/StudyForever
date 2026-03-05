#include "libled.h"

int led_get(int fd, int *state)
{
    return ioctl(fd, LED_GET, state);
}