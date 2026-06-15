#include "epwm.h"

//
// ── 全局时基时钟控制 ──
// TBCLKSYNC 是所有 ePWM 模块共享的时基开关，不能由单个模块的 init 来操作，
// 否则后初始化的模块会打断已在运行的模块。
//

//
// 暂停所有 ePWM 时基计数时钟 TBCLK
// 在所有 epwmX_init() 之前调用一次即可
//
void epwm_tbclk_stop(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;
}

//
// 启动所有 ePWM 时基计数时钟 TBCLK
// 在所有 epwmX_init() 之后调用一次，保证各模块相位对齐起跑
//
void epwm_tbclk_start(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

//
// EPWM1：普通 PWM（单路 A 输出）
//

void epwm1_init(void)
{
    //
    // 注意：不再操作 TBCLKSYNC，由调用者在外部统一管理
    // 初始化顺序：epwm_tbclk_stop() → epwm1_init() → epwm2_init() → epwm_tbclk_start()
    //

    //
    // ePWM1 对应 GPIO0 和 GPIO1 引脚
    //
    InitEPwm1Gpio();

    //
    // 150MHz, 中心对齐计数模式，计数器周期为EPWM1_PERIOD
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // 计数模式：中心对齐
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // 相位偏移使能：禁止
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // 预装载：影子模式
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; // 同步选择：禁止
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // 时钟分频：1
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;           // 时钟分频：1

    //
    // 设置计数器初始值和计数周期
    //
    EPwm1Regs.TBCTR = 0;            // 计数器初始值为0
    EPwm1Regs.TBPRD = EPWM1_PERIOD; // 计数器周期为EPWM1_PERIOD

    //
    // 设置比较值，这里只配置CMPA比较值，CMPB比较值默认不使用
    //
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;               // 比较值影子模式
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;             // 计数器为0时加载
    EPwm1Regs.CMPA.half.CMPA = (Uint16)(EPWM1_PERIOD * 0.5F); // 设置比较值，普通PWM模式

    //
    // 设置动作限定
    // 下面这种配置，CMPA越大，占空比越大
    //
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // CAU事件触发输出置0
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;   // CAD事件触发输出置1

    //
    // 设置死区控制
    // 不使用死区控制，只用ePWM1 A 路输出，不存在死区问题
    //
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;
    EPwm1Regs.PCCTL.bit.CHPEN = CHP_DISABLE;
}

//
// 释放软件强制输出控制，允许ePWM1A输出
// 这个寄存器默认是0，所以初始化后不需要调用这个函数
// 这个函数是搭配epwm1_stop函数使用的
//
void epwm1_start(void)
{
    EPwm1Regs.AQCSFRC.bit.CSFA = 0; // 允许ePWM1A输出
}

//
// 停止ePWM1A输出，强制为高电平
//
void epwm1_stop(void)
{
    EPwm1Regs.AQCSFRC.bit.CSFA = 2; // 强制 ePWM1A 为高
}

//
// 运行时修改 EPWM1 占空比
// 比较值使用影子模式，新值在计数器到 0 时加载，不会产生毛刺
//
void epwm1_set_duty(float32 duty)
{
    EPwm1Regs.CMPA.half.CMPA = (Uint16)(EPWM1_PERIOD * duty);
}

//
// 运行时修改 EPWM1 频率
// TBPRD 使用影子模式，新值在计数器到 0 时加载，不会产生毛刺
// 调用后需同步调用 epwm1_set_duty() 更新比较值，否则占空比会偏移
//
void epwm1_set_freq(Uint32 freq_hz)
{
    Uint16 period = (Uint16)(EPWM1_TBCLK_HZ / (2UL * freq_hz));
    EPwm1Regs.TBPRD = period;
}

//
// EPWM2：互补 PWM + 死区 + TZ 保护 + ADC 触发
//

static epwm_tz_cb_t s_epwm2_tz_cb = 0;

//
// EPWM2 跳闸中断：硬件已自动把 2A/2B 拉到安全态，这里只做"通知上层" + 清除锁存
// 必须同时清除 OST 和 INT：
//   - 只清 INT 不清 OST → OST 锁存位仍为 1，中断条件持续成立，ISR 被反复触发形成中断风暴
//   - 同时清 OST + INT → 中断条件解除，主循环可正常执行故障恢复逻辑
//
static interrupt void epwm2_tzint_isr(void)
{
    if (s_epwm2_tz_cb != 0)
    {
        s_epwm2_tz_cb();
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2; // 应答 PIE 组2，放行后续中断
}

void epwm2_init(void)
{
    //
    // 注意：不再操作 TBCLKSYNC，由调用者在外部统一管理
    //

    //
    // EPWM2A=GPIO2, EPWM2B=GPIO3
    //
    InitEPwm2Gpio();

    //
    // 把 GPIO16 复用成 TZ5，作为故障输入。异步采样(QSEL=3)保证故障能立刻穿透、不受时钟门控
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // 使能上拉，悬空时默认不触发
    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // 异步输入
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 3;  // 复用为 TZ5
    EDIS;

    //
    // 时基：中心对齐，不分频，周期 EPWM2_PERIOD
    //
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm2Regs.TBCTR = 0;
    EPwm2Regs.TBPRD = EPWM2_PERIOD;

    //
    // 比较：影子加载、计数到0时生效。互补模式下只需配 CMPA，下管由死区模块生成
    // 初始 50% 占空比是中性安全点，实际由控制算法调 epwm2_set_duty 更新
    //
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPA.half.CMPA = (Uint16)(EPWM2_PERIOD * 0.5F);

    //
    // 动作限定：只配 A 路。向上撞 CMPA 置低、向下撞 CMPA 置高 → CMPA 越大占空比越大
    // B 路不配 AQ，完全交给死区模块取反生成
    //
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;

    //
    // 死区
    // IN_MODE = DBA_ALL   两沿都以 A 为源
    // POLSEL  = DB_ACTV_HIC  A 不变、B 取反 → 高有效互补对
    // OUT_MODE= DB_FULL_ENABLE  上升沿/下降沿都插死区
    // DBRED/DBFED = 死区计数，防上下桥臂直通
    //
    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm2Regs.DBRED = EPWM2_DB_COUNT;
    EPwm2Regs.DBFED = EPWM2_DB_COUNT;

    //
    // 不用斩波功能
    //
    EPwm2Regs.PCCTL.bit.CHPEN = CHP_DISABLE;

    //
    // 故障保护(Trip Zone)：TZ5 一次性锁存
    // 跳闸时 2A 拉高，2B 拉低，这里为了方便led观察，所以2A拉高，2B拉低
    // 实际使用时，根据实际情况配置电平
    // one-shot 锁死后需 epwm2_clear_fault() 才恢复
    //
    EALLOW;
    EPwm2Regs.TZSEL.bit.OSHT5 = TZ_ENABLE; // TZ5 选为 one-shot 源
    EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_HI; // 跳闸时 2A 拉高
    EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // 跳闸时 2B 拉低
    EPwm2Regs.TZEINT.bit.OST = 1;          // 使能 one-shot 跳闸中断
    EPwm2Regs.TZCLR.all = 0x0007;          // 清掉初始残留标志(INT/CBC/OST)
    EDIS;

    //
    // 事件触发 ADC：计数到 0(波谷)产生 SOCA，每周期一次
    // 波谷采样避开开关时刻噪声，得到的电流值最干净
    //
    EPwm2Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO; // 事件触发A触发条件为计数器到0
    EPwm2Regs.ETPS.bit.SOCAPRD = ET_1ST;       // 事件触发A触发分频为1，就是每周期一次
    EPwm2Regs.ETSEL.bit.SOCAEN = 1;            // 使能事件触发A
    EPwm2Regs.ETCLR.bit.SOCA = 1;              // 清掉初始残留标志(SOCA)
}

//
// 设置 EPWM2 占空比
// 占空比是由比较值决定的
//
void epwm2_set_duty(float32 duty)
{
    EPwm2Regs.CMPA.half.CMPA = (Uint16)(EPWM2_PERIOD * duty);
}

//
// 设置 EPWM2 频率
// 频率是周期值决定
//
void epwm2_set_freq(Uint32 freq_hz)
{
    Uint16 period = (Uint16)(EPWM2_TBCLK_HZ / (2UL * freq_hz));
    EPwm2Regs.TBPRD = period;
}

//
// 注册跳闸回调。要在 epwm2_tz_irq_enable() 之前调用
//
void epwm2_reg_tz_cb(epwm_tz_cb_t cb)
{
    s_epwm2_tz_cb = cb;
}

//
// 把 TZ 中断接入：PIE 组2第2位 → CPU INT2。全局中断已在 main 开启
//
void epwm2_tz_irq_enable(void)
{
    EALLOW;
    PieVectTable.EPWM2_TZINT = &epwm2_tzint_isr;
    EDIS;

    PieCtrlRegs.PIEIER2.bit.INTx2 = 1; // 使能 PIE 组2.2 (EPWM2_TZINT)
    IER |= M_INT2;                     // 使能 CPU INT2
}

//
// 清除 one-shot 锁存，恢复 PWM 输出。
// 必须在确认故障源已消失后再调用，否则会立刻被再次锁死
//
void epwm2_clear_fault(void)
{
    EALLOW;
    EPwm2Regs.TZCLR.bit.OST = 1; // 清除故障标志位
    EPwm2Regs.TZCLR.bit.INT = 1; // 清除中断标志位
    EDIS;
}

//
// 软件强制触发 TZ one-shot 故障，效果与外部 TZ5 引脚拉低相同
// 写 TZFRC.OST=1 会立刻产生 one-shot 事件，硬件拉低 2A/2B 并触发中断
//
void epwm2_force_trip(void)
{
    EALLOW;
    EPwm2Regs.TZFRC.bit.OST = 1;
    EDIS;
}
