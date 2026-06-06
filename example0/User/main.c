#include "DSP28x_Project.h"
#include "bsp/led.h"

//#define FLASH

int main(void)
{
    // 配置系统时钟150MHz，禁止看门狗
    InitSysCtrl();

#ifdef FLASH

    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    InitFlash();

#endif

    // 初始化LED
    led_init();

    while (1)
    {
        led_toggle();     // 翻转LED状态
        DELAY_MS(5000);   // 延时500ms
    }
}
