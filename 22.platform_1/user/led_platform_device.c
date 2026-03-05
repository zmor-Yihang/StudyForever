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
        .flags = IORESOURCE_MEM_32BIT,                          // 资源类型为32位内存
        .name = "led_mux_control_reg",                          // 资源名称
    },
    {
        .start = LED_DIRECTION_CONTROL_REG,                     // LED方向控制寄存器起始地址
        .end = LED_DIRECTION_CONTROL_REG + REG_SIZE - 1,        // LED方向控制寄存器结束地址
        .flags = IORESOURCE_MEM_32BIT,                          // 资源类型为32位内存
        .name = "led_direction_control_reg",                    // 资源名称
    },
    {
        .start = LED_DATA_REG,                                  // LED数据寄存器起始地址
        .end = LED_DATA_REG + REG_SIZE - 1,                     // LED数据寄存器结束地址
        .flags = IORESOURCE_MEM_32BIT,                          // 资源类型为32位内存
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
 * 
 * 关于资源释放的说明：
 * 1. 当前led_resources数组是静态分配的，不需要在此函数中手动释放
 * 2. 如果将来修改为动态分配资源数组，则需要在此处添加释放代码：
 *    - 对于动态分配的资源数组，应使用kfree()释放
 *    - 示例代码：
 *      struct platform_device *pdev = to_platform_device(dev);
 *      if (pdev->resource) {
 *          kfree(pdev->resource);
 *          pdev->resource = NULL;
 *      }
 * 3. 此函数主要用于释放设备驱动中动态分配的其他资源
 *    如动态分配的内存、申请的IRQ等
 */
void led_platform_device_release(struct device *dev)
{
    printk("This is led_platform_device_release!\n");
}

static int __init platform_device_init(void)
{
    /* 注册平台设备到内核中 */
    platform_device_register(&led_platform_device_t);

    return 0;
}

static void __exit platform_device_exit(void)
{
    /* 从内核中注销平台设备 */
    platform_device_unregister(&led_platform_device_t);
}

module_init(platform_device_init);
module_exit(platform_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zmor");
MODULE_DESCRIPTION("A platform led device module");