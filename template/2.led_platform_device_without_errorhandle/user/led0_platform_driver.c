#include "led0_platform_driver.h"

/* 平台驱动相关函数声明 */
static int led0_platform_driver_probe(struct platform_device *pdev);
static int led0_platform_driver_remove(struct platform_device *pdev);

/* 设备操作函数声明 */
static int led0_platform_driver_open(struct inode *inode, struct file *filp);
static int led0_platform_driver_release(struct inode *inode, struct file *filp);
static ssize_t led0_platform_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static ssize_t led0_platform_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static loff_t led0_platform_driver_llseek(struct file *filp, loff_t offset, int whence);
static long led0_platform_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

/* 实例化字符设备 */
char_device_handle led0_h;

/* 文件操作结构体 */
struct file_operations led0_file_operations_t = {
    .owner = THIS_MODULE,
    .read = led0_platform_driver_read,
    .write = led0_platform_driver_write,
    .open = led0_platform_driver_open,
    .release = led0_platform_driver_release,
    .llseek = led0_platform_driver_llseek,
    .unlocked_ioctl = led0_platform_driver_ioctl,
};

/* 平台驱动结构体 */
struct platform_driver led0_driver_t = {
    .probe = led0_platform_driver_probe,
    .remove = led0_platform_driver_remove,
    .driver.name = "led0",
    .driver.owner = THIS_MODULE,
};

static int led0_platform_driver_probe(struct platform_device *pdev)
{
    printk("This is led0_probe！\n");

    struct resource *res;

    /* 获取复用寄存器地址 */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    /* 映射物理地址到虚拟地址 */
    led0_h.vir_mux_control = ioremap(res->start, resource_size(res));

    /* 获取方向控制寄存器地址 */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 1);

    /* 映射物理地址到虚拟地址 */
    led0_h.vir_direction_control = ioremap(res->start, resource_size(res));

    /* 获取数据寄存器地址 */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 2);

    /* 映射物理地址到虚拟地址 */
    led0_h.vir_data = ioremap(res->start, resource_size(res));

    printk("Resources mapped successfully (bypassing resource checks)!\n");

    /* 设置设备名称 */
    strncpy(led0_h.name, "led0", sizeof(led0_h.name) - 1);
    led0_h.name[sizeof(led0_h.name) - 1] = '\0';

    /* 分配设备号 */
    alloc_chrdev_region(&led0_h.device_number, 0, 1, led0_h.name);

    /* 获取设备号 */
    led0_h.device_major_number = MAJOR(led0_h.device_number);
    led0_h.device_minor_number = MINOR(led0_h.device_number);

    /* 初始化内核缓冲区 */
    memset(led0_h.kernel_buffer, 0, sizeof(led0_h.kernel_buffer));

    /* 字符设备结构体赋值 */
    led0_h.char_device_t.owner = THIS_MODULE;
    led0_h.char_device_t.dev = led0_h.device_number;

    /* 关联字符设备实例和文件操作结构体 */
    cdev_init(&led0_h.char_device_t, &led0_file_operations_t);

    /* 把设备添加到内核 */
    cdev_add(&led0_h.char_device_t, led0_h.device_number, 1);

    /* 创建设备类 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)
    led0_h.device_class_p = class_create("led0");
#else
    led0_h.device_class_p = class_create(THIS_MODULE, "led");
#endif

    /* 创建设备节点 */
    led0_h.device_node_p = device_create(led0_h.device_class_p, NULL, led0_h.device_number, NULL, led0_h.name);

    return 0;
}

static int led0_platform_driver_remove(struct platform_device *pdev)
{
    printk("This is led0_remove！\n");

    /* 注销设备 */
    cdev_del(&led0_h.char_device_t);

    /* 销毁设备节点 */
    device_destroy(led0_h.device_class_p, led0_h.device_number);

    /* 销毁设备类 */
    class_destroy(led0_h.device_class_p);

    /* 注销设备号 */
    unregister_chrdev_region(led0_h.device_number, 1);

    /* 释放手动映射的资源 */
    if (led0_h.vir_mux_control)
    {
        iounmap(led0_h.vir_mux_control);
        led0_h.vir_mux_control = NULL;
    }
    if (led0_h.vir_direction_control)
    {
        iounmap(led0_h.vir_direction_control);
        led0_h.vir_direction_control = NULL;
    }
    if (led0_h.vir_data)
    {
        iounmap(led0_h.vir_data);
        led0_h.vir_data = NULL;
    }

    return 0;
}

