#ifndef __EPWM_H__
#define __EPWM_H__

#include "DSP28x_Project.h"

//
//  EPWM1：普通 PWM（单路输出）
//
#define EPWM1_FREQ_HZ   10000UL         // PWM 频率 10KHz
#define EPWM1_TBCLK_HZ  150000000UL     // 时基单元频率 150MHz

// 中心对齐计数模式，计数器周期为EPWM1_PERIOD
#define EPWM1_PERIOD    ((Uint16)(EPWM1_TBCLK_HZ / (2UL * EPWM1_FREQ_HZ)))

//
// EPWM2：电机控制用，互补带死区 + TZ硬件保护 + 触发ADC
//
#define EPWM2_FREQ_HZ   10000UL         // PWM 频率 10KHz
#define EPWM2_TBCLK_HZ  150000000UL     // 时基单元频率 150MHz

// 中心对齐，半周期计数：TBPRD = TBCLK / (2 * 频率)
#define EPWM2_PERIOD    ((Uint16)(EPWM2_TBCLK_HZ / (2UL * EPWM2_FREQ_HZ)))

//
// 死区时间。DBRED/DBFED 的单位是 TBCLK 周期数。
// 1 个 TBCLK = 1/150MHz ≈ 6.67ns，所以 1µs = 150 个计数。
// 改死区只动 EPWM2_DB_NS 即可，配小会导致上下桥臂直通短路。
//
#define EPWM2_DB_NS     1000UL          // 死区 1µs
#define EPWM2_DB_COUNT  ((Uint16)((EPWM2_TBCLK_HZ / 1000000UL) * EPWM2_DB_NS / 1000UL))

// TZ 跳闸回调（故障发生时由中断调用，用于通知上层状态机停机/报警）
typedef void (*epwm_tz_cb_t)(void);

//
// 全局时基时钟控制
// TBCLKSYNC 是所有 ePWM 模块共享的时基开关，不能由单个模块的 init 来操作，
// 否则后初始化的模块会打断已在运行的模块。
// 典型调用顺序：epwm_tbclk_stop() → 各 epwmX_init() → epwm_tbclk_start()
//
void epwm_tbclk_stop(void);
void epwm_tbclk_start(void);

//
// EPWM1 API
//
void epwm1_init(void);
void epwm1_set_duty(float32 duty);
void epwm1_start(void);
void epwm1_stop(void);
void epwm1_set_freq(Uint32 freq_hz);    // 运行时修改频率（影子加载，下一个周期生效）

//
// EPWM2 API
//
void epwm2_init(void);
void epwm2_set_duty(float32 duty);
void epwm2_set_freq(Uint32 freq_hz);    // 运行时修改频率（影子加载，下一个周期生效）
void epwm2_reg_tz_cb(epwm_tz_cb_t cb);
void epwm2_tz_irq_enable(void);
void epwm2_clear_fault(void);
void epwm2_force_trip(void);     // 软件强制触发 TZ one-shot 故障

#endif
