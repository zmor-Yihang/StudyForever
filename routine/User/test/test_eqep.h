#ifndef __TEST_EQEP_H__
#define __TEST_EQEP_H__

#include "DSP28x_Project.h"

//
// 模拟编码器测试（推荐）
//
void test_eqep_sim_init(void);      // 初始化：配置 ePWM1 生成正交信号
void test_eqep_sim_loop(void);      // 循环：计算并输出位置速度

//
// 基础测试（需连接真实编码器）
//
void test_eqep_basic_init(void);    // 初始化：仅配置 eQEP1
void test_eqep_basic_loop(void);    // 循环：读取位置和速度

#endif  // __TEST_EQEP_H__

