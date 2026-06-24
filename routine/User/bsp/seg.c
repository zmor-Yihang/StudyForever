#include "seg.h"
#include "spi.h"

//
// 字模表：按"亮=1"存，bit0=a,bit1=b,...,bit6=g,bit7=dp(此处 dp=0)。
// 与共阳/共阴无关，输出时再按 SEG_ACTIVE_LOW 决定是否整字取反。索引 0~15 → 0~F。
//
static const Uint16 seg_font[16] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};

//
// 显存：每格按"亮=1"存(含 dp 位)。s_buf[0] 最左(高位)，末格最右(低位)。s_pos 为当前扫描位。
//
static Uint16 s_buf[SEG_DIGIT_COUNT];
static Uint16 s_pos = 0;

//
// 四位位选掩码(Port C 内)。低有效:关断=拉高、选通=拉低；高有效相反。
//
#define SEG_DIGIT_MASK (((Uint32)((1UL << SEG_DIGIT_COUNT) - 1UL)) << SEG_DIGIT_PC_SHIFT)

static void digit_all_off(void) {
#if SEG_DIGIT_ACTIVE_LOW
    GpioDataRegs.GPCSET.all = SEG_DIGIT_MASK; // 拉高 → 全部关断
#else
    GpioDataRegs.GPCCLEAR.all = SEG_DIGIT_MASK; // 拉低 → 全部关断
#endif
}

//
// 选通第 pos 位(pos 0→GPIO70 起按位偏移)。调用前位选已全灭，此处只点亮目标位。
//
static void digit_on(Uint16 pos) {
    Uint32 bit = (Uint32)1 << (SEG_DIGIT_PC_SHIFT + pos);
#if SEG_DIGIT_ACTIVE_LOW
    GpioDataRegs.GPCCLEAR.all = bit; // 拉低 → 选通
#else
    GpioDataRegs.GPCSET.all = bit; // 拉高 → 选通
#endif
}

//
// 配 GPIO70~73 为普通输出做位选，初值全灭；清显存。须在 spia_init 之后调用。
//
void seg_init(void) {
    EALLOW;
    GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO70 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO70 = 1;

    GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO71 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO71 = 1;

    GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO72 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO72 = 1;

    GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO73 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO73 = 1;
    EDIS;

    digit_all_off();
    seg_clear();
    s_pos = 0;
}

void seg_clear(void) {
    Uint16 i;
    for (i = 0; i < SEG_DIGIT_COUNT; i++) {
        s_buf[i] = 0x00;
    }
}

//
// 第 pos 位写原始段位(亮=1)，越界忽略。自定义字形/特殊符号走这里。
//
void seg_set_raw(Uint16 pos, Uint16 segbits) {
    if (pos < SEG_DIGIT_COUNT) {
        s_buf[pos] = (segbits & 0x00FF);
    }
}

//
// 第 pos 位显示 0~F(val&0xF)，dp 非 0 点亮小数点(bit7)。
//
void seg_set_digit(Uint16 pos, Uint16 val, Uint16 dp) {
    Uint16 code;
    if (pos >= SEG_DIGIT_COUNT) {
        return;
    }
    code = seg_font[val & 0x000F];
    if (dp) {
        code |= 0x80;
    }
    s_buf[pos] = code;
}

//
// 十进制右对齐显示，高位前导零消隐(至少保留最低位)。超量程按 SEG_DIGIT_COUNT 位截断。
//
void seg_set_uint(Uint16 value) {
    Uint16 i;
    Uint16 lead = 1; // 仍处于前导零区
    for (i = 0; i < SEG_DIGIT_COUNT; i++) {
        Uint16 pos = SEG_DIGIT_COUNT - 1 - i; // 从最低位(最右)往高位填
        Uint16 d = value % 10;
        value /= 10;

        if (d == 0 && value == 0 && i > 0 && lead) {
            s_buf[pos] = 0x00; // 高位前导零消隐
        } else {
            s_buf[pos] = seg_font[d];
            lead = 0;
        }
    }
}

//
// 4 位十六进制显示(不消隐前导零)。
//
void seg_set_hex(Uint16 value) {
    Uint16 i;
    for (i = 0; i < SEG_DIGIT_COUNT; i++) {
        Uint16 pos = SEG_DIGIT_COUNT - 1 - i;
        s_buf[pos] = seg_font[value & 0x000F];
        value >>= 4;
    }
}

//
// 步进一位扫描:先灭全部位选(防上一位残影串到本位)→把本位段码移入 74HC164→选通本位→位号自增。
// 共阳时整字取反(亮=1→输出 0 点亮)。spia_send_block 阻塞至该字节移完，故选通时段码已就位。
//
void seg_scan_step(void) {
    Uint16 tx;

    digit_all_off();

#if SEG_ACTIVE_LOW
    tx = (Uint16)((~s_buf[s_pos]) & 0x00FF);
#else
    tx = (Uint16)(s_buf[s_pos] & 0x00FF);
#endif
    spia_send_block(&tx, 1);

    digit_on(s_pos);

    s_pos++;
    if (s_pos >= SEG_DIGIT_COUNT) {
        s_pos = 0;
    }
}