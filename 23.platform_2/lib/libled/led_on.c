#include "libled.h"

int led_on(int fd)
{
    return ioctl(fd, LED_ON);
}