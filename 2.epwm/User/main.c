#include "DSP28x_Project.h"
#include "bsp/led.h"
#include "bsp/key.h"
#include "bsp/timer.h"

// #define FLASH  // 代码烧录到Flash中，这样就可以在Flash中运行TI官方函数

// 定时器中断回调：每个周期翻转一次 LED
static void on_timer_tick(void)
{
    led_toggle(LED_D8);
    //
    // 应答中断的代码写在中断函数timer0_isr中
    //
}

int main(void)
{
    InitSysCtrl(); // 配置系统时钟150MHz，禁止看门狗

    InitPieCtrl();      // 初始化中断控制器
    IER = 0x0000;       // 禁止所有中断
    IFR = 0x0000;       // 清除中断标志位
    InitPieVectTable(); // 初始化中断向量表并使能了PIE中断
    EnableInterrupts(); // 清除中断应答位并使能了CPU中断
    ERTM;               // 使能CPU实时中断，调试时能进入中断函数

    //
    // TI官方函数写在RAM中，所以需要将RAM中的代码复制到Flash中
    // 这样就可以在Flash中运行TI官方函数
    // 
#ifdef FLASH
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    InitFlash();
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

    timer0_init();
    timer0_register_isr_callback(on_timer_tick); // 注册定时器中断回调函数
    timer0_irq_enable();                         // 把中断函数地址赋值给 PIE 中断向量表，并使能PIE中断和CPU中断
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

        DELAY_MS(10);
    }
}
