#include "led.h"
#include <linux/kernel.h>

/* led_init()是空的, 因为默认就是 GPIO 复用, 输出模式 */
void led_init(void)
{
    /* 设置LED方向为输出 - 使用位操作设置GPIO_B7为输出模式 */
    LED_DIRECTION_CONTROL_REG |= (1 << 31); // 设置bit15为1，使GPIO_B7为输出模式

    /* 设置GPIO_B7的写使能掩码 - 只需要设置一次 */
    LED_DATA_REG |= (1 << 31); // 高16位设置写使能，低16位不改变数据
}

/**
 * @brief 设置LED状态
 * @param state 1表示点亮，0表示熄灭
 */

void led_set(int state)
{
    if (state)
    {
        /* 点亮LED: 设置GPIO_B7为高电平 */
        LED_DATA_REG = (LED_BIT_MASK << 16) | LED_BIT_MASK;
    }
    else
    {
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
}

/**
 * @brief 熄灭LED
 */
void led_off(void)
{
    led_set(0);
}

/**
 * @brief 切换LED状态
 */
void led_toggle(void)
{
    /* 读取当前LED状态并取反 */
    unsigned int current_state = LED_DATA_REG & (1 << 15);
    led_set(!current_state);
}