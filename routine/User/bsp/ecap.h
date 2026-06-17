#ifndef __ECAP_H__
#define __ECAP_H__

#include "DSP28x_Project.h"

// 捕获完成回调函数类型
typedef void (*ecap_callback_t)(void);

void ecap_init(void);
void ecap_irq_enable(ecap_callback_t callback);  // 使能中断并注册回调
Uint32 ecap_get_period(void);       // 获取周期值（单位：SYSCLK计数）
Uint32 ecap_get_high_time(void);    // 获取高电平时间（单位：SYSCLK计数）
float ecap_get_duty_cycle(void);    // 获取占空比（百分比）

#endif // __ECAP_H__