static int led0_platform_driver_open(struct inode *inode, struct file *filp)
{
    /* 获取字符设备实例的句柄, 存放在 filp->private_data */
    /* 从inode结构中获取字符设备结构体指针 */
    struct cdev *cdev = inode->i_cdev;
    /* 使用container_of宏通过字符设备结构体指针获取包含它的char_device_handle结构体指针 */
    /* container_of(ptr, type, member)通过结构体成员指针获取结构体首地址 */
    char_device_handle *handle = container_of(cdev, char_device_handle, char_device_t);

    filp->private_data = handle;

    return 0;
}

static int led0_platform_driver_release(struct inode *inode, struct file *filp)
{
    char_device_handle *handle = filp->private_data;
    (void)handle;

    return 0;
}

/* f_pos是应用层传入的参数, 每次用户空间调用 open() 一个文件，内核都会创建一个新的 struct file 实例。每个 struct file 都有自己的 f_pos */
static ssize_t led0_platform_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    /* 从文件结构的私有数据中获取设备句柄 */
    char_device_handle *dev_handle = filp->private_data;
    loff_t p;              /* 当前写入位置的偏移量 */
    size_t bytes_to_write; /* 实际要写入的字节数 */

    /* 检查设备句柄是否有效 */
    if (!dev_handle)
    {
        return -EINVAL; /* 返回无效参数错误 */
    }

    /* 获取当前文件位置偏移量 */
    p = *f_pos;
    /* 初始设定要写入的字节数为请求的字节数 */
    bytes_to_write = count;

    /* 边界检查：如果当前位置已经超过缓冲区大小，则不写入任何数据 */
    if (p > BUFFER_SIZE)
    {
        return 0; /* 返回0表示没有写入任何数据 */
    }

    /* 边界检查：如果要写入的数据超出缓冲区剩余空间，则只写入到缓冲区末尾 */
    if (bytes_to_write > BUFFER_SIZE - p)
    {
        bytes_to_write = BUFFER_SIZE - p;
    }

    /* 从用户空间复制数据到内核空间的缓冲区 */
    /* copy_from_user(to, from, n) 是Linux内核提供的安全函数，用于从用户空间复制数据到内核空间 */
    if (copy_from_user(dev_handle->kernel_buffer + p, buf, bytes_to_write))
    {
        printk(KERN_ERR "copy_from_user error \n"); /* 记录错误日志 */
        return -EFAULT;                             /* 返回错误代码，表示数据复制失败 */
    }

    /* 记录写入操作的信息 */
    // printk(KERN_INFO "Wrote %zu bytes at position %llu\n", bytes_to_write, p);

    /* 更新文件位置偏移量 */
    *f_pos = *f_pos + bytes_to_write;

    /* 返回实际写入的字节数 */
    return bytes_to_write;
}

static ssize_t led0_platform_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    char_device_handle *dev_handle = filp->private_data;
    loff_t p;
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

    /* 打印读取的信息 */
    printk(KERN_INFO "Read %zu bytes at position %llu\n", bytes_to_read, p);

    *f_pos = *f_pos + bytes_to_read; /* 更新偏移值, 存储到文件描述符的 f_pos 中 */

    return bytes_to_read;
}

static loff_t led0_platform_driver_llseek(struct file *filp, loff_t offset, int whence)
{
    /* 从文件结构的私有数据中获取设备句柄 */
    char_device_handle *dev_handle = filp->private_data;

    (void)dev_handle;

    /* 新的位置偏移量 */
    loff_t new_pos = 0;

    /* 根据whence参数确定文件位置设置方式 */
    switch (whence)
    {
    /* SEEK_SET: 从文件开始处计算偏移 */
    case SEEK_SET:
        /* 检查偏移量是否在有效范围内 */
        if (offset < 0 || offset > BUFFER_SIZE)
        {
            /* 偏移量无效，返回错误码 */
            return -EINVAL;
        }
        /* 设置新位置为指定的偏移量 */
        new_pos = offset;
        break;

    /* SEEK_CUR: 从当前位置计算偏移 */
    case SEEK_CUR:
        /* 检查新的位置是否在有效范围内 */
        if ((filp->f_pos + offset) < 0 || (filp->f_pos + offset) > BUFFER_SIZE)
        {
            /* 新位置无效，返回错误码 */
            return -EINVAL;
        }
        /* 设置新位置为当前位置加上偏移量 */
        new_pos = filp->f_pos + offset;
        break;

    /* SEEK_END: 从文件结尾计算偏移 */
    case SEEK_END:
        /* 检查新的位置是否在有效范围内 */
        if ((BUFFER_SIZE + offset) < 0 || (BUFFER_SIZE + offset) > BUFFER_SIZE)
        {
            /* 新位置无效，返回错误码 */
            return -EINVAL;
        }
        /* 设置新位置为文件末尾加上偏移量 */
        new_pos = BUFFER_SIZE + offset;
        break;

    /* 其他无效的whence值 */
    default:
        /* 返回错误码 */
        return -EINVAL;
    }

    /* 更新文件的位置偏移量 */
    filp->f_pos = new_pos;
    /* 返回新的位置 */
    return new_pos;
}

