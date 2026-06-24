#ifndef __TEST_SEG_H__
#define __TEST_SEG_H__

//
// 4 位数码管(IC3 74HC164)测试用例
// init: 初始化 SPI-A 与数码管驱动，显示固定数 1234
// loop: 扫描刷新一位(自带驻留延时)
//
// 用法(main.c)：test_seg_init(); 之后 while(1) { test_seg_loop(); }
//   延时已在 loop 内，主循环不要再加 DELAY。
//
void test_seg_init(void);
void test_seg_loop(void);

#endif