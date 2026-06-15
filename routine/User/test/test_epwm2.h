#ifndef __TEST_EPWM2_H__
#define __TEST_EPWM2_H__

//
// ePWM2 互补 PWM + 死区 + TZ 保护测试用例
// init: 装配 ePWM2（互补输出、死区、TZ5 故障保护）并启动时基时钟
// loop: 主循环体，由 main 的 while(1) 反复调用
//       - 扫描占空比并检测故障状态
//       - 检测 SW1 按键，软件触发一次 TZ one-shot 故障
//
void test_epwm2_init(void);
void test_epwm2_loop(void);

#endif
