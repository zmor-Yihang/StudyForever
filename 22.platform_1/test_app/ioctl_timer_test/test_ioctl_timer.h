#ifndef __TEST__IOCTL__TIMER__
#define __TEST__IOCTL__TIMER__

#define MAGIC 'G'

#define TIMER_START _IO(MAGIC, 0)
#define TIMER_STOP _IO(MAGIC, 1)
#define TIMER_SET_PERIOD _IOW(MAGIC, 2, int)

#endif /* __TEST__IOCTL__TIMER__ */
