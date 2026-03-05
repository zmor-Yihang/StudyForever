#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

int add(int a, int b)
{
    return a + b;
}
EXPORT_SYMBOL(add);

static int a = 11;

static int array[5] = {2};
static int array_size;      /* 数组大小, 用于接收 module_param_array 传递的数组大小 */

static char string[11] = "hello zmor";

module_param(a, int, S_IRUGO);
MODULE_PARM_DESC(a, "驱动传参: 整数");

module_param_array(array, int, &array_size, S_IRUGO);
MODULE_PARM_DESC(array, "驱动传参: 整数数组");

module_param_string(string, string, sizeof(string), S_IRUGO);
MODULE_PARM_DESC(string, "驱动传参: 字符串");

static int __init param_init(void)
{
    printk("print a = %d\n", a);
    for (int i = 0; i < array_size; i++)
    {
        printk("array[%d] = %d\n", i, array[i]);
    }
    printk("string = %s\n", string);
    return 0;
}

static void __exit param_exit(void)
{
    printk(KERN_INFO "Goodbye param module!\n");
}

module_init(param_init);
module_exit(param_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A simple param module");