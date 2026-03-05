#ifndef __IO_MAPPING_H__
#define __IO_MAPPING_H__

#include <linux/io.h>

/* 虚拟地址映射指针声明 */
extern void __iomem *pmu_grf_base_addr;   /* PMU GRF基地址虚拟映射 */
extern void __iomem *gpio_0_base_addr;    /* GPIO_0基地址虚拟映射 */

#endif /* __IO_MAPPING_H__ */