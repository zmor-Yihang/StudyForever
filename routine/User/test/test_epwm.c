#include "test/test_epwm.h"
#include "DSP28x_Project.h"
#include "bsp/epwm.h"

//
// 呼吸灯节奏参数
// BREATH_STEP     : 每个 loop 周期占空比的增减量，越大呼吸越快
// BREATH_DELAY_MS : 相邻两步之间的延时，决定渐变的细腻程度
// 当前取值：0->1 与 1->0 各约 1s，一个完整呼吸约 2s
//
#define BREATH_STEP         0.01F
#define BREATH_DELAY_MS     10

void test_epwm_init(void)
{
    epwm_tbclk_stop();
    
    // ePWM1 中心对齐 10kHz，A 路输出在 GPIO0（外接 LED 接此脚）
    // 注意：不再在内部启动 TBCLK，由 main 统一管理
    epwm1_init();

    epwm_tbclk_start();
}

void test_epwm_loop(void)
{
    static float32 duty = 0.0F; // 当前占空比 [0,1]
    static int16 dir = 1;       // 渐变方向：+1 渐亮，-1 渐暗

    duty += dir * BREATH_STEP;

    // 撞到两端就夹紧并掉头，形成连续往返的呼吸
    if (duty >= 1.0F)
    {
        duty = 1.0F;
        dir = -1;
    }
    else if (duty <= 0.0F)
    {
        duty = 0.0F;
        dir = 1;
    }

    epwm1_set_duty(duty);

    DELAY_MS(BREATH_DELAY_MS);
}
