#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

dev_t dev_num;

struct cdev char_dev_handle = {
    .owner = THIS_MODULE,
};

struct file_operations char_dev_fops = {
    .owner = THIS_MODULE,
};

static int __init char_dev_register_init(void)
{
    printk("char_dev_register example!\n");

    /* 动态注册字符设备号 */
    alloc_chrdev_region(&dev_num, 0, 1, "char_dev_register_device");
    printk("dev_num is %d\n", dev_num);

    /* 初始化cdev结构体 */
    cdev_init(&char_dev_handle, &char_dev_fops);

    /* 添加字符设备 */
    cdev_add(&char_dev_handle, dev_num, 1);

    return 0;
}

static void __exit char_dev_register_exit(void)
{
    /* 删除字符设备 */
    cdev_del(&char_dev_handle);

    /* 删除设备号 */
    unregister_chrdev_region(dev_num, 1);

    printk("Goodbye char_dev_register example!\n");
}

module_init(char_dev_register_init);
module_exit(char_dev_register_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A char_dev_register module");
