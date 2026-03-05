#include "llseek_zmor.h"

/* 实例化字符设备 */
llseek_device_handle llseek_device_h;

/* 初始化定时器结构体 */
struct timer_list timer_struct;

/* 定时器回调函数声明 */
static void timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "Timer callback called at jiffies = %lu\n", jiffies);

    // 重新设置定时器
    mod_timer(&timer_struct, jiffies + msecs_to_jiffies(1000)); // 1秒后再次触发
}

/* 文件操作结构体 */
struct file_operations file_operations_t;

static int llseek_device_open(struct inode *inode, struct file *filp)
{
    /* 获取字符设备实例的句柄, 存放在 filp->private_data */
    struct cdev *cdev = inode->i_cdev;
    llseek_device_handle *handle = container_of(cdev, llseek_device_handle, char_device_t);

    filp->private_data = handle;

    return 0;
}

static int llseek_device_release(struct inode *inode, struct file *filp)
{
    llseek_device_handle *dev_handle = filp->private_data;
    (void)dev_handle; // Mark as intentionally unused

    return 0;
}
static ssize_t llseek_device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    llseek_device_handle *dev_handle = filp->private_data;
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

static ssize_t llseek_device_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    llseek_device_handle *dev_handle = filp->private_data;
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

static loff_t llseek_device_llseek(struct file *filp, loff_t offset, int whence)
{
    llseek_device_handle *dev_handle = filp->private_data;
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

static long llseek_device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    llseek_device_handle *dev_handle = filp->private_data;
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
        if (copy_from_user(&time_tmp, (void __user *)arg, sizeof(time_tmp))){
            return -EFAULT;
        }
        mod_timer(&timer_struct, jiffies + msecs_to_jiffies(time_tmp));

        break;
    }
    default:
        printk(KERN_WARNING "Invalid ioctl command: 0x%x\n", cmd);
        return -ENOTTY; // 返回错误码，表示不适当的ioctl命令
    }

    return 0;
}

static int __init llseek_device_init(void)
{
    printk(KERN_INFO "llseek example init\n");

    /* 设置设备名称 */
    strncpy(llseek_device_h.name, "llseek_device", sizeof(llseek_device_h.name) - 1);
    llseek_device_h.name[sizeof(llseek_device_h.name) - 1] = '\0';

    /* 分配设备号 */
    alloc_chrdev_region(&llseek_device_h.device_number, 0, 1, llseek_device_h.name);

    /* 获取设备号 */
    llseek_device_h.device_major_number = MAJOR(llseek_device_h.device_number);
    llseek_device_h.device_minor_number = MINOR(llseek_device_h.device_number);

    /* 初始化内核缓冲区 */
    memset(llseek_device_h.kernel_buffer, 0, sizeof(llseek_device_h.kernel_buffer));

    /* 字符设备结构体赋值 */
    llseek_device_h.char_device_t.owner = THIS_MODULE;
    llseek_device_h.char_device_t.dev = llseek_device_h.device_number;

    /* 文件操作结构体赋值 */
    file_operations_t.owner = THIS_MODULE;
    file_operations_t.read = llseek_device_read;
    file_operations_t.write = llseek_device_write;
    file_operations_t.open = llseek_device_open;
    file_operations_t.release = llseek_device_release;
    file_operations_t.llseek = llseek_device_llseek;
    file_operations_t.unlocked_ioctl = llseek_device_ioctl;

    /* 关联字符设备实例和文件操作结构体 */
    cdev_init(&llseek_device_h.char_device_t, &file_operations_t);

    /* 把设备添加到内核 */
    cdev_add(&llseek_device_h.char_device_t, llseek_device_h.device_number, 1);

    /* 创建设备类 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)
    llseek_device_h.device_class_p = class_create("CHAR_DEV");
#else
    llseek_device_h.device_class_p = class_create(THIS_MODULE, "CHAR_DEV");
#endif

    /* 创建设备节点 */
    llseek_device_h.device_node_p = device_create(llseek_device_h.device_class_p, NULL, llseek_device_h.device_number, NULL, llseek_device_h.name);

    // 初始化定时器
    timer_setup(&timer_struct, timer_callback, 0);
    return 0;
}

static void __exit llseek_device_exit(void)
{
    printk(KERN_INFO "llseek example exit\n");

    /* 注销设备 */
    cdev_del(&llseek_device_h.char_device_t);

    /* 销毁设备节点 */
    device_destroy(llseek_device_h.device_class_p, llseek_device_h.device_number);

    /* 销毁设备类 */
    class_destroy(llseek_device_h.device_class_p);

    /* 注销设备号 */
    unregister_chrdev_region(llseek_device_h.device_number, 1);
}

module_init(llseek_device_init);
module_exit(llseek_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A llseek module");