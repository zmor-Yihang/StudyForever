#include "DSP28x_Project.h"
#include "test/test_sci.h"

int main(void) {
    InitSysCtrl();      // 配置系统时钟150MHz，禁止看门狗
    InitPieCtrl();      // 初始化中断控制器
    IER = 0x0000;       // 禁止所有中断
    IFR = 0x0000;       // 清除中断标志位
    InitPieVectTable(); // 初始化中断向量表并使能PIE中断
    EnableInterrupts(); // 清除中断应答位并使能CPU中断
    ERTM;               // 使能CPU实时中断，调试时能进入中断函数

    //
    // 4 位数码管(IC3 74HC164)可视化自检
    // loop 内部自带扫描刷新与驻留延时，主循环不要再加 DELAY，否则闪烁
    //
    test_sci_init();

    while (1) {
        test_sci_loop();
    }
}
