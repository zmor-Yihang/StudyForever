#include "kernel_timer.h"

/* 初始化定时器结构体 */
struct timer_list timer_struct;

/* 定时器回调函数声明 */
static void timer_callback(struct timer_list *t);

static int __init kernel_timer_init(void)
{
    printk(KERN_INFO "Timer example init\n");

    // 初始化定时器
    timer_setup(&timer_struct, timer_callback, 0);

    // 启动定时器，5秒后触发
    mod_timer(&timer_struct, jiffies + msecs_to_jiffies(5000));

    return 0;
}

static void __exit kernel_timer_exit(void)
{
    // 删除定时器
    del_timer(&timer_struct);
    printk(KERN_INFO "Timer example exit\n");
}

// 定时器回调函数
static void timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "Timer callback called at jiffies = %lu\n", jiffies);

    // 重新设置定时器
    mod_timer(&timer_struct, jiffies + msecs_to_jiffies(1000)); // 1秒后再次触发
}
 
module_init(kernel_timer_init);
module_exit(kernel_timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A timer module");
