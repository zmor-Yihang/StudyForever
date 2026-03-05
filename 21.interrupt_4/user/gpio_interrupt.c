#include "gpio_interrupt.h"

/* 实例化字符设备 */
char_device_handle gpio_interrupt_handle;

/* 初始化定时器结构体 */
struct timer_list timer_struct;

/* 保存中断号，用于注销 */
static int gpio_irq_number = 0;

/* 中断下文回调函数 */
static irqreturn_t thread_callback(int irq, void *dev_id )
{
    printk(KERN_INFO "This is thread_callback!!\n");
    msleep(10);
    return IRQ_HANDLED;
}

/* 中断回调函数 */
static irqreturn_t gpio_interrupt_callback(int irq, void *dev_id)
{
    /* 打印中断信息 */
    printk(KERN_INFO "GPIO interrupt triggered!\n");

    /* 唤醒中断下文线程处理函数 */
    return IRQ_WAKE_THREAD;
}

/* 定时器回调函数声明 */
static void timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "Timer callback called at jiffies = %lu\n", jiffies);

    // 重新设置定时器
    mod_timer(&timer_struct, jiffies + msecs_to_jiffies(1000)); // 1秒后再次触发
}

/* 文件操作结构体 */
struct file_operations file_operations_t;

static int gpio_interrupt_open(struct inode *inode, struct file *filp)
{
    /* 获取字符设备实例的句柄, 存放在 filp->private_data */
    struct cdev *cdev = inode->i_cdev;
    char_device_handle *handle = container_of(cdev, char_device_handle, char_device_t);

    filp->private_data = handle;

    return 0;
}

static int gpio_interrupt_release(struct inode *inode, struct file *filp)
{
    char_device_handle *dev_handle = filp->private_data;
    (void)dev_handle; // Mark as intentionally unused

    return 0;
}
static ssize_t gpio_interrupt_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    char_device_handle *dev_handle = filp->private_data;
    loff_t p;
    size_t bytes_to_write;
    int i;

    if (!dev_handle)
    {
        return -EINVAL;
    }

    p = *f_pos; // 将写入数据的偏移量赋值给 loff_t 类型变量 p
    bytes_to_write = count;

    if (p > BUFFER_SIZE)
    { // 如果当前偏移值比最大偏移量大则返回错误
        return 0;
    }
    if (bytes_to_write > BUFFER_SIZE - p)
    {
        bytes_to_write = BUFFER_SIZE - p; // 如果要写入的数据超出剩余空间，则写入到最后位置
    }
    if (copy_from_user(dev_handle->kernel_buffer + p, buf, bytes_to_write))
    { // 将用户空间 buf 中的数据复制到 kernel_buffer
        printk(KERN_ERR "copy_from_user error \n");
        return -EFAULT;
    }

    // 打印写入的数据
    for (i = 0; i < bytes_to_write && i < 20; i++)
    {
        printk(KERN_INFO "kernel_buffer[%d] is %c\n", (int)(p + i), dev_handle->kernel_buffer[p + i]);
    }
    printk(KERN_INFO "Wrote %zu bytes at position %llu\n", bytes_to_write, p);

    *f_pos = *f_pos + bytes_to_write; // 更新偏移值
    return bytes_to_write;
}

static ssize_t gpio_interrupt_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    char_device_handle *dev_handle = filp->private_data;
    loff_t p;
    int i;
    size_t bytes_to_read;

    if (!dev_handle)
    {
        return -EINVAL;
    }

    p = *f_pos; // 将读取数据的偏移量赋值给 loff_t 类型变量 p
    bytes_to_read = count;

    if (p > BUFFER_SIZE)
    { // 如果当前偏移值比最大偏移量大则返回错误
        return 0;
    }
    if (bytes_to_read > BUFFER_SIZE - p)
    {
        bytes_to_read = BUFFER_SIZE - p; // 如果要读取的偏移值超出剩余的空间，则读取到最后位置
    }
    if (copy_to_user(buf, dev_handle->kernel_buffer + p, bytes_to_read))
    { // 将 kernel_buffer 中的值写入 buf，并传递到用户空间
        printk(KERN_ERR "copy_to_user error \n");
        return -EFAULT;
    }
    // 只打印当前读取位置附近的字符，避免访问超出缓冲区范围的内存
    for (i = 0; i < bytes_to_read && i < 20; i++)
    {
        printk(KERN_INFO "kernel_buffer[%d] is %c\n", (int)(p + i), dev_handle->kernel_buffer[p + i]);
    }
    printk(KERN_INFO "Read %zu bytes at position %llu\n", bytes_to_read, p);
    *f_pos = *f_pos + bytes_to_read; // 更新偏移值
    return bytes_to_read;
}

static loff_t gpio_interrupt_llseek(struct file *filp, loff_t offset, int whence)
{
    char_device_handle *dev_handle = filp->private_data;
    (void)dev_handle; // Mark as intentionally unused

    loff_t new_pos = 0;

    switch (whence)
    {
    case SEEK_SET:
        if (offset < 0 || offset > BUFFER_SIZE)
        {
            return -EINVAL;
        }
        new_pos = offset;
        break;
    case SEEK_CUR:
        if ((filp->f_pos + offset) < 0 || (filp->f_pos + offset) > BUFFER_SIZE)
        {
            return -EINVAL;
        }
        new_pos = filp->f_pos + offset;
        break;
    case SEEK_END:
        if ((BUFFER_SIZE + offset) < 0 || (BUFFER_SIZE + offset) > BUFFER_SIZE)
        {
            return -EINVAL;
        }
        new_pos = BUFFER_SIZE + offset;
        break;
    default:
        return -EINVAL;
    }

    filp->f_pos = new_pos;
    return new_pos;
}

