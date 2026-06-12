#include "test/test_sci.h"
#include "DSP28x_Project.h"
#include "bsp/sci.h"
#include "bsp/led.h"

//
// SCI-A 测试
//
// 上电先做一次「内部数字回环自测」，再进入串口回显。
//
// 自测原理：打开 SCI 的 LOOPBKENA，把 TX 在芯片内部直接接到 RX，完全不经过外部
// 引脚和接线。用真实驱动 scia_send / scia_recv 发一段已知数据再读回比对，从而把
// 「驱动本身的问题」和「外部接线/波特率/串口工具的问题」彻底分开。
//
// LED_D8 指示自测结果：
//   常亮 → 驱动收发链路(时钟/中断/FIFO/寄存器)正常。若外部串口仍收不到，
//          问题在接线(TX/RX 是否接反、GND、电平)、波特率或串口工具/COM 口。
//   快闪 → 驱动本身异常，与外部接线无关，需要继续查驱动。
//
#define TEST_SCI_RECV_BUF_SIZE 32

static Uint16 test_sci_loopback_ok(void)
{
    Uint16 i;
    Uint16 tx[8];
    Uint16 rx[8];
    Uint16 got;

    for (i = 0; i < 8; i++)
    {
        tx[i] = (Uint16)(0x41 + i); // 'A'..'H'
    }

    // 打开内部数字回环：改 SCICCR 需在软件复位期间，改完再释放
    SciaRegs.SCICTL1.bit.SWRESET = 0;
    SciaRegs.SCICCR.bit.LOOPBKENA = 1;
    SciaRegs.SCICTL1.bit.SWRESET = 1;

    scia_send(tx, 8);
    DELAY_MS(50); // 远大于 8 字节@9600 的传输时间(约 8.3ms)
    got = scia_recv(rx, 8);

    // 关闭内部回环，恢复正常收发
    SciaRegs.SCICTL1.bit.SWRESET = 0;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0;
    SciaRegs.SCICTL1.bit.SWRESET = 1;

    if (got != 8)
    {
        return 0;
    }
    for (i = 0; i < 8; i++)
    {
        if (rx[i] != tx[i])
        {
            return 0;
        }
    }
    return 1;
}

void test_sci_init(void)
{
    led_init(LED_D8);

    scia_init();

    if (test_sci_loopback_ok())
    {
        led_on(LED_D8); // 驱动 OK：再收不到就是外部接线/串口工具问题
    }
    else
    {
        for (;;) // 驱动异常：快闪报警
        {
            led_toggle(LED_D8);
            DELAY_MS(100);
        }
    }

    scia_send_str("\r\nSCIA echo test ready.\r\n");
    scia_send_str("Send data to SCIA, it will echo back.\r\n");
}

void test_sci_loop(void)
{
    Uint16 buf[TEST_SCI_RECV_BUF_SIZE];
    Uint16 n = scia_recv(buf, TEST_SCI_RECV_BUF_SIZE);

    if (n > 0)
    {
        scia_send(buf, n);
    }
}