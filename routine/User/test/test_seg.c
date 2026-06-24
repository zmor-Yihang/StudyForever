#include "test/test_seg.h"
#include "DSP28x_Project.h"
#include "bsp/spi.h"
#include "bsp/seg.h"

//
// 4 位数码管最简测试：显示固定 1234，主循环持续扫描刷新。
// 能正常显示即说明 SPI→74HC164 段码、位选、字模、极性都对。
//

void test_seg_init(void) {
    spia_init();        // 段码经 SPI-A → 74HC164，必须先于 seg_init
    seg_init();         // GPIO70~73 位选输出，清屏
    seg_set_uint(1234); // 要显示的内容
}

void test_seg_loop(void) {
    seg_scan_step();  // 每次刷一位，4 次走完一轮
    DELAY_US(2000);   // 单位驻留 2ms → 整屏 ~125Hz
}