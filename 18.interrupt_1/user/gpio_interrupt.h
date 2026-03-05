#ifndef __GPIO_INTERRUPT_H__
#define __GPIO_INTERRUPT_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/gpio.h>
#include <linux/interrupt.h> 
#include <linux/irq.h>


#define MAGIC 'G'

#define TIMER_START _IO(MAGIC, 0)
#define TIMER_STOP _IO(MAGIC, 1)
#define TIMER_SET_PERIOD _IOW(MAGIC, 2, int)

#define BUFFER_SIZE 512

typedef struct
{
    char name[20];                /* 设备名称 */
    dev_t device_number;          /* 设备号 */
    uint32_t device_major_number; /* 主设备号 */
    uint32_t device_minor_number; /* 次设备号 */

    struct cdev char_device_t;    /* 字符设备结构体 */
    struct class *device_class_p; /* 设备类指针，用于创建设备类 */
    struct device *device_node_p; /* 设备节点指针，用于在/dev下创建设备节点 */

    char kernel_buffer[BUFFER_SIZE]; /*  */
} char_device_handle;

#endif /* __GPIO_INTERRUPT_H__ */
