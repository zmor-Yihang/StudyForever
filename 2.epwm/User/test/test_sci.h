#ifndef __TEST_SCI_H__
#define __TEST_SCI_H__

//
// SCI-A 测试用例
// init: 初始化 SCI-A 并发送启动提示
// loop: 主循环体，收到串口数据后原样回显
//
void test_sci_init(void);
void test_sci_loop(void);

#endif
