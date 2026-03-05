#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <atomic64.h>
#include "led.h"
#include "io_mapping.h"


/**
 * @brief 字符设备句柄结构体
 *
 * 该结构体用于存储字符设备的相关信息，包括设备编号、主设备号、次设备号、设备类指针、设备节点指针、字符设备结构体和文件操作结构体。
 */
typedef struct
{
    char device_name[32];         /* 设备名称 */
    dev_t device_number;          /* 设备号 */
    uint32_t device_major_number; /* 主设备号 */
    uint32_t device_minor_number; /* 次设备号 */

    struct cdev char_device_t;    /* 字符设备结构体 */
    struct class *device_class_p; /* 设备类指针，用于创建设备类 */
    struct device *device_node_p; /* 设备节点指针，用于在/dev下创建设备节点 */

    char kernel_buffer[64];               /* 内核缓冲区，用于存储设备数据 */
    void *io_base_addr_p;                 /* IO映射基地址，用于访问硬件寄存器 */
    atomic64_t open_count;                /* 字符设备打开计数，防止并发访问 */
    int flag_of_poll;                     /* 状态标志位, 表示是否有数据可读, 用于poll函数 */
    struct fasync_struct *fasync_queue_p; /* 异步通知队列，用于信号驱动IO */

} char_device_handle;

#endif /* __LED_DRIVER_H__ */