static long led0_platform_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    char_device_handle *dev_handle = filp->private_data;

    (void)dev_handle;

    unsigned int data_reg_val;

    switch (cmd)
    {
    case LED0_ON:
    {
        /* 点亮LED: 设置GPIO_B7为高电平 */
        /* 数据寄存器格式: 高16位为写使能掩码，低16位为数据值 */
        data_reg_val = (LED_BIT_MASK << 16) | LED_BIT_MASK;
        iowrite32(data_reg_val, led0_h.vir_data);
        break;
    }
    case LED0_OFF:
    {
        /* 熄灭LED: 设置GPIO_B7为低电平 */
        data_reg_val = (LED_BIT_MASK << 16) | 0x0000;
        iowrite32(data_reg_val, led0_h.vir_data);
        break;
    }

    case LED0_TOGGLE:
    {
        /* 读取当前LED状态 */
        data_reg_val = ioread32(led0_h.vir_data);
        unsigned int current_state = data_reg_val & LED_BIT_MASK;

        if (!current_state)
        {
            /* 点亮LED: 设置GPIO_B7为高电平 */
            data_reg_val = (LED_BIT_MASK << 16) | LED_BIT_MASK;
            iowrite32(data_reg_val, led0_h.vir_data);
        }
        else
        {
            /* 熄灭LED: 设置GPIO_B7为低电平 */
            data_reg_val = (LED_BIT_MASK << 16) | 0x0000;
            iowrite32(data_reg_val, led0_h.vir_data);
        }
        break;
    }
    case LED0_SET:
    {
        /* 从用户空间获取要设置的LED状态 */
        unsigned int set_state;
        if (arg) {
            if (copy_from_user(&set_state, (unsigned int *)arg, sizeof(unsigned int))) {
                return -EFAULT;
            }
            
            /* 根据用户提供的状态设置LED */
            if (set_state) {
                /* 点亮LED: 设置GPIO_B7为高电平 */
                data_reg_val = (LED_BIT_MASK << 16) | LED_BIT_MASK;
                iowrite32(data_reg_val, led0_h.vir_data);
            } else {
                /* 熄灭LED: 设置GPIO_B7为低电平 */
                data_reg_val = (LED_BIT_MASK << 16) | 0x0000;
                iowrite32(data_reg_val, led0_h.vir_data);
            }
        }
        break;
    }
    case LED0_GET:
    {
        /* 读取当前LED状态 */
        data_reg_val = ioread32(led0_h.vir_data);
        unsigned int current_state = data_reg_val & LED_BIT_MASK;
        
        /* 将状态返回给用户空间 */
        if (arg) {
            unsigned int state = current_state ? 1 : 0;
            if (copy_to_user((unsigned int *)arg, &state, sizeof(unsigned int))) {
                return -EFAULT;
            }
        }
        break;
    }

    default:
        printk(KERN_WARNING "Invalid ioctl command: 0x%x\n", cmd);
        return -ENOTTY; // 返回错误码，表示不适当的ioctl命令
    }

    return 0;
}

static int __init led0_platform_driver_init(void)
{
    printk(KERN_INFO "led0_platform_driver example init\n");

    /* 注册设备驱动 */
    platform_driver_register(&led0_driver_t);

    return 0;
}

static void __exit led0_platform_driver_exit(void)
{
    printk(KERN_INFO "led0_platform_driver example exit\n");

    /* 注销平台设备 */
    platform_driver_unregister(&led0_driver_t);
}

module_init(led0_platform_driver_init);
module_exit(led0_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A platform led0 driver module");
