#include "char_dev_register.h"

/**
 * @brief 字符设备开启函数
 * @param inode inode指针
 * @param filp 文件指针
 * @return int 0表示成功
 */
static int cdev_open(struct inode *inode, struct file *filp)
{
    struct cdev *cdev = inode->i_cdev;
    cdev_handle *handle = container_of(cdev, cdev_handle, char_device_t);

    filp->private_data = handle;
    return 0;
}
/**
 * @brief 字符设备测试关闭函数
 *
 * @param inode inode指针
 * @param filp 文件指针
 * @return int 0表示成功
 */
static int cdev_release(struct inode *inode, struct file *filp)
{
    /* 从文件指针的private_data字段中获取字符设备结构体指针 */
    cdev_handle *device_handle = filp->private_data;
    return 0;
}
/**
 * @brief 字符设备测试读取函数
 *
 * @param filp 文件指针
 * @param buf 用户空间缓冲区指针
 * @param count 要读取的字节数
 * @param f_pos 文件当前位置指针
 * @return ssize_t 实际读取的字节数
 */
static ssize_t cdev_read(struct file *filp, char __user *user_read_buffer, size_t count, loff_t *f_pos)
{
    /* 从文件指针的private_data字段中获取字符设备结构体指针 */
    cdev_handle *device_handle = filp->private_data;

    /* 计算内核缓冲区的实际长度 */
    size_t len = strlen(device_handle->kernel_buffer);

    /* 确定要读取的字节数：取用户请求的字节数和内核缓冲区长度的较小值 */
    size_t to_read = min(count, len);

    /* 将内核空间的数据复制到用户空间缓冲区
     * copy_to_user返回非0值表示复制失败
     * 这是内核与用户空间数据交互的标准方法
     */
    if (copy_to_user(user_read_buffer, device_handle->kernel_buffer, to_read))
    {
        /* 复制失败，返回错误码-EFAULT表示内存访问错误 */
        return -EFAULT;
    }

    /* 返回实际读取的字节数 */
    return to_read;
}

/**
 * @brief 字符设备测试写入函数
 *
 * @param filp 文件指针
 * @param buf 用户空间缓冲区指针
 * @param count 要写入的字节数
 * @param f_pos 文件当前位置指针
 * @return ssize_t 实际写入的字节数
 */
static ssize_t cdev_write(struct file *filp, const char __user *user_write_buffer, size_t count, loff_t *f_pos)
{
    /* 从文件指针的private_data字段中获取字符设备结构体指针 */
    cdev_handle *device_handle = filp->private_data;

    size_t len;

    /* 限制写入大小，为字符串终止符预留空间 */
    len = min(count, sizeof(device_handle->kernel_buffer) - 1);

    // 检查copy_from_user返回值
    if (copy_from_user(device_handle->kernel_buffer, user_write_buffer, len))
    {
        return -EFAULT; // 复制失败
    }

    // 添加字符串终止符
    device_handle->kernel_buffer[len] = '\0';

    return len; // 返回实际写入的字节数
}

/* 临时存储字符设备号 */
static dev_t cdev_base;

/* 设备类指针, 用于在/dev下创建设备目录, 所有设备实例共享 */
static struct class *cdev_class_p = NULL;

/* 该结构体定义了字符设备的文件操作函数指针，包括打开、关闭、读取、写入等操作 */
static struct file_operations cdev_fops = {
    .owner = THIS_MODULE,
    .open = cdev_open,
    .release = cdev_release,
    .read = cdev_read,
    .write = cdev_write,
};

/* 字符设备句柄动态数组 */
static cdev_handle *hcdev = NULL;

