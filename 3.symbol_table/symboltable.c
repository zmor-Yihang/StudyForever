#include <linux/module.h>
#include <linux/init.h>

extern int add(int a, int b);

static int __init symboltable_init(void)
{
    printk("hello world\n");
    printk("add(1, 2) = %d\n", add(1, 2));
    return 0;
}

static void __exit symboltable_exit(void)
{
    printk("Goodbye world!\n");
}

module_init(symboltable_init);
module_exit(symboltable_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A simple symboltable module");
