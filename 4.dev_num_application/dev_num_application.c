/*
 * 设备号申请示例内核模块
 *
 * 本模块演示了Linux内核中设备号的申请和释放，包括：
 * 1. 静态申请设备号：使用register_chrdev_region()函数
 * 2. 动态申请设备号：使用alloc_chrdev_region()函数
 * 3. 设备号的组成：主设备号和次设备号
 *
 * 使用方法：
 * 1. 动态申请设备号（默认）：sudo insmod dev_num_application.ko
 * 2. 静态申请设备号：sudo insmod dev_num_application.ko first_dev_num=250 second_dev_num=0
 * 3. 查看设备号：dmesg | tail
 * 4. 卸载模块：sudo rmmod dev_num_application
 */

#include <linux/init.h>   /* 初始化函数相关宏定义 */
#include <linux/module.h> /* 模块加载和卸载相关函数 */
#include <linux/fs.h>     /* 文件系统相关函数和结构体 */
#include <linux/kdev_t.h> /* 设备号类型定义和相关宏 */

/* 模块参数：主设备号，默认为0（表示动态分配） */
static int first_dev_num = 0;
/* 模块参数：次设备号，默认为0 */
static int second_dev_num = 0;

/* 注册模块参数，允许用户在加载模块时指定设备号 */
module_param(first_dev_num, int, S_IRUGO);
module_param(second_dev_num, int, S_IRUGO);

/* 全局设备号变量，用于存储申请到的设备号 */
dev_t dev_num;

/*
 * 模块初始化函数
 * 在模块加载时调用，负责申请设备号
 * 返回值：成功返回0，失败返回负数
 */
static int __init dev_num_application_init(void)
{
    int ret = 0;

    printk(KERN_INFO "dev_number_application example!\n");

    /*
     * 根据用户是否指定主设备号，选择静态或动态申请方式
     * first_dev_num为0表示使用动态分配，非0表示使用静态分配
     */
    if (first_dev_num)
    {
        /* MKDEV宏将主设备号和次设备号合并为一个32位的设备号 */
        dev_num = MKDEV(first_dev_num, second_dev_num);
        printk(KERN_INFO "dev_num is %d\n", dev_num);

        /* 静态注册字符设备号：使用用户指定的设备号 */
        ret = register_chrdev_region(dev_num, 1, "dev_number_application_example_static");
        if (ret < 0)
        {
            printk(KERN_ERR "register_chrdev_region failed!\n");
            return ret;
        }
        printk(KERN_INFO "register_chrdev_region success!\n");
    }
    else
    {
        /* 动态注册字符设备号：由内核自动分配可用的设备号 */
        ret = alloc_chrdev_region(&dev_num, 0, 1, "dev_number_application_example_dynamic");
        if (ret < 0)
        {
            printk(KERN_ERR "alloc_chrdev_region failed!\n");
            return ret;
        }
        printk(KERN_INFO "alloc_chrdev_region success!\n");

        /* 从分配的设备号中提取主设备号和次设备号 */
        first_dev_num = MAJOR(dev_num);
        second_dev_num = MINOR(dev_num);
        printk(KERN_INFO "first_dev_num is %d, second_dev_num is %d\n", first_dev_num, second_dev_num);
        printk(KERN_INFO "dev_num is %d\n", dev_num);
    }

    return 0;
}

/*
 * 模块退出函数
 * 在模块卸载时调用，负责释放申请的设备号
 */
static void __exit dev_num_application_exit(void)
{
    printk(KERN_INFO "Goodbye dev_number_application example!\n");
    /* 释放申请的设备号，参数：设备号、连续设备号数量 */
    unregister_chrdev_region(dev_num, 1);
}

/* 注册模块初始化和退出函数 */
module_init(dev_num_application_init);
module_exit(dev_num_application_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A dev_number_application module for demonstrating device number allocation");
