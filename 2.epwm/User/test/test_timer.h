#ifndef __TEST_TIMER_H__
#define __TEST_TIMER_H__

//
// 定时器测试用例
// setup: 完成本测试所需的外设装配（一次性）
// loop : 主循环体，需由 main 的 while(1) 反复调用
//
void test_timer_init(void);
void test_timer_loop(void);

#endif
