#ifndef __LIB_TIMER_H__
#define __LIB_TIMER_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

#define MAGIC 'G'

#define TIMER_START _IO(MAGIC, 0)
#define TIMER_STOP _IO(MAGIC, 1)
#define TIMER_SET_PERIOD _IOW(MAGIC, 2, int)

void timer_open(int fd);
void timer_close(int fd);
int timer_set_period(int fd, int period);

#endif /* __LIB_TIMER_H__ */