static long gpio_interrupt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    char_device_handle *dev_handle = filp->private_data;
    (void)dev_handle; // Mark as intentionally unused

    switch (cmd)
    {
    case TIMER_START:
    {
        timer_setup(&timer_struct, timer_callback, 0);
        mod_timer(&timer_struct, jiffies + msecs_to_jiffies(5000)); // 1秒后触发
        break;
    }
    case TIMER_STOP:
    {
        del_timer(&timer_struct);
        break;
    }
    case TIMER_SET_PERIOD:
    {
        int time_tmp = 0;
        if (copy_from_user(&time_tmp, (void __user *)arg, sizeof(time_tmp)))
        {
            return -EFAULT;
        }

        // 验证周期值的有效性
        if (time_tmp <= 0)
        {
            printk(KERN_WARNING "Invalid timer period: %d (must be positive)\n", time_tmp);
            return -EINVAL;
        }

        if (time_tmp > 60000)
        {
            printk(KERN_WARNING "Timer period too large: %d (max 60000 ms)\n", time_tmp);
            return -EINVAL;
        }

        mod_timer(&timer_struct, jiffies + msecs_to_jiffies(time_tmp));
        printk(KERN_INFO "Timer period set to %d ms\n", time_tmp);

        break;
    }
    default:
        printk(KERN_WARNING "Invalid ioctl command: 0x%x\n", cmd);
        return -ENOTTY; // 返回错误码，表示不适当的ioctl命令
    }

    return 0;
}

static int __init gpio_interrupt_init(void)
{
    printk(KERN_INFO "llseek example init\n");

    /* 设置设备名称 */
    strncpy(gpio_interrupt_handle.name, "gpio_interrupt", sizeof(gpio_interrupt_handle.name) - 1);
    gpio_interrupt_handle.name[sizeof(gpio_interrupt_handle.name) - 1] = '\0';

    /* 分配设备号 */
    alloc_chrdev_region(&gpio_interrupt_handle.device_number, 0, 1, gpio_interrupt_handle.name);

    /* 获取设备号 */
    gpio_interrupt_handle.device_major_number = MAJOR(gpio_interrupt_handle.device_number);
    gpio_interrupt_handle.device_minor_number = MINOR(gpio_interrupt_handle.device_number);

    /* 初始化内核缓冲区 */
    memset(gpio_interrupt_handle.kernel_buffer, 0, sizeof(gpio_interrupt_handle.kernel_buffer));

    /* 字符设备结构体赋值 */
    gpio_interrupt_handle.char_device_t.owner = THIS_MODULE;
    gpio_interrupt_handle.char_device_t.dev = gpio_interrupt_handle.device_number;

    /* 文件操作结构体赋值 */
    file_operations_t.owner = THIS_MODULE;
    file_operations_t.read = gpio_interrupt_read;
    file_operations_t.write = gpio_interrupt_write;
    file_operations_t.open = gpio_interrupt_open;
    file_operations_t.release = gpio_interrupt_release;
    file_operations_t.llseek = gpio_interrupt_llseek;
    file_operations_t.unlocked_ioctl = gpio_interrupt_ioctl;

    /* 关联字符设备实例和文件操作结构体 */
    cdev_init(&gpio_interrupt_handle.char_device_t, &file_operations_t);

    /* 把设备添加到内核 */
    cdev_add(&gpio_interrupt_handle.char_device_t, gpio_interrupt_handle.device_number, 1);

    /* 创建设备类 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)
    gpio_interrupt_handle.device_class_p = class_create("CHAR_DEV");
#else
    gpio_interrupt_handle.device_class_p = class_create(THIS_MODULE, "CHAR_DEV");
#endif

    /* 创建设备节点 */
    gpio_interrupt_handle.device_node_p = device_create(gpio_interrupt_handle.device_class_p, NULL, gpio_interrupt_handle.device_number, NULL, gpio_interrupt_handle.name);

    // 初始化定时器
    timer_setup(&timer_struct, timer_callback, 0);

    /* 申请中断号 */
    gpio_irq_number = gpio_to_irq(13);
    if (gpio_irq_number < 0)
    {
        printk(KERN_ERR "Failed to get IRQ for GPIO 13\n");
        return gpio_irq_number;
    }

    /* 注册中断 */
    int ret = request_threaded_irq(gpio_irq_number, gpio_interrupt_callback, thread_callback, IRQF_TRIGGER_FALLING, "gpio_interrupt", NULL);
    if (ret)
    {
        printk(KERN_ERR "Failed to request IRQ %d\n", gpio_irq_number);
        return ret;
    }

    printk(KERN_INFO "Successfully registered GPIO interrupt %d\n", gpio_irq_number);

    return 0;
}

static void __exit gpio_interrupt_exit(void)
{
    printk(KERN_INFO "llseek example exit\n");

    /* 删除定时器 */
    del_timer_sync(&timer_struct);

    /* 注销中断 */
    free_irq(gpio_irq_number, NULL);

    /* 注销设备 */
    cdev_del(&gpio_interrupt_handle.char_device_t);

    /* 销毁设备节点 */
    device_destroy(gpio_interrupt_handle.device_class_p, gpio_interrupt_handle.device_number);

    /* 销毁设备类 */
    class_destroy(gpio_interrupt_handle.device_class_p);

    /* 注销设备号 */
    unregister_chrdev_region(gpio_interrupt_handle.device_number, 1);
}

module_init(gpio_interrupt_init);
module_exit(gpio_interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A llseek module");