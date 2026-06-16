#ifndef __ADC_H
#define __ADC_H

#include "DSP28x_Project.h"

// 转换完成回调：ISR 内调用，把最近一次采样原始值(0~4095)交给上层(FOC 电流环)
typedef void (*adc_conv_cb_t)(Uint16 result);

void adc_init(void);                 // 配触发源=ePWM SOCA、使能 SEQ1 中断，不软件触发
void adc_reg_cb(adc_conv_cb_t cb);   // 注册转换完成回调，须在 adc_irq_enable() 之前调用
void adc_irq_enable(void);           // 接入 PIE 组1.6(ADCINT) → CPU INT1
Uint16 adc_get_last(void);           // 读最近一次采样结果(主循环观察用)

// 以下为只读状态查询，仅用于调试/观察采样链健康，正常运行不依赖
Uint16 adc_seq_busy(void);           // SEQ1 是否正在转换中(1=忙)
Uint16 adc_seq_remaining(void);      // 级联排序器剩余未转换通道数(SEQ_CNTR)
Uint16 adc_int_pending(void);        // SEQ1 转换完成中断标志是否置位(1=有待处理)

#endif
