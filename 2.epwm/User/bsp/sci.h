#ifndef __SCI_H__
#define __SCI_H__

#include "DSP28x_Project.h"

//
// ── SCI-A 通信参数 ──
// LSPCLK = SYSCLKOUT / 4 = 150MHz / 4 = 37.5MHz（由 InitSysCtrl 的 LOSPCP=0x0002 决定）
// 波特率分频值 BRR = LSPCLK / (baud * 8) - 1，拆成高/低 8 位写入 SCIHBAUD/SCILBAUD
//
#define SCIA_LSPCLK_HZ 37500000UL
#define SCIA_BAUD      9600UL
#define SCIA_BRR       ((Uint16)(SCIA_LSPCLK_HZ / (SCIA_BAUD * 8UL) - 1UL))

//
// ── 软件 FIFO 大小 ──
// lwrb 内部保留 1 格区分满/空；驱动会额外分配 1 格，因此这里配置的是实际可用容量。
//
#define SCIA_RX_BUF_SIZE 256
#define SCIA_TX_BUF_SIZE 256

//
// ── FIFO 触发深度 ──
// RX：FIFO 里攒到 >= 该值个字节触发接收中断。设 1 = 每字节触发，避免尾字节滞留
// TX：FIFO 里剩 <= 该值个字节触发发送中断。设 0 = 发空才补，约每 16 字节一次中断
//
#define SCIA_RX_FIFO_LVL 1
#define SCIA_TX_FIFO_LVL 0

//
// ── API ──
// 全部非阻塞，基于软件 FIFO + 中断收发
//
void scia_init(void);

// 入队待发，返回实际入队字节数（< len 表示发送缓冲已满）
Uint16 scia_send(Uint16 *data, Uint16 len);

// 发送以 '\0' 结尾的字符串，返回实际入队字节数
Uint16 scia_send_str(char *s);

// 取出已收数据，返回实际取出字节数（< len 表示接收缓冲不够）
Uint16 scia_recv(Uint16 *data, Uint16 len);

#endif
