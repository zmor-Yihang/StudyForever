#ifndef __TEST_EPWM_H__
#define __TEST_EPWM_H__

//
// ePWM 呼吸灯测试用例
// init: 装配 ePWM1 并启动时基时钟 TBCLK（一次性）
// loop: 主循环体，需由 main 的 while(1) 反复调用，让占空比往返渐变
//
void test_epwm_init(void);
void test_epwm_loop(void);

#endif
