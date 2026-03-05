#include "led_driver.h"

/* LED设备实例，LED句柄 */
char_device_handle led_handle;

/* 文件操作结构体声明，将在初始化时填充具体的函数指针 */
struct file_operations led_fops;

/* 虚拟地址映射指针，用于访问硬件寄存器 */
void __iomem *pmu_grf_base_addr = NULL; /* PMU GRF基地址虚拟映射，用于GPIO复用功能控制 */
void __iomem *gpio_0_base_addr = NULL;  /* GPIO_0基地址虚拟映射，用于GPIO数据读写 */

/* 定义等待队列头, 该队列被所有设备实例共享, 由内核维护
 * 当设备状态发生变化时，会唤醒在此队列上等待的进程
 * 这是实现poll/select系统调用和阻塞I/O的关键机制
 */
DECLARE_WAIT_QUEUE_HEAD(queue_head_of_device);

/**
 * @brief LED设备打开函数
 * 
 * 当应用程序通过open系统调用打开LED设备文件时，内核会调用此函数
 * 该函数负责初始化设备访问，并防止多个进程同时访问同一设备
 * 
 * @param inode 指向inode结构体的指针，包含文件的元数据信息
 * @param filp 指向file结构体的指针，表示打开的文件实例
 * @return int 成功返回0，失败返回负的错误码（如-EBUSY表示设备忙）
 */
static int led_open(struct inode *inode, struct file *filp)
{
    /* 从inode中获取cdev结构体指针 */
    struct cdev *cdev = inode->i_cdev;
    /* 使用container_of宏通过cdev指针获取包含它的char_device_handle结构体指针
     * 这是Linux内核中常用的通过结构体成员获取整个结构体的方法
     */
    char_device_handle *device_handle = container_of(cdev, char_device_handle, char_device_t);

    /* 检查设备是否已被打开
     * 使用原子操作读取open_count，确保线程安全
     * 当open_count为0时，表示设备已被打开，拒绝新的访问
     */
    if (atomic64_read(&device_handle->open_count) == 0)
    {
        printk(KERN_INFO "cdev_open: device %d:%d is already open\n",
               device_handle->device_major_number, device_handle->device_minor_number);
        return -EBUSY; /* 返回忙错误码，表示设备已被占用 */
    }

    /* 设备未被占用，减少打开计数，表示设备已被当前进程打开 */
    atomic64_dec(&device_handle->open_count);

    /* 将设备句柄指针保存到file结构的private_data字段中
     * 这样在后续的read、write、ioctl等操作中可以获取到设备信息
     */
    filp->private_data = device_handle;
    return 0; /* 返回成功 */
}

/**
 * @brief LED设备关闭函数
 * 
 * 当应用程序通过close系统调用关闭LED设备文件时，内核会调用此函数
 * 该函数负责释放设备资源，并允许其他进程访问设备
 *
 * @param inode 指向inode结构体的指针，包含文件的元数据信息
 * @param filp 指向file结构体的指针，表示打开的文件实例
 * @return int 成功返回0
 */
static int led_release(struct inode *inode, struct file *filp)
{
    /* 从文件指针的private_data字段中获取LED设备结构体指针 */
    char_device_handle *device_handle = filp->private_data;

    /* 增加打开计数，表示设备已被释放，其他进程可以访问 */
    atomic64_inc(&device_handle->open_count);

    /* 打印关闭信息，便于调试和监控设备使用情况 */
    printk(KERN_INFO "led_release: device %d:%d is closed\n", device_handle->device_major_number, device_handle->device_minor_number);

    return 0; /* 返回成功 */
}

/**
 * @brief LED设备读取函数
 *
 * 当应用程序通过read系统调用从LED设备读取数据时，内核会调用此函数
 * 该函数将内核缓冲区中的数据复制到用户空间缓冲区
 *
 * @param filp 文件指针，包含设备信息
 * @param user_read_buffer 用户空间缓冲区指针，用于接收读取的数据
 * @param count 用户请求读取的字节数
 * @param f_pos 文件当前位置指针，用于支持随机访问（本驱动中未使用）
 * @return ssize_t 实际读取的字节数，失败返回负的错误码
 */
