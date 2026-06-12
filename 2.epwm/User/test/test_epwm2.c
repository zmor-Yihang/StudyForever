#include "test/test_epwm2.h"
#include "DSP28x_Project.h"
#include "bsp/epwm.h"
#include "bsp/key.h"

//
// 占空比扫描参数
// DUTY_STEP       : 每个 loop 周期占空比的增减量
// DUTY_DELAY_MS   : 相邻两步之间的延时
// DUTY_MIN/MAX    : 占空比钳位范围，互补模式下避免极端值导致直通
// 当前取值：0.1->0.9 与 0.9->0.1 各约 0.8s，一个完整扫描约 1.6s
//
#define DUTY_STEP 0.03F
#define DUTY_DELAY_MS 10
#define DUTY_MIN 0.1F // 互补 PWM 不宜给 0%，下管需要最小导通时间
#define DUTY_MAX 0.9F // 互补 PWM 不宜给 100%，上管需要最小关断时间

//
// 故障状态标志：由 TZ 中断回调置 1，主循环检测后做故障处理与恢复
// volatile 防止编译器优化掉对它的读取
//
static volatile Uint16 s_fault_flag = 0;

//
// 故障恢复延时：跳闸后等多久再尝试清除故障
// 避免故障源未消失就立刻恢复导致反复跳闸
//
#define FAULT_RECOVER_DELAY_MS 3000

//
// TZ 跳闸回调：在中断上下文中执行，仅置标志位，不做耗时操作
//
static void tz_fault_callback(void)
{
    s_fault_flag = 1;
}

void test_epwm2_init(void)
{
    epwm_tbclk_stop();

    // ePWM2 初始化：互补输出 GPIO2/3、死区 1µs、TZ5 故障保护、ADC 触发
    // 注意：不在内部启动 TBCLK，由 main 统一管理
    epwm2_init();

    // 先注册回调，再使能中断，保证中断到来时回调已就绪
    epwm2_reg_tz_cb(tz_fault_callback);

    // 使能 TZ 中断接入 PIE 和 CPU
    epwm2_tz_irq_enable();

    // 矩阵键盘初始化，用于 SW1 软件触发故障
    key_init();

    epwm_tbclk_start();
}

void test_epwm2_loop(void)
{
    static float32 duty = 0.5F; // 当前占空比，初始 50% 安全中性点
    static int16 dir = 1;       // 渐变方向：+1 增大，-1 减小

    //
    // SW1 软件触发故障：按下 SW1 通过 TZFRC 寄存器强制产生 one-shot 跳闸
    // 效果与外部 TZ5 引脚拉低完全相同，硬件会立刻拉低 2A/2B 并触发中断
    // 使用边沿检测，同一次按下只触发一次
    //

    static Uint16 last_sw1 = 0;
    Uint16 cur_sw1 = (key_scan() == KEY_SW1) ? 1 : 0;
    if (cur_sw1 && !last_sw1) // 上升沿：刚按下
    {
        epwm2_force_trip();
    }
    last_sw1 = cur_sw1;

    //
    // 故障检测与处理
    // TZ 触发 → 硬件自动拉低 2A/2B → ISR 清除 OST+INT 并置 s_fault_flag
    // 主循环检测到标志后延时，再清除软件标志并恢复到安全占空比
    //
    if (s_fault_flag)
    {
        // 延时等待故障源消失（如过流已退、外部信号已恢复高电平）
        DELAY_MS(FAULT_RECOVER_DELAY_MS);

        // 清除 one-shot 锁存，恢复 PWM 输出
        epwm2_clear_fault();

        // 清标志，回到正常扫描
        s_fault_flag = 0;

        // 恢复到安全占空比
        duty = 0.5F;
        dir = 1;
    }

    //
    // 占空比往返扫描：在 [DUTY_MIN, DUTY_MAX] 之间线性渐变
    // 互补 PWM 下 A 路占空比 = duty，B 路 = (1-duty) 减去死区
    //
    duty += dir * DUTY_STEP;

    if (duty >= DUTY_MAX)
    {
        duty = DUTY_MAX;
        dir = -1;
    }
    else if (duty <= DUTY_MIN)
    {
        duty = DUTY_MIN;
        dir = 1;
    }

    epwm2_set_duty(duty);

    DELAY_MS(DUTY_DELAY_MS);
}
