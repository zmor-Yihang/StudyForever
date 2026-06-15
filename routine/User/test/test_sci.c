#include "test/test_sci.h"
#include "DSP28x_Project.h"
#include "bsp/sci.h"

//
// SCI-A 测试
//
// 上电后直接进入串口回显，发送任意数据到 SCIA 即原样返回。
//

void test_sci_init(void)
{
    scia_init();
    scia_send_str("\r\nSCIA echo test ready.\r\n");
    scia_send_str("Send data to SCIA, it will echo back.\r\n");
}

void test_sci_loop(void)
{
    Uint16 buf[32];
    Uint16 n = scia_recv(buf, 32);

    if (n > 0)
    {
        scia_send(buf, n);
    }
}