static ssize_t led_read(struct file *filp, char __user *user_read_buffer, size_t count, loff_t *f_pos)
{
    /* 从文件指针的private_data字段中获取LED设备结构体指针 */
    char_device_handle *device_handle = filp->private_data;

    /* 计算内核缓冲区的实际长度（不包括字符串终止符） */
    size_t len = strlen(device_handle->kernel_buffer);

    /* 确定要读取的字节数：取用户请求的字节数和内核缓冲区长度的较小值
     * 这样可以防止读取超出缓冲区边界的数据
     */
    size_t to_read = min(count, len);

    /* 将内核空间的数据复制到用户空间缓冲区
     * copy_to_user返回非0值表示复制失败
     * 这是内核与用户空间数据交互的标准方法，确保数据安全传输
     */
    if (copy_to_user(user_read_buffer, device_handle->kernel_buffer, to_read))
    {
        /* 复制失败，返回错误码-EFAULT表示内存访问错误 */
        return -EFAULT;
    }
    
    /* 打印读取信息和缓冲区内容，便于调试和监控 */
    printk(KERN_INFO "cdev_read: read %zu bytes from device %d:%d, buffer content: %s\n",
           to_read, device_handle->device_major_number, device_handle->device_minor_number, device_handle->kernel_buffer);

    /* 重置状态标志位，表示数据已被读取
     * 这个标志位用于poll函数，表示当前是否有数据可读
     */
    device_handle->flag_of_poll = 0;

    /* 返回实际读取的字节数 */
    return to_read;
}

/**
 * @brief LED设备写入函数
 *
 * 当应用程序通过write系统调用向LED设备写入数据时，内核会调用此函数
 * 该函数将用户空间的数据复制到内核缓冲区，并根据写入的内容控制LED
 *
 * @param filp 文件指针，包含设备信息
 * @param user_write_buffer 用户空间缓冲区指针，包含要写入的数据
 * @param count 用户请求写入的字节数
 * @param f_pos 文件当前位置指针，用于支持随机访问（本驱动中未使用）
 * @return ssize_t 实际写入的字节数，失败返回负的错误码
 */
static ssize_t led_write(struct file *filp, const char __user *user_write_buffer, size_t count, loff_t *f_pos)
{
    /* 从文件指针的private_data字段中获取LED设备结构体指针 */
    char_device_handle *device_handle = filp->private_data;

    size_t len;

    /* 限制写入大小，为字符串终止符预留空间
     * 防止缓冲区溢出，确保字符串正确终止
     */
    len = min(count, sizeof(device_handle->kernel_buffer) - 1);

    /* 从用户空间复制数据到内核空间缓冲区
     * copy_from_user返回非0值表示复制失败
     * 这是内核与用户空间数据交互的标准方法，确保数据安全传输
     */
    if (copy_from_user(device_handle->kernel_buffer, user_write_buffer, len))
    {
        return -EFAULT; // 复制失败，返回内存访问错误
    }

    // 添加字符串终止符，确保字符串正确结束
    device_handle->kernel_buffer[len] = '\0';

    /* 根据写入的内容控制LED */
    if (strcmp(device_handle->kernel_buffer, "on") == 0)
    {
        led_on(); /* 调用LED开启函数 */
        /* 设置状态标志位，表示有数据可读 */
        device_handle->flag_of_poll = 1;
        /* 唤醒等待队列中的进程，以便poll可以检测到变化
         * 这是实现非阻塞I/O的关键机制
         */
        wake_up_interruptible(&queue_head_of_device);
        /* 发送信号给注册的进程，实现异步I/O通知
         * 当有进程通过fcntl设置了FASYNC标志时，会收到SIGIO信号
         */
        if (device_handle->fasync_queue_p)
            kill_fasync(&device_handle->fasync_queue_p, SIGIO, POLL_IN);
    }
    else if (strcmp(device_handle->kernel_buffer, "off") == 0)
    {
        led_off(); /* 调用LED关闭函数 */
        /* 设置状态标志位，表示有数据可读 */
        device_handle->flag_of_poll = 1;
        /* 唤醒等待队列中的进程，以便poll可以检测到变化 */
        wake_up_interruptible(&queue_head_of_device);
        /* 发送信号给注册的进程 */
        if (device_handle->fasync_queue_p)
            kill_fasync(&device_handle->fasync_queue_p, SIGIO, POLL_IN);
    }
    else if (strcmp(device_handle->kernel_buffer, "toggle") == 0)
    {
        led_toggle(); /* 调用LED切换函数 */
        /* 设置状态标志位，表示有数据可读 */
        device_handle->flag_of_poll = 1;
        /* 唤醒等待队列中的进程，以便poll可以检测到变化 */
        wake_up_interruptible(&queue_head_of_device);
        /* 发送信号给注册的进程 */
        if (device_handle->fasync_queue_p)
            kill_fasync(&device_handle->fasync_queue_p, SIGIO, POLL_IN);
    }
    else
    {
        /* 无效命令，打印警告信息 */
        printk(KERN_WARNING "Invalid command. Use 'on', 'off', or 'toggle'\n");
    }

    return len; // 返回实际写入的字节数
}

/**
 * @brief LED设备poll函数
 *
 * 当应用程序调用poll、select或epoll系统调用时，内核会调用此函数
 * 该函数用于检查设备是否可读或可写，是实现非阻塞I/O的关键
 *
 * @param filp 文件指针，包含设备信息
 * @param wait 指向poll_table_struct的指针，用于注册等待队列
 * @return __poll_t 返回设备状态掩码，指示设备是否可读/可写
 */
