#ifndef __SPI_H__
#define __SPI_H__

#include "DSP28x_Project.h"

//
// SPI-A 主机驱动：硬件 FIFO(16级) 之上叠一层软件
// FIFO(lwrb)，中断后台收发，应用不碰 FIFO 寄存器。 软件片选走
// GPIO57(默认)，事务期间手动拉低，由 spia_cs_low/high 控制。
//
// 关键：SPI 是同步全双工，主机每发 1 个字必收 1
// 个字。收发字数恒等，发送驱动时钟， 接收中断当心跳排空接收 FIFO。详见 spi.c
// 顶部说明。
//

//
// 命名约定：异步操作无后缀（spia_send / spia_recv），入队/取走立即返回，
// 后台中断接力完成；阻塞操作加 _block 后缀（spia_send_block / spia_recv_block），
// 在调用线程内自旋等待完成。SPI 接收必须由主机发送空字产生时钟，
// 因此 spia_recv_block 内部会发送 SPIA_DUMMY 来接收数据。
//

//
// SPICLK 速率
// LSPCLK = SYSCLKOUT/4 = 37.5MHz（由 InitSysCtrl 的 LOSPCP=0x0002 决定）
// SPICLK = LSPCLK / (SPIBRR + 1)，仅当 SPIBRR 在 3..127 有效；SPIBRR<3
// 时硬件固定取 LSPCLK/4 因此 SPICLK 可设范围约 293kHz(BRR=127)
// ~ 9.375MHz(BRR=3)
//
#define SPIA_LSPCLK_HZ 37500000UL
#define SPIA_CLK_HZ 5000000UL
#define SPIA_BRR ((Uint16)(SPIA_LSPCLK_HZ / SPIA_CLK_HZ - 1UL))

//
// 数据帧格式
// SPIA_CHAR_BITS：每帧位数 1..16，写入 SPICCR.SPICHAR 时减 1
// 发送数据在 SPITXBUF 内左对齐，故发送前左移 (16 - 位数)；接收数据在 SPIRXBUF
// 内右对齐，取低位即可
//
#define SPIA_CHAR_BITS 8
#define SPIA_DATA_MASK ((Uint16)(0xFFFFU >> (16 - SPIA_CHAR_BITS)))
#define SPIA_TX_SHIFT (16 - SPIA_CHAR_BITS)
#define SPIA_DUMMY 0x00FF

//
// 时钟极性/相位（CLKPOLARITY 写 SPICCR.6，CLK_PHASE 写 SPICTL.3）。
// 注意 C28x 的 CLK_PHASE 与通用 SPI 的 CPHA 语义相反，按 TI 手册的时钟方案表选：
//   CLKPOL=0,CLKPHASE=0 上升沿无延迟：数据在上升沿当沿翻转
//   CLKPOL=0,CLKPHASE=1 上升沿有延迟：数据提前半周期送出，上升沿时已稳定 ← 本板用
// 从机 74HC164 在 SPICLKA 上升沿锁存 DSA，要求数据在上升沿前稳定，故取
// CLKPOL=0 + CLKPHASE=1。若设 CLKPHASE=0，数据翻转与 164 锁存同沿竞争，
// 高速(SPICLK 越快越明显)下采到中间态 → 段码错乱(乱码)。
//
#define SPIA_CLKPOL 0
#define SPIA_CLKPHASE 1

//
// 软件 FIFO 容量（lwrb 内部另保留 1
// 格区分满/空，驱动已额外分配，这里即实际可用容量）
//
#define SPIA_RX_BUF_SIZE 256
#define SPIA_TX_BUF_SIZE 256

//
// 接收 FIFO 中断触发深度：必须为 1。
// 引擎靠"每收 1 字进中断、排空、补发"维持，>1
// 会让不足一批的尾字永远触发不了中断而滞留， 还会使 spia_recv_block
// 一直等不到最后几个字而卡死。
//
#define SPIA_RX_FIFO_LVL 1

//
// 在途字数上限：等于接收 FIFO 深度 16。补发只在 s_inflight<该值时进行，
// 据此保证接收 FIFO 永不溢出（见 spi.c 不变量说明）。
//
#define SPIA_INFLIGHT_MAX 16

//
// 软件片选：GPIO57，低有效。改引脚需同步改 spia_init 里 GPIO57 的 MUX/DIR
// 配置。
//
#define SPIA_CS_GPIO 57

void   spia_init(void);                           // 配 GPIO、主机模式、FIFO、接收中断；GPIO57 改为软件片选输出
Uint16 spia_send(Uint16 *data, Uint16 len);       // 异步：入队待发，后台中断发出，返回实际入队字数（收回的字进 RX FIFO）
Uint16 spia_recv(Uint16 *data, Uint16 len);       // 异步：从 RX 软件 FIFO 取出已收字，立即返回，不等待
Uint16 spia_send_block(Uint16 *data, Uint16 len); // 阻塞：发 data，等待全部发完并丢弃同步收到的字，返回实际发送数
Uint16 spia_recv_block(Uint16 *data, Uint16 len); // 阻塞：发送空字产生时钟，把收到的数据写入 data，返回实际接收数
Uint16 spia_busy(void);                           // 异步通道是否仍在发/在途（片选拉高前应等其归 0）
void   spia_flush_rx(void);                       // 清空 RX 软件 FIFO（只写器件用来丢弃回读的回声）
void   spia_cs_low(void);                         // 片选拉低
void   spia_cs_high(void);                        // 片选拉高

#endif