static int __init char_dev_register_init(void)
{
    int ret = 0; /* 错误返回值 */
    int i;       /* 循环索引 */

    /* 分配字符设备句柄数组内存 */
    hcdev = kzalloc(sizeof(cdev_handle) * CDEV_NUM, GFP_KERNEL);
    if (!hcdev)
    {
        ret = -ENOMEM;
        goto err_out;
    }

    /* 1.动态申请字符设备号 */
    ret = alloc_chrdev_region(&cdev_base, 0, CDEV_NUM, "cdev_led");
    if (ret)
        goto err_free_hcdev; /* 此时hcdev数组内存申请成功, 需要释放 */

    /* 2.初始化字符设备句柄 */
    for (i = 0; i < CDEV_NUM; i++)
    {
        /* 初始化设备结构体 */
        hcdev[i].char_device_t.owner = THIS_MODULE; /* 字符设备结构体的owner字段指向当前模块 */

        /* 初始化设备号 */
        hcdev[i].device_number = MKDEV(MAJOR(cdev_base), MINOR(cdev_base) + i);

        /* 初始化内核缓冲区 */
        strcpy(hcdev[i].kernel_buffer, "off");

        /* 初始化字符设备结构体 */
        cdev_init(&hcdev[i].char_device_t, &cdev_fops);
    }

    /* 3.添加字符设备到内核 */
    for (i = 0; i < CDEV_NUM; i++)
    {
        ret = cdev_add(&hcdev[i].char_device_t, hcdev[i].device_number, 1);
        if (ret)
        {
            goto err_cleanup_cdev_and_cdevnum;
        }
    }

    /* 4.创建设备类, 该API在不同的Linux内核参数数量不一样, 就是在/sys/class
       下创建设备类目录/sys/class/cdev_led_class, 之后的设备节点挂载到这个目录下 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cdev_class_p = class_create("cdev_led_class"); /* 新版内核不需要MODULE参数 */
#else
    cdev_class_p = class_create(THIS_MODULE, "cdev_led_class");
#endif
    if (IS_ERR(cdev_class_p))
    {
        ret = PTR_ERR(cdev_class_p);
        goto err_cleanup_cdevs; /* 复用 err_cleanup_cdev_and_cdevnum 的处理逻辑*/
    }

    /* 5.创建设备节点 */
    for (i = 0; i < CDEV_NUM; i++)
    {
        /* 使用device_create创建设备节点, 设备节点挂载到设备类文件夹/sys/class/cdev_led_class,
           设备节点存在句柄 hcdev[i].device_node_p 中 */
        hcdev[i].device_node_p = device_create(cdev_class_p, NULL, hcdev[i].device_number, NULL, "cdev_led%d", i);
        if (IS_ERR(hcdev[i].device_node_p))
        {
            ret = PTR_ERR(hcdev[i].device_node_p);
            goto err_cleanup_devnode_and_devclass;
        }
    }

    return 0;

    /* ========= 错误回滚 ========= */

err_cleanup_devnode_and_devclass:
    /* 删除已创建的设备节点 */
    for (i = 0; i < CDEV_NUM; i++)
    {
        /* 只有当设备节点指针不为NULL且不是错误指针时，才销毁设备节点 */
        if (hcdev[i].device_node_p && !IS_ERR(hcdev[i].device_node_p))
            device_destroy(cdev_class_p, hcdev[i].device_number);
    }
    /* 删除设备类 */
    class_destroy(cdev_class_p);

err_cleanup_cdevs:
err_cleanup_cdev_and_cdevnum:
    /* 删除已创建的字符设备, 释放设备号 */
    for (i = 0; i < CDEV_NUM; i++)
    {
        /* 只有当设备的ops字段不为NULL时，才表示设备已成功添加到内核 */
        if (hcdev[i].char_device_t.ops)
            cdev_del(&hcdev[i].char_device_t);
    }
    unregister_chrdev_region(cdev_base, CDEV_NUM);

err_free_hcdev:
    kfree(hcdev);
    hcdev = NULL;

err_out:
    printk(KERN_ERR "char dev init failed: %d\n", ret);
    return ret;
}

static void __exit char_dev_register_exit(void)
{
    for (int i = 0; i < CDEV_NUM; i++)
    {
        /* 删除设备节点 */
        device_destroy(cdev_class_p, hcdev[i].device_number);

        /* 删除字符设备 */
        cdev_del(&hcdev[i].char_device_t);
    }

    /* 删除类 */
    class_destroy(cdev_class_p);

    /* 删除设备号 */
    unregister_chrdev_region(cdev_base, CDEV_NUM);

    /* 释放内存 */
    kfree(hcdev);

    /* 清空指针, 防止使用已释放的内存 */
    hcdev = NULL;
    cdev_class_p = NULL;

    printk("Goodbye cdev example!\n");
}

module_init(char_dev_register_init);
module_exit(char_dev_register_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A cdev module");
