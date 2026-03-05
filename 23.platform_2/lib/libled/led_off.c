#include "libled.h"

int led_off(int fd)
{
    return ioctl(fd, LED_OFF);
}