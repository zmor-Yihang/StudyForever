#ifndef __PLATFORM_DRIVER_H__
#define __PLATFORM_DRIVER_H__

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
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include "lib/libled/libled.h"

#define BUFFER_SIZE 1024

/* 
 * GPIO数据寄存器写入格式说明：
 * - 高16位：写使能掩码，1表示对应位可写入
 * - 低16位：数据值，1表示置高电平，0表示置低电平
 * - LED连接在GPIO_B7引脚上，对应位为bit7
 * - GPIO_B7在GPIO0_B数据寄存器中，对应位为bit7 (0x8000)
 */
#define LED_BIT_MASK (1 << 15)  // GPIO_B7对应的掩码

typedef struct
{
    char name[20];                /* 设备名称 */
    dev_t device_number;          /* 设备号 */
    uint32_t device_major_number; /* 主设备号 */
    uint32_t device_minor_number; /* 次设备号 */

    struct cdev char_device_t;    /* 字符设备结构体 */
    struct class *device_class_p; /* 设备类指针，用于创建设备类 */
    struct device *device_node_p; /* 设备节点指针，用于在/dev下创建设备节点 */

    void __iomem *vir_mux_control;       // 复用控制寄存器虚拟地址
    void __iomem *vir_direction_control; // 方向控制寄存器虚拟地址
    void __iomem *vir_data;              // 数据寄存器虚拟地址

    char kernel_buffer[BUFFER_SIZE]; /* 字符设备缓冲区 */
} char_device_handle;

#endif /* __PLATFORM_DRIVER_H__ */
