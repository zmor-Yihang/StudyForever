#ifndef __LIB_LED_H__
#define __LIB_LED_H__

// 用户空间应用程序需要的头文件
#ifdef __KERNEL__
// 内核空间不需要包含用户空间头文件
#else
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#endif

#define MAGICNUM 'Z'

#define LED_ON _IO(MAGICNUM, 0)
#define LED_OFF _IO(MAGICNUM, 1)
#define LED_TOGGLE _IO(MAGICNUM, 2)

#define LED_SET _IOW(MAGICNUM, 3, int)
#define LED_GET _IOR(MAGICNUM, 4, int)


int led_on(int fd);
int led_off(int fd);
int led_toggle(int fd);
int led_set(int fd, int state);
int led_get(int fd, int *state);

#endif /* __LIB_LED_H__ */
