#ifndef __LED_REG_H__
#define __LED_REG_H__

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

/* 声明外部虚拟地址映射指针 */
extern void __iomem *pmu_grf_base_addr;
extern void __iomem *gpio_0_base_addr;

/* LED IO 复用寄存器 - 使用虚拟地址 */
#define LED_MUX_CONTROL_REG (*(volatile unsigned int __iomem *)(pmu_grf_base_addr + GPIO_B_H_MUX_CONTROL_REG_OFFSET))

/* LED 方向控制寄存器 - 使用虚拟地址 */
#define LED_DIRECTION_CONTROL_REG (*(volatile unsigned int __iomem *)(gpio_0_base_addr + GPIO0_AB_DIRECTION_CONTROL_REG_OFFSET))

/* LED 数据寄存器 - 使用虚拟地址 */
#define LED_DATA_REG (*(volatile unsigned int __iomem *)(gpio_0_base_addr + GPIO0_DATA_REG_OFFSET))

/* 
 * GPIO数据寄存器写入格式说明：
 * - 高16位：写使能掩码，1表示对应位可写入
 * - 低16位：数据值，1表示置高电平，0表示置低电平
 * - LED连接在GPIO_B7引脚上，对应位为bit7
 * - GPIO_B7在GPIO0_B数据寄存器中，对应位为bit7 (0x80)
 */
#define LED_BIT_MASK 0x0080  // GPIO_B7对应的掩码

/* LED控制函数声明 */
void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
void led_set(int state);

#endif /* __LED_REG_H__ */
