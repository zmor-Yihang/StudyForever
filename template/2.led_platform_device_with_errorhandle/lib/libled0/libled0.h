#ifndef __LIB_LED0_H__
#define __LIB_LED0_H__

#include <sys/ioctl.h>

#define MAGICNUM 'Z'

#define LED0_ON _IO(MAGICNUM, 0)
#define LED0_OFF _IO(MAGICNUM, 1)
#define LED0_TOGGLE _IO(MAGICNUM, 2)

#define LED0_SET _IOW(MAGICNUM, 3, int)
#define LED0_GET _IOR(MAGICNUM, 4, int)

int led0_on(int fd);
int led0_off(int fd);
int led0_toggle(int fd);
int led0_set(int fd, int state);
int led0_get(int fd, int *state);

#endif /* __LIB_LED0_H__ */
