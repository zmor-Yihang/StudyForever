#include "led_platform_device.h"

/* 设备释放函数声明 */
void led_platform_device_release(struct device *dev);

/* 定义平台设备使用的资源数组
 * 包含三个内存资源：
 * 1. LED复用控制寄存器资源
 * 2. LED方向控制寄存器资源
 * 3. LED数据寄存器资源
 */
struct resource led_resources[3] = {
    {
        .start = LED_MUX_CONTROL_REG,                           // LED复用控制寄存器起始地址
        .end = LED_MUX_CONTROL_REG + REG_SIZE - 1,              // LED复用控制寄存器结束地址
        .flags = IORESOURCE_MEM,                                // 资源类型为内存
        .name = "led_mux_control_reg",                          // 资源名称
    },
    {
        .start = LED_DIRECTION_CONTROL_REG,                     // LED方向控制寄存器起始地址
        .end = LED_DIRECTION_CONTROL_REG + REG_SIZE - 1,        // LED方向控制寄存器结束地址
        .flags = IORESOURCE_MEM,                                // 资源类型为内存
        .name = "led_direction_control_reg",                    // 资源名称
    },
    {
        .start = LED_DATA_REG,                                  // LED数据寄存器起始地址
        .end = LED_DATA_REG + REG_SIZE - 1,                     // LED数据寄存器结束地址
        .flags = IORESOURCE_MEM,                                // 资源类型为内存
        .name = "led_data_reg",                                 // 资源名称
    }
};

/* 定义平台设备结构体
 * 平台设备是Linux设备模型中的重要概念，用于描述硬件平台相关的设备信息
 */
struct platform_device led_platform_device_t = {
    .name = "led_device",                       // 设备名称，用于匹配驱动
    .id = -1,                                   // 设备ID，-1表示只有一个此类设备
    .resource = led_resources,                  // 设备使用的资源数组
    .num_resources = 3,                         // 资源数量
    .dev.release = led_platform_device_release, // 设备释放回调函数
};

/* 设备释放函数实现
 * 注意：这个函数不能定义成static，否则会报空指针异常
 * 当设备被注销时，内核会调用此函数来释放相关资源
 */
void led_platform_device_release(struct device *dev)
{
    printk("This is led_platform_device_release!\n");
}

static int __init platform_device_init(void)
{
    /* 注册平台设备到内核中 */
    platform_device_register(&led_platform_device_t);
    printk("LED platform device registered\n");

    return 0;
}

static void __exit platform_device_exit(void)
{
    /* 从内核中注销平台设备 */
    platform_device_unregister(&led_platform_device_t);
    printk("LED platform device unregistered\n");
}

module_init(platform_device_init);
module_exit(platform_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A platform led device module");