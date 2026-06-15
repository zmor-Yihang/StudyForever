#include "test/test_timer.h"
#include "DSP28x_Project.h"
#include "bsp/led.h"
#include "bsp/key.h"
#include "bsp/timer.h"

// 定时器中断回调：每个周期翻转一次 LED
static void on_timer_tick(void)
{
    led_toggle(LED_D8);
    //
    // 清除中断标志位的代码写在中断函数timer0_isr中
    //
}

void test_timer_init(void)
{
    // 初始化 LED组
    led_init(LED_D8);
    led_init(LED_D9);
    led_init(LED_D10);
    led_init(LED_D11);
    led_init(LED_D12);
    led_init(LED_D13);
    led_init(LED_D14);

    // 初始化矩阵键盘
    key_init();

    // 初始化定时器0
    timer0_init();
    timer0_reg_isr_cb(on_timer_tick); // 注册定时器中断回调函数
    timer0_irq_enable();                         // 把中断函数地址赋值给 PIE 中断向量表，并使能PIE中断和CPU中断
    timer0_start();
}

void test_timer_loop(void)
{
    static key_id_t prev_key = KEY_NONE;

    key_id_t key = key_scan();

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
