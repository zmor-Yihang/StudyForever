#include "DSP28x_Project.h"
#include "bsp/led.h"

// #define FLASH  // 代码烧录到Flash中，这样就可以在Flash中运行TI官方函数

int main(void)
{
    InitSysCtrl();      // 配置系统时钟150MHz，禁止看门狗
    DINT;               // 禁止全局中断
    InitPieCtrl();      // 初始化中断控制器
    IER = 0x0000;       // 禁止所有中断
    IFR = 0x0000;       // 清除中断标志位
    InitPieVectTable(); // 初始化中断向量表
    EINT;               // 使能全局中断
    ERTM;               // 使能实时中断

    // TI官方函数写在RAM中，所以需要将RAM中的代码复制到Flash中
    // 这样就可以在Flash中运行TI官方函数
#ifdef FLASH
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    InitFlash(); // 初始化Flash
#endif

    // 初始化LED
    led_init();

    while (1)
    {
        led_toggle();   // 翻转LED状态
        DELAY_MS(1000); // 延时500ms
    }
}
