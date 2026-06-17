#include "ecap.h"

static ecap_callback_t s_ecap_callback = 0;

//
// eCAP1 中断服务函数
// 捕获完成后调用用户注册的回调函数
//
static interrupt void ecap1_isr(void)
{
    // 调用用户回调
    if(s_ecap_callback != 0)
    {
        s_ecap_callback();
    }
    
    // 清除中断标志
    ECap1Regs.ECCLR.bit.CEVT3 = 1;
    ECap1Regs.ECCLR.bit.INT = 1;
    
    // 应答PIE中断
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

void ecap_init(void)
{
    InitECap1Gpio();

    EALLOW;
    
    // 停止计数器
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;
    
    // 控制寄存器1配置 - 测量周期和占空比
    ECap1Regs.ECCTL1.bit.CAP1POL = 0;       // CAP1上升沿捕获
    ECap1Regs.ECCTL1.bit.CTRRST1 = 1;       // CAP1事件发生时重置计数器
    ECap1Regs.ECCTL1.bit.CAP2POL = 1;       // CAP2下降沿捕获
    ECap1Regs.ECCTL1.bit.CTRRST2 = 0;       // CAP2事件不重置计数器
    ECap1Regs.ECCTL1.bit.CAP3POL = 0;       // CAP3上升沿捕获（下一个周期）
    ECap1Regs.ECCTL1.bit.CTRRST3 = 0;       // CAP3事件不重置计数器
    ECap1Regs.ECCTL1.bit.CAP4POL = 1;       // CAP4下降沿捕获（下一个周期）
    ECap1Regs.ECCTL1.bit.CTRRST4 = 0;       // CAP4事件不重置计数器
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // 使能捕获寄存器加载
    ECap1Regs.ECCTL1.bit.PRESCALE = 0;      // 不分频，每次事件都捕获
    ECap1Regs.ECCTL1.bit.FREE_SOFT = 3;     // 仿真模式自由运行
    
    // 控制寄存器2配置
    ECap1Regs.ECCTL2.bit.CAP_APWM = 0;      // 选择捕获模式
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;   // 连续模式
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;     // 第4次捕获事件后回卷
    ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;      // 禁用同步输入
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 2;     // 禁用同步输出
    ECap1Regs.ECCTL2.bit.SWSYNC = 0;        // 不进行软件同步
    
    // 中断配置（默认不使能，由 ecap_irq_enable 函数使能）
    ECap1Regs.ECEINT.bit.CEVT3 = 1;         // 使能CAP3事件中断源
    
    // 清除所有标志
    ECap1Regs.ECCLR.all = 0xFFFF;
    
    // 计数器清零
    ECap1Regs.TSCTR = 0;
    
    // 启动计数器
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;
    
    EDIS;
}

Uint32 ecap_get_period(void)
{
    return ECap1Regs.CAP3;  // CAP3存储的是周期值（从第一个上升沿到下一个上升沿）
}

Uint32 ecap_get_high_time(void)
{
    return ECap1Regs.CAP2;  // CAP2存储的是高电平持续时间（从上升沿到下降沿）
}

float ecap_get_duty_cycle(void)
{
    Uint32 period = ECap1Regs.CAP3;
    Uint32 high_time = ECap1Regs.CAP2;
    
    if(period == 0)
        return 0.0f;
    
    return ((float)high_time / (float)period) * 100.0f;  // 返回百分比
}

//
// 使能 eCAP1 中断
// 需要在 ecap_init() 之后调用
//
void ecap_irq_enable(ecap_callback_t callback)
{
    s_ecap_callback = callback;
    
    EALLOW;
    // 配置中断向量
    PieVectTable.ECAP1_INT = &ecap1_isr;
    EDIS;
    
    // 使能PIE中断：eCAP1_INT 在 PIE 组4.1
    PieCtrlRegs.PIEIER4.bit.INTx1 = 1;
    
    // 使能CPU INT4
    IER |= M_INT4;
}

