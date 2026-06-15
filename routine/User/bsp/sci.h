#ifndef __SCI_H__
#define __SCI_H__

#include "DSP28x_Project.h"

//
// LSPCLK = SYSCLKOUT / 4 = 150MHz / 4 = 37.5MHz（由 InitSysCtrl 的 LOSPCP=0x0002 决定）
// 波特率分频值 BRR = LSPCLK / (baud * 8) - 1，拆成高/低 8 位写入 SCIHBAUD/SCILBAUD
//
#define SCIA_LSPCLK_HZ 37500000UL
#define SCIA_BAUD      9600UL
#define SCIA_BRR       ((Uint16)(SCIA_LSPCLK_HZ / (SCIA_BAUD * 8UL) - 1UL))

//
// lwrb FIFO 配置
// lwrb 内部保留 1 格区分满/空；驱动会额外分配 1 格，因此这里配置的是实际可用容量。
//
#define SCIA_RX_BUF_SIZE 256
#define SCIA_TX_BUF_SIZE 256

//
// 硬件 FIFO 中断触发深度
// RX：FIFO 里攒到 >= 该值个字节触发接收中断。设 1 = 每字节触发，避免尾字节滞留
// TX：FIFO 里剩 <= 该值个字节触发发送中断。设 0 = 发空才补，约每 16 字节一次中断
//
#define SCIA_RX_FIFO_LVL 1
#define SCIA_TX_FIFO_LVL 0

void scia_init(void);
Uint16 scia_send(Uint16 *data, Uint16 len);
Uint16 scia_send_str(char *s);
Uint16 scia_recv(Uint16 *data, Uint16 len);

#endif
