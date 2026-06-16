#include "adc.h"

//
// ePWM2 波谷(SOCA) → ADC SEQ1 自动转换 → INT_SEQ1 → CPU ISR → 回调
// CPU 不再轮询、不再软件触发，采样点与 PWM 严格同步在波谷，电流最干净。
//

static adc_conv_cb_t s_adc_cb = 0;
static volatile Uint16 s_adc_last = 0;

//
// SEQ1 转换完成中断：读结果 → 通知上层(FOC 电流环) → 复位排序器 → 应答 PIE
// RST_SEQ1 必须做：单次/级联模式下排序器转换完会停在末尾，不复位则下一个
// SOCA 触发不会从 CONV00 重新开始，采样链就此卡死。
//
static interrupt void adc_seq1_isr(void)
{
    s_adc_last = AdcMirror.ADCRESULT0;

    if (s_adc_cb != 0)
    {
        s_adc_cb(s_adc_last);
    }

    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;   // 排序器指针复位到 CONV00
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1; // 清 SEQ1 中断标志

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; // 应答 PIE 组1，放行后续中断
}

void adc_init(void)
{
    InitAdc(); // adc校准、给adc上电并等待ADC稳定

    //
    // 设置ADC时钟个时钟必须小于25Mhz，否则采样保持转换时间不够
    // ADCCLKPS == 0 : ADCCLK = HSPCLK / (CPS + 1)
    // ADCCLKPS != 0 : ADCCLK = HSPCLK / [2 * ADCCLKPS * (CPS + 1)]
    // 75MHz / [2 * 2 * (1 + 1)] = 9.375MHz
    //
    AdcRegs.ADCTRL3.bit.ADCCLKPS = 2; // 对高速时钟进行分频
    AdcRegs.ADCTRL1.bit.CPS = 1;      // 对分频后的时钟再次分频

    AdcRegs.ADCTRL1.bit.ACQ_PS = 7;   // 采样窗口 = 8个ADCCLK
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1; // 级联模式
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0; // 单次运行，每次靠 SOCA 触发

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;  // 把ADCINA0添加到排序器，放在第0位
    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x0; // 每轮转换一个通道

    //
    // 触发与中断：
    // EPWM_SOCA_SEQ1 — SEQ1 由 ePWM 的 SOCA 事件硬件触发（不再软件写 SOC_SEQ1）
    // INT_MOD_SEQ1=0 — 每个 SEQ1 转换序列结束都产生中断
    // INT_ENA_SEQ1   — 使能 SEQ1 中断
    //
    AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; // 使能 ePWM 的 SOCA 事件硬件触发
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0;   // 每个 SEQ1 转换序列结束都产生中断
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;   // 使能 SEQ1 中断使能
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;     // 清掉初始残留标志
}

//
// 注册转换完成回调。要在 adc_irq_enable() 之前调用。
// FOC 里把 Clarke/Park → 电流环 → epwm2_set_duty 挂在这个回调里。
//
void adc_reg_cb(adc_conv_cb_t cb)
{
    s_adc_cb = cb;
}

//
// 把 ADC 中断接入：PIE 组1第6位(ADCINT) → CPU INT1。全局中断已在 main 开启。
//
void adc_irq_enable(void)
{
    EALLOW;
    PieVectTable.ADCINT = &adc_seq1_isr;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx6 = 1; // 使能 PIE 组1.6 (ADCINT)
    IER |= M_INT1;                     // 使能 CPU INT1
}

//
// 读最近一次采样结果(0~4095)。主循环里想观察时用，FOC 实时值仍走回调。
//
Uint16 adc_get_last(void)
{
    return s_adc_last;
}

//
// 以下为只读状态查询，仅用于调试/观察采样链健康
//
Uint16 adc_seq_busy(void)
{
    return AdcRegs.ADCST.bit.SEQ1_BSY;
}

Uint16 adc_seq_remaining(void)
{
    return AdcRegs.ADCASEQSR.bit.SEQ_CNTR;
}

Uint16 adc_int_pending(void)
{
    return AdcRegs.ADCST.bit.INT_SEQ1;
}
