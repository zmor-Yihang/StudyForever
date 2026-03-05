#include "libled.h"

int led_set(int fd, int state)
{
    return ioctl(fd, LED_SET, &state);
}