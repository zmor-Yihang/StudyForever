#ifndef __LED0_PLATFORM_DEVICE_H__
#define __LED0_PLATFORM_DEVICE_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define REG_SIZE 4 /* 寄存器大小为4字节 */

/* PMU GRF 复用功能控制寄存器基地址 */
#define PMU_GRF_MUX_CONTROL_REG_BASE_ADDR 0xFDC20000
/* GPIO_B_4567 复用功能控制寄存器的偏移地址 */
#define GPIO_B_H_MUX_CONTROL_REG_OFFSET 0x000C

/* GPIO_0 寄存器基地址 */
#define GPIO_0_BASE_ADDR 0xFDD60000
/* GPIO_x_A/B 方向控制寄存器偏移地址 */
#define GPIO0_AB_DIRECTION_CONTROL_REG_OFFSET 0x0008
/* GPIO_x_A/B 数据寄存器偏移地址 */
#define GPIO0_DATA_REG_OFFSET 0x0000

/* LED IO 复用寄存器 */
#define LED_MUX_CONTROL_REG (PMU_GRF_MUX_CONTROL_REG_BASE_ADDR + GPIO_B_H_MUX_CONTROL_REG_OFFSET)

/* LED 方向控制寄存器 */
#define LED_DIRECTION_CONTROL_REG (GPIO_0_BASE_ADDR + GPIO0_AB_DIRECTION_CONTROL_REG_OFFSET)

/* LED 数据寄存器 */
#define LED_DATA_REG (GPIO_0_BASE_ADDR + GPIO0_DATA_REG_OFFSET)

#endif /* __LED0_PLATFORM_DEVICE_H__ */
