#include "test/test_adc.h"
#include "DSP28x_Project.h"
#include "bsp/adc.h"
#include "bsp/epwm.h"
#include "bsp/sci.h"

//
// ADC 中断驱动测试（FOC 采样链路）
//
// 链路：EPWM2 波谷(SOCA) → ADC SEQ1 → INT_SEQ1 → adc 回调
// 回调按 PWM 频率(10kHz)进，里面绝不能打串口(9600 太慢会拖垮中断)，
// 只做两件事：存最近一次原始值、累加计数。
// 主循环每 500ms 打印一次最近值，并用计数差算出 SOCA 实际触发频率，
// 频率≈10000Hz 就证明 ePWM→ADC→ISR 整条链通了。
//
// 本测试覆盖 adc.h 全部对外接口：
//   adc_init / adc_reg_cb / adc_irq_enable  靠 f≈10000Hz 证明整链贯通
//   adc_get_last     与回调抓到的 s_raw 对照，应一致(last==raw)
//   adc_seq_busy     随机时刻采样，单通道转换极快，busy 多为 0
//   adc_seq_remaining 单通道级联，转完回到 0(rem 多为 0)
//   adc_int_pending  ISR 已清标志，loop 观察 int 多为 0
//

#define ADC_VREF_MV 3000UL

static volatile Uint16 s_raw = 0;   // 回调写入的最近一次采样值
static volatile Uint32 s_count = 0; // 回调累计次数，用于验证触发率

//
// ADC 转换完成回调：跑在中断上下文，必须极短、不阻塞、不打串口
// FOC 里这里换成 Clarke/Park → 电流环 → epwm2_set_duty
//
static void adc_on_conv(Uint16 result)
{
    s_raw = result;
    s_count++;
}

static void put_u16(char *buf, Uint16 *idx, Uint16 val)
{
    char tmp[5];
    Uint16 i = 0;

    if (val == 0)
    {
        buf[(*idx)++] = '0';
        return;
    }

    while (val > 0)
    {
        tmp[i++] = (char)('0' + (val % 10));
        val /= 10;
    }
    while (i > 0)
    {
        buf[(*idx)++] = tmp[--i];
    }
}

static void put_u32(char *buf, Uint16 *idx, Uint32 val)
{
    char tmp[10];
    Uint16 i = 0;

    if (val == 0)
    {
        buf[(*idx)++] = '0';
        return;
    }

    while (val > 0)
    {
        tmp[i++] = (char)('0' + (val % 10));
        val /= 10;
    }
    while (i > 0)
    {
        buf[(*idx)++] = tmp[--i];
    }
}

void test_adc_init(void)
{
    scia_init();

    //
    // ADC：先注册回调，再把中断接进 PIE，最后才让触发源跑起来
    //
    adc_init();
    adc_reg_cb(adc_on_conv);
    adc_irq_enable();

    //
    // 触发源 EPWM2：init 里已配好 SOCA(波谷触发 ADC)
    // TBCLK 必须用全局开关统一启停，一旦 start，波谷就开始按 10kHz 触发 ADC
    //
    epwm_tbclk_stop();
    epwm2_init();
    epwm_tbclk_start();

    scia_send_str("\r\nADC ISR test ready. EPWM2 SOCA -> ADC -> ISR @10kHz...\r\n");
}

void test_adc_loop(void)
{
    Uint32 c0 = s_count;
    DELAY_MS(500);
    Uint32 freq = (s_count - c0) * 2UL; // 500ms 内的次数 ×2 = 每秒触发率

    Uint16 raw  = s_raw;
    Uint16 last = adc_get_last();       // 应与回调抓到的 raw 一致
    Uint16 busy = adc_seq_busy();       // 随机时刻，单通道转换极快，多为 0
    Uint16 rem  = adc_seq_remaining();  // 单通道级联，转完回到 0
    Uint16 ipd  = adc_int_pending();    // ISR 已清标志，多为 0
    Uint16 mv   = (Uint16)((Uint32)raw * ADC_VREF_MV / 4095UL);

    char line[120];
    Uint16 idx = 0;

    line[idx++] = 'r';
    line[idx++] = 'a';
    line[idx++] = 'w';
    line[idx++] = '=';
    put_u16(line, &idx, raw);

    line[idx++] = ' ';
    line[idx++] = 'V';
    line[idx++] = '=';
    put_u16(line, &idx, mv / 1000);
    line[idx++] = '.';
    put_u16(line, &idx, (mv % 1000) / 100);
    put_u16(line, &idx, (mv % 100) / 10);
    put_u16(line, &idx, mv % 10);

    line[idx++] = ' ';
    line[idx++] = 'f';
    line[idx++] = '=';
    put_u32(line, &idx, freq);
    line[idx++] = 'H';
    line[idx++] = 'z';

    line[idx++] = ' ';
    line[idx++] = 'l';
    line[idx++] = 'a';
    line[idx++] = 's';
    line[idx++] = 't';
    line[idx++] = '=';
    put_u16(line, &idx, last);

    line[idx++] = ' ';
    line[idx++] = 'b';
    line[idx++] = 's';
    line[idx++] = 'y';
    line[idx++] = '=';
    put_u16(line, &idx, busy);

    line[idx++] = ' ';
    line[idx++] = 'r';
    line[idx++] = 'e';
    line[idx++] = 'm';
    line[idx++] = '=';
    put_u16(line, &idx, rem);

    line[idx++] = ' ';
    line[idx++] = 'i';
    line[idx++] = 'n';
    line[idx++] = 't';
    line[idx++] = '=';
    put_u16(line, &idx, ipd);

    line[idx++] = '\r';
    line[idx++] = '\n';
    line[idx]   = '\0';

    scia_send_str(line);
}