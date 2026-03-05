#include "libled.h"

int led_toggle(int fd)
{
    return ioctl(fd, LED_TOGGLE);
}