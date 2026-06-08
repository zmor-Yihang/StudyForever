#include "DSP28x_Project.h"
#include "bsp/led.h"
#include "bsp/key.h"
#include "bsp/timer.h"

// #define FLASH  // 代码烧录到Flash中，这样就可以在Flash中运行TI官方函数

// 定时器中断回调：每个周期翻转一次 LED
static void on_timer_tick(void)
{
    led_toggle(LED_D8);
}

int main(void)
{
    InitSysCtrl(); // 配置系统时钟150MHz，禁止看门狗

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

    // 初始化 LED
    led_init(LED_D8);
    led_init(LED_D9);
    led_init(LED_D10);
    led_init(LED_D11);
    led_init(LED_D12);
    led_init(LED_D13);
    led_init(LED_D14);

    // 初始化矩阵键盘
    key_init();

    // 初始化定时器并接入中断：每秒翻转一次 LED_D8
    timer0_init();
    timer0_set_callback(on_timer_tick);
    timer0_irq_enable();
    timer0_start();

    key_id_t key = KEY_NONE;
    key_id_t prev_key = KEY_NONE;

    while (1)
    {
        key = key_scan();

        // 检测按键按下事件(上升沿检测：从无按键到有按键)
        if (key != KEY_NONE && key != prev_key)
        {
            switch (key)
            {
            case KEY_SW1:
                led_toggle(LED_D8);
                break;
            case KEY_SW2:
                led_toggle(LED_D9);
                break;
            case KEY_SW3:
                led_toggle(LED_D10);
                break;
            case KEY_SW4:
                led_toggle(LED_D11);
                break;
            case KEY_SW5:
                led_toggle(LED_D12);
                break;
            case KEY_SW6:
                led_toggle(LED_D13);
                break;
            case KEY_SW7:
                led_toggle(LED_D14);
                break;
            default:
                break;
            }
        }
        prev_key = key;

        DELAY_MS(10); // 扫描间隔约 10ms
    }
}
