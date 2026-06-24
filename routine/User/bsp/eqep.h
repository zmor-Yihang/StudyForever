#ifndef __EQEP_H__
#define __EQEP_H__

#include "DSP28x_Project.h"

//
// eQEP1 配置参数
//
#if (CPU_FRQ_150MHZ)
    #define EQEP1_UNIT_PERIOD   1500000UL   // 单位定时器周期：100Hz @ 150MHz SYSCLK
#endif
#if (CPU_FRQ_100MHZ)
    #define EQEP1_UNIT_PERIOD   1000000UL   // 单位定时器周期：100Hz @ 100MHz SYSCLK
#endif

//
// 电机和编码器参数（根据实际应用修改）
//
#define EQEP_ENCODER_LINES  1000UL          // 编码器线数（1000线/转）
#define EQEP_COUNTS_PER_REV 4000UL          // 每转计数（4倍频：4000 counts/转）
#define EQEP_POLE_PAIRS     2               // 电机极对数
#define EQEP_BASE_RPM       6000            // 基准转速 rpm

//
// 速度计算分频器（根据 SYSCLK 和最小转速自动计算）
//
#if (CPU_FRQ_150MHZ)
    // SpeedScaler = (UPPS/4000) * (SYSCLK/CCPS) * 60 / BaseRpm
    // = (32/4000) * (150MHz/128) * 60 / 6000 = 94
    #define EQEP_SPEED_SCALER   94
#endif
#if (CPU_FRQ_100MHZ)
    #define EQEP_SPEED_SCALER   63
#endif

//
// eQEP 位置和速度结构体
//
typedef struct {
    // 输出：位置信息
    int16 theta_elec;           // 电气角 (0~32767 对应 0~180°)
    int16 theta_mech;           // 机械角 (0~32767 对应 0~180°)
    int16 direction;            // 旋转方向 (0=反转CCW, 1=正转CW)
    
    // 输出：速度信息
    float32 speed_pu_fr;        // 单位时间法测速（标幺值 -1.0~1.0）
    float32 speed_pu_pr;        // 捕获单元法测速（标幺值 -1.0~1.0）
    int32 speed_rpm_fr;         // 单位时间法测速（rpm）
    int32 speed_rpm_pr;         // 捕获单元法测速（rpm）
    
    // 内部变量
    int16 theta_raw;            // 原始计数值
    Uint16 cal_angle;           // 角度校准偏移
    Uint16 index_sync_flag;     // Index 同步标志
    float32 oldpos;             // 上一次位置（用于差分计算）
    
    // 配置参数
    Uint16 pole_pairs;          // 极对数
    int32 base_rpm;             // 基准转速
} EQEP_PosSpeed;

//
// eQEP1 API
//
void eqep1_init(void);
void eqep1_calc(EQEP_PosSpeed *p);
void eqep1_reset_counter(void);
Uint32 eqep1_get_position(void);
int16 eqep1_get_direction(void);

//
// 结构体初始化默认值
//
#define EQEP_POSSPEED_DEFAULTS { \
    0, 0, 0,                    /* theta_elec, theta_mech, direction */ \
    0.0F, 0.0F, 0, 0,           /* speed_pu_fr, speed_pu_pr, speed_rpm_fr, speed_rpm_pr */ \
    0, 0, 0, 0.0F,              /* theta_raw, cal_angle, index_sync_flag, oldpos */ \
    EQEP_POLE_PAIRS,            /* pole_pairs */ \
    EQEP_BASE_RPM               /* base_rpm */ \
}

#endif  // __EQEP_H__