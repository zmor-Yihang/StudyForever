#include "timer.h"
#include "DSP2833x_CpuTimers.h"

static timer_cb_t s_timer_cb = 0;

//
// CpuTimer0 是 CpuTimer0 的配置结构体实例，声明在 DSP2833x_CpuTimers.h 中
// 定时器初始化，初始化周期为1s，频率为15MHz
//
void timer0_init(void)
{
    //
    // CpuTimer0Regs的值由DSP2833x_Headers_nonBIOS.cmd文件定义，链接到真实硬件地址
    // CPU_TIMER0  : origin = 0x000C00, length = 0x000008
    //
    CpuTimer0.RegsAddr = &CpuTimer0Regs;

    //
    // Initialize timer with 15MHz clock and 1s period
    //
    ConfigCpuTimer(&CpuTimer0, 150, 1000000);
}

void timer0_start(void)
{
    StartCpuTimer0();
}

void timer0_stop(void)
{
    StopCpuTimer0();
}

void timer0_reload(void)
{
    ReloadCpuTimer0();
}

void timer0_set_callback(timer_cb_t cb)
{
    s_timer_cb = cb;
}

//
// 将 CpuTimer0 的中断 TINT0 接入中断系统：
// PIE 组1第7位 -> CPU INT1 -> 全局中断已在 main 中开启
//
void timer0_irq_enable(void)
{
    EALLOW;
    PieVectTable.TINT0 = &timer0_isr;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // 使能 PIE 组1.7 (TINT0)
    IER |= M_INT1;                     // 使能 CPU INT1
}

//
// 计数器递减到0触发：执行回调（翻转LED），并应答中断
//
__interrupt void timer0_isr(void)
{
    if (s_timer_cb != 0)
    {
        s_timer_cb();
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; // 应答 PIE 组1，允许后续中断
}

Uint32 timer0_read_counter(void)
{
    return ReadCpuTimer0Counter();
}

Uint32 timer0_read_period(void)
{
    return ReadCpuTimer0Period();
}

