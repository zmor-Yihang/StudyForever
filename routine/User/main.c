#include "DSP28x_Project.h"
#include "test/test_ecap.h"

int main(void)
{
    InitSysCtrl(); // 配置系统时钟150MHz，禁止看门狗

    InitPieCtrl();      // 初始化中断控制器
    IER = 0x0000;       // 禁止所有中断
    IFR = 0x0000;       // 清除中断标志位
    InitPieVectTable(); // 初始化中断向量表并使能了PIE中断
    EnableInterrupts(); // 清除中断应答位并使能了CPU中断
    ERTM;               // 使能CPU实时中断，调试时能进入中断函数

    test_ecap_init();
    DELAY_MS(1000);

    while (1)
    {
        test_ecap_loop();
    }
}