static __poll_t led_poll(struct file *filp, struct poll_table_struct *wait)
{
    /* 从文件指针的private_data字段中获取LED设备结构体指针 */
    char_device_handle *device_handle = filp->private_data;

    /* 初始化poll mask，用于返回设备状态 */
    __poll_t mask = 0;

    /* 将当前进程添加到等待队列中，当设备状态变化时会被唤醒
     * 这样poll/select系统调用就能知道何时设备可读/可写
     * poll_wait函数不会阻塞，只是注册等待队列
     */
    poll_wait(filp, &queue_head_of_device, wait);

    /* 检查设备是否有数据可读（flag_of_poll为1表示有数据） */
    if (device_handle->flag_of_poll)
    {
        /* 设置POLLIN和POLLRDNORM标志，表示设备可读
         * POLLIN表示有数据可读，POLLRDNORM表示普通数据可读
         */
        mask |= POLLIN | POLLRDNORM;
    }

    /* 设置POLLOUT和POLLWRNORM标志，表示设备可写
     * 对于LED设备，总是可写的，所以总是设置这些标志
     */
    mask |= POLLOUT | POLLWRNORM;

    return mask; /* 返回设备状态掩码 */
}

/**
 * @brief LED设备异步通知函数
 *
 * 当应用程序调用fcntl设置FASYNC标志时，内核会调用此函数
 * 该函数用于管理异步通知队列，当设备状态变化时可以向应用程序发送信号
 *
 * @param fd 文件描述符
 * @param filp 文件指针，包含设备信息
 * @param mode 模式标志，指示如何处理异步通知
 * @return int 成功返回0，失败返回负的错误码
 */
static int led_fasync(int fd, struct file *filp, int mode)
{
    /* 从文件指针的private_data字段中获取LED设备结构体指针 */
    char_device_handle *device_handle = filp->private_data;

    /* 调用fasync_helper处理异步通知
     * 这是内核提供的标准函数，用于管理异步通知队列
     * 当mode为FASYNC时，将文件添加到异步通知队列
     * 当mode为0时，从异步通知队列中移除文件
     */
    return fasync_helper(fd, filp, mode, &device_handle->fasync_queue_p);
}

/**
 * @brief LED字符设备初始化函数
 *
 * 当模块加载时，内核会调用此函数
 * 该函数负责初始化LED字符设备，包括分配设备号、创建设备节点、映射IO地址等
 *
 * @return int 成功返回0，失败返回负的错误码
 */
static int __init char_device_led_init(void)
{
    int ret = 0; /* 错误返回值 */

    /* 1.动态申请字符设备号
     * alloc_chrdev_region函数动态分配一个主设备号和次设备号
     * 参数：设备号指针，起始次设备号，设备数量，设备名称
     */
    ret = alloc_chrdev_region(&led_handle.device_number, 0, 1, "led0");
    if (ret)
    {
        goto cdev_err; /* 分配失败，跳转到错误处理 */
    }

    /* 从设备号中提取主设备号和次设备号 */
    led_handle.device_major_number = MAJOR(led_handle.device_number);
    led_handle.device_minor_number = MINOR(led_handle.device_number);

    /* 2.初始化字符设备句柄 */
    led_handle.char_device_t.owner = THIS_MODULE; /* 设置模块所有者 */
    led_handle.char_device_t.dev = led_handle.device_number; /* 设置设备号 */

    /* 填充文件操作结构体 */
    led_fops.owner = THIS_MODULE;
    led_fops.read = led_read; /* 设置读函数指针 */
    led_fops.write = led_write; /* 设置写函数指针 */
    led_fops.open = led_open; /* 设置打开函数指针 */
    led_fops.release = led_release; /* 设置关闭函数指针 */
    led_fops.poll = led_poll; /* 设置poll函数指针 */
    led_fops.fasync = led_fasync; /* 设置异步通知函数指针 */

    /* 初始化字符设备结构体，关联文件操作结构体 */
    cdev_init(&led_handle.char_device_t, &led_fops);

    /* 初始化内核缓冲区，设置默认值为"off" */
    strcpy(led_handle.kernel_buffer, "off");

    /* 初始化打开计数, 表示最多两个进程同时访问
     * 初始值设为2，表示设备未被占用
     */
    atomic64_set(&led_handle.open_count, 2);

    /* 初始化poll状态标志位，0表示无数据可读 */
    led_handle.flag_of_poll = 0;

    /* 初始化异步通知队列，初始为空 */
    led_handle.fasync_queue_p = NULL;

    /* 3.添加字符设备到内核
     * cdev_add函数将字符设备添加到系统中，使其可用
     */
    ret = cdev_add(&led_handle.char_device_t, led_handle.device_number, 1);
    if (ret)
    {
        goto cdev_add_err; /* 添加失败，跳转到错误处理 */
    }

    /* 4.创建设备类, 该API在不同的Linux内核参数数量不一样
     * 在/sys/class下创建设备类目录/sys/class/led_class
     * 之后的设备节点挂载到这个目录下
     */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    led_handle.device_class_p = class_create("led_class"); /* 新版内核不需要MODULE参数 */
#else
    led_handle.device_class_p = class_create(THIS_MODULE, "led_class"); /* 旧版内核需要MODULE参数 */
#endif
    if (IS_ERR(led_handle.device_class_p))
    {
        ret = PTR_ERR(led_handle.device_class_p); /* 获取错误码 */
        goto cdev_create_class_err; /* 跳转到错误处理 */
    }

    /* 5.创建设备节点
     * device_create函数在/dev目录下创建设备节点
     * 这样用户空间程序可以通过设备文件访问设备
     */
    led_handle.device_node_p = device_create(led_handle.device_class_p, NULL, led_handle.device_number, NULL, "led0");
    if (IS_ERR(led_handle.device_node_p))
    {
        ret = PTR_ERR(led_handle.device_node_p); /* 获取错误码 */
        goto cdev_create_inode_err; /* 跳转到错误处理 */
    }

    /* 6.映射IO地址
     * ioremap函数将物理地址映射为虚拟地址，使内核可以访问硬件寄存器
     */
    /* LED0 映射PMU GRF基地址，用于GPIO复用功能控制 */
    pmu_grf_base_addr = ioremap(PMU_GRF_MUX_CONTROL_REG_BASE_ADDR, 0x1000);
    if (!pmu_grf_base_addr)
    {
        ret = -ENOMEM; /* 内存不足错误 */
        goto unmap_pmu_grf_base_addr_err; /* 跳转到错误处理 */
    }

    /* 映射GPIO_0基地址，用于GPIO数据读写 */
    gpio_0_base_addr = ioremap(GPIO_0_BASE_ADDR, 0x1000);
    if (!gpio_0_base_addr)
    {
        ret = -ENOMEM; /* 内存不足错误 */
        goto unmap_gpio_0_base_addr_err; /* 跳转到错误处理 */
    }

    /* 7.初始化LED0和其它设备
     * led_init函数初始化LED硬件，包括设置GPIO方向和初始状态
     */
    led_init();

    return 0; /* 初始化成功 */

    /* ========= 错误回滚处理 ========= */
    /* 按照初始化的逆序释放资源 */
unmap_gpio_0_base_addr_err:
    iounmap(pmu_grf_base_addr); /* 释放PMU GRF地址映射 */
unmap_pmu_grf_base_addr_err:
    device_destroy(led_handle.device_class_p, led_handle.device_number); /* 删除设备节点 */
cdev_create_inode_err:
    class_destroy(led_handle.device_class_p); /* 删除设备类 */
cdev_create_class_err:
    cdev_del(&led_handle.char_device_t); /* 删除字符设备 */
cdev_add_err:
    unregister_chrdev_region(led_handle.device_number, 1); /* 释放字符设备号 */
cdev_err:
    printk(KERN_ERR "led init failed: %d\n", ret); /* 打印错误信息 */
    return ret; /* 返回错误码 */
}

/**
 * @brief LED字符设备退出函数
 *
 * 当模块卸载时，内核会调用此函数
 * 该函数负责释放LED字符设备占用的所有资源
 */
static void __exit char_device_led_exit(void)
{
    /* 删除设备节点，从/dev目录中移除设备文件 */
    device_destroy(led_handle.device_class_p, led_handle.device_number);

    /* 删除字符设备，从系统中移除字符设备 */
    cdev_del(&led_handle.char_device_t);

    /* 删除设备类，从/sys/class目录中移除设备类 */
    class_destroy(led_handle.device_class_p);

    /* 删除设备号，释放分配的设备号 */
    unregister_chrdev_region(led_handle.device_number, 1);

    /* 取消IO地址映射，释放虚拟地址空间 */
    iounmap(gpio_0_base_addr); /* 释放GPIO_0地址映射 */
    iounmap(pmu_grf_base_addr); /* 释放PMU GRF地址映射 */

    /* 打印退出信息，表示模块已成功卸载 */
    printk("Goodbye led example!\n");
}

/* 注册模块初始化和退出函数 */
module_init(char_device_led_init); /* 加载字符设备模块时调用初始化函数 */
module_exit(char_device_led_exit); /* 卸载字符设备模块时调用退出函数 */

/* 模块许可证声明，使用GPL许可证 */
MODULE_LICENSE("GPL");
/* 模块作者信息 */
MODULE_AUTHOR("zmor");
/* 模块描述信息 */
MODULE_DESCRIPTION("A cdev module");