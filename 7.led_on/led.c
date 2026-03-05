#include "led.h"
#include <linux/kernel.h>

/* led_init()是空的, 因为默认就是 GPIO 复用, 输出模式 */
void led_init(void)
{
    
}

/**
 * @brief 设置LED状态
 * @param state 1表示点亮，0表示熄灭
 */
void led_set(int state)
{
    if (state) {
        /* 点亮LED: 设置GPIO_B7为高电平 */
        LED_DATA_REG = (LED_BIT_MASK << 16) | LED_BIT_MASK;
    } else {
        /* 熄灭LED: 设置GPIO_B7为低电平 */
        LED_DATA_REG = (LED_BIT_MASK << 16) | 0x0000;
    }
}

/**
 * @brief 点亮LED
 */
void led_on(void)
{
    led_set(1);
    printk(KERN_INFO "LED turned ON\n");
}

/**
 * @brief 熄灭LED
 */
void led_off(void)
{
    led_set(0);
    printk(KERN_INFO "LED turned OFF\n");
}

/**
 * @brief 切换LED状态
 */
void led_toggle(void)
{
    /* 读取当前LED状态并取反 */
    unsigned int current_state = LED_DATA_REG & LED_BIT_MASK;
    led_set(!current_state);
    printk(KERN_INFO "LED toggled\n");
}