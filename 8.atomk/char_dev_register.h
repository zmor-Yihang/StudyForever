#ifndef __CHAR_DEV_REGISTER_H__
#define __CHAR_DEV_REGISTER_H__

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
#include <linux/delay.h>
#include "led.h"
#include "io_mapping.h"

/* 字符设备数量 */
#define CDEV_NUM 1

/**
 * @brief 字符设备句柄结构体
 *
 * 该结构体用于存储字符设备的相关信息，包括设备编号、主设备号、次设备号、设备类指针、设备节点指针、字符设备结构体和文件操作结构体。
 */
typedef struct
{
    struct cdev char_device_t;    /* 字符设备结构体 */
    dev_t device_number;          /* 设备号，包含主设备号和次设备号的32位整数 */
    struct device *device_node_p; /* 设备节点指针，用于在/dev下创建设备节点 */
    char kernel_buffer[64];       /* 内核缓冲区，用于存储设备数据 */
    void *io_base_addr;           /* IO映射基地址，用于访问硬件寄存器 */
    atomic64_t open_count;        /* 字符设备打开计数，防止并发访问 */
} cdev_handle;

#endif /* __CHAR_DEV_REGISTER_H__ */
