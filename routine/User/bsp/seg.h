#ifndef __SEG_H__
#define __SEG_H__

#include "DSP28x_Project.h"

//
// 4 位共阳数码管驱动（IC3 74HC164 段驱动 + GPIO 位选）
//
// 段码路径：应用只写显存，主循环周期调用 seg_scan_step() 逐位扫描。
//   每步：灭全部位选 → 把本位段码经 SPI-A 移入 74HC164 → 选通本位 → 位号自增。
//   段码走 SPISIMOA/SPICLKA，由 spi.c 的 spia_send_block 发出；74HC164 无片选/锁存，
//   故本驱动不碰 spia_cs_*。调用 seg_init 前必须先 spia_init。
//
// 位序：74HC164 把 DS 每拍移向 Q7，SPI 又 MSB 先发，于是字节 bitN→QN，
//   即 bit0=a,bit1=b,...,bit6=g,bit7=dp，正好标准段序，无需翻转。显存按"亮=1"存。
//

//
// 位选：SEG1~SEG4 接 GPIO70~73(连续)，均在 GPIO Port C，高电平选通对应位公共端。
// 改引脚/位数只动这三个宏；底层用 Port C 的 SET/CLEAR.all 按掩码操作，无需逐脚具名。
//
#define SEG_DIGIT_COUNT 4
#define SEG_DIGIT_GPIO_BASE 70
#define SEG_DIGIT_PC_SHIFT (SEG_DIGIT_GPIO_BASE - 64) // Port C 内位偏移(GPIO64 为 bit0)

//
// 极性开关，按面板器件类型改这里即可：
//   SEG_ACTIVE_LOW=1        共阳数码管，段输出拉低点亮，发送前段码整字取反
//   SEG_DIGIT_ACTIVE_LOW=0   S8050 NPN 高边位选，基极拉高导通(选通)，拉低关断(消隐)
//
#define SEG_ACTIVE_LOW 1
#define SEG_DIGIT_ACTIVE_LOW 0

//
// 显存第 0 位为最左(最高位)，第 SEG_DIGIT_COUNT-1 位为最右(最低位)。
// 若实物左右顺序相反，调换 digit_on 的位映射或在 set 系列里反序即可。
//

void seg_init(void);                                   // 配 GPIO70~73 为位选输出(全灭)、清显存；调用前须先 spia_init()
void seg_clear(void);                                  // 显存清空(全灭)
void seg_set_raw(Uint16 pos, Uint16 segbits);          // 第 pos 位写原始段位(bit0=a..bit7=dp，亮=1)，自定义字形用
void seg_set_digit(Uint16 pos, Uint16 val, Uint16 dp); // 第 pos 位显示 0~F(val&0xF)，dp 非 0 点亮小数点
void seg_set_uint(Uint16 value);                       // 十进制右对齐显示 0~9999，高位前导零消隐
void seg_set_hex(Uint16 value);                        // 4 位十六进制显示 0x0000~0xFFFF
void seg_scan_step(void);                              // 步进一位:灭→移段码→选通；主循环周期调用(建议 2~5ms 一次)

#endif