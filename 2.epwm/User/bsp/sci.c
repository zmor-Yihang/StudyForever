#include "sci.h"
#include "../tools/lwrb/lwrb.h"

//
// ── 双层缓冲结构 ──
// 硬件 FIFO(16级) 之上再叠一层软件 FIFO(lwrb)，应用只碰软件 FIFO，不直接读写 FIFO 寄存器。
//
//   发送: scia_send() →[TX lwrb]→ TX中断 →[发送FIFO]→ TX引脚
//   接收: RX引脚 →[接收FIFO]→ RX中断 →[RX lwrb]→ scia_recv()
//
// lwrb 的读写指针会被生产者和消费者共同更新。凡是主程序侧访问软件 FIFO，都会临时
// 关闭对应的 SCI FIFO 中断，避免主程序和中断同时改同一个软件 FIFO 状态。
//
// C28x平台特性:最小可寻址单元为 16 位,Uint16 实为 16 位。SCI API 仍按 Uint16/格
// 传递数据，每格低 8 位有效；内部 lwrb 每槽位使用 16 位存储，低 8 位有效。
//

static lwrb_t s_rx_rb;
static lwrb_t s_tx_rb;
static lwrb_data_t s_rx_rb_data[SCIA_RX_BUF_SIZE + 1];
static lwrb_data_t s_tx_rb_data[SCIA_TX_BUF_SIZE + 1];

static volatile Uint16 s_rx_overflow = 0; // RX 软件 FIFO 满丢弃计数，调试用

//
// 接收中断
// 接收 FIFO 攒够 SCIA_RX_FIFO_LVL 个字节 → 进中断，全部搬进 RX 软件 FIFO
// 必须把 FIFO 读空，否则残留字节维持中断条件造成反复触发。
//
static interrupt void sciaRxIsr(void)
{
    Uint16 n = SciaRegs.SCIFFRX.bit.RXFFST; // FIFO 内现有字节数

    while (n--)
    {
        // FIFO 模式下 SCIRXBUF 高位含每字节错误标志,屏蔽后取数据字节
        lwrb_data_t b = (lwrb_data_t)(SciaRegs.SCIRXBUF.all & 0x00FF);

        if (lwrb_write(&s_rx_rb, &b, 1U) == 0U) // 软件 FIFO 满,写不进则丢弃并计数
        {
            s_rx_overflow++;
        }
    }

    SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;   // 清 FIFO 溢出标志
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;   // 清接收中断标志(不清会反复进中断)
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; // 应答 PIE 组9
}

//
// ── 发送中断 ──
// 发送 FIFO 剩余 <= SCIA_TX_FIFO_LVL 个 → 进中断 → 从 TX 软件 FIFO 补数据进 FIFO。
// 关键:TX 软件 FIFO 搬空后必须自己关掉 TXFFIENA，否则 FIFO 长期空会持续触发中断空转。
//
static interrupt void sciaTxIsr(void)
{
    lwrb_data_t b;

    // FIFO 还有空位(<16)且 TX 软件 FIFO 还有数据,就继续填
    while (SciaRegs.SCIFFTX.bit.TXFFST < 16 && lwrb_read(&s_tx_rb, &b, 1U) == 1U)
    {
        SciaRegs.SCITXBUF = (Uint16)b;
    }

    if (lwrb_get_full(&s_tx_rb) == 0U) // TX 软件 FIFO 已空 → 关发送中断,等下次 scia_send 再开
    {
        SciaRegs.SCIFFTX.bit.TXFFIENA = 0;
    }

    SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;   // 清发送中断标志
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; // 应答 PIE 组9
}

//
// ── 初始化:8-N-1 / 9600 / 收发中断 + FIFO ──
//
void scia_init(void)
{
    //
    // 先复位软件 FIFO,务必在使能中断之前:否则 SWRESET 后 RX 中断可能先到,
    // 操作未准备好的软件缓冲。
    //
    lwrb_init(&s_rx_rb, s_rx_rb_data, SCIA_RX_BUF_SIZE + 1);
    lwrb_init(&s_tx_rb, s_tx_rb_data, SCIA_TX_BUF_SIZE + 1);

    InitSciaGpio(); // GPIO28=SCIRXDA, GPIO29=SCITXDA

    //
    // 通信格式:8 位数据 / 无校验 / 1 位停止位 / 空闲线模式(8-N-1)
    //
    SciaRegs.SCICCR.all = 0x0007;

    //
    // 收发使能,脱离睡眠,不用接收错误中断
    //
    SciaRegs.SCICTL1.bit.RXENA = 1;
    SciaRegs.SCICTL1.bit.TXENA = 1;
    SciaRegs.SCICTL1.bit.SLEEP = 0;
    SciaRegs.SCICTL1.bit.RXERRINTENA = 0;

    //
    // 波特率:BRR 拆成高/低 8 位
    //
    SciaRegs.SCIHBAUD = (SCIA_BRR >> 8) & 0x00FF;
    SciaRegs.SCILBAUD = SCIA_BRR & 0x00FF;

    //
    // SCICTL2 的收发中断关闭，中断触发源由 SCIFFTX/SCIFFRX 控制
    //
    SciaRegs.SCICTL2.bit.TXINTENA = 0;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 0;

    //
    // 启用 FIFO 、收发通道工作、复位发送 FIFO 指针
    // TXFFIENA 此刻不开,等 scia_send 有数据才开
    //
    SciaRegs.SCIFFTX.bit.SCIFFENA = 1;            // FIFO 模式总开关
    SciaRegs.SCIFFTX.bit.SCIRST = 1;              // 收发通道正常工作
    SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;        // 发送 FIFO 复位(清空指针)
    SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;        // 释放,开始工作
    SciaRegs.SCIFFTX.bit.TXFFIL = SCIA_TX_FIFO_LVL;
    SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;
    SciaRegs.SCIFFTX.bit.TXFFIENA = 0;            // 发送中断暂不开

    //
    //
    // 接收 FIFO:复位指针、设触发深度、开接收 FIFO 中断
    // 关键:RXFFINTCLR 必须紧靠 RXFFIENA 之前!若两者之间有时间窗口且恰好
    // 收到数据,RXFFINT 会被硬件置位但中断尚未使能,之后不会再触发,导致
    // 接收永久失效。
    //
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;         // 接收 FIFO 复位(清空指针)
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;         // 释放,开始工作
    SciaRegs.SCIFFRX.bit.RXFFIL = SCIA_RX_FIFO_LVL;
    SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;          // 紧靠使能前清除,最大限度缩小窗口
    SciaRegs.SCIFFRX.bit.RXFFIENA = 1;            // 接收中断常开

    //
    // FIFO 字节间无额外延时
    //
    SciaRegs.SCIFFCT.bit.FFTXDLY = 0;

    //
    // 挂 PIE 向量，这里和别的外设中断不太一样，像epwm中断是内部提供注册回调，外部写中断函数，
    // 中断函数也是有外部业务逻辑决定，因此挂向量表是在外部实现，内部只提供回调。
    // 而sci中断，中断函数是内部业务逻辑决定，因此挂向量表是在内部实现，中断函数也在内部实现。
    // 全局中断(EINT)已在 main 的 EnableInterrupts() 开启
    //
    EALLOW;
    PieVectTable.SCIRXINTA = &sciaRxIsr;
    PieVectTable.SCITXINTA = &sciaTxIsr;
    EDIS;

    PieCtrlRegs.PIEIER9.bit.INTx1 = 1; // SCIRXINTA (9.1)
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1; // SCITXINTA (9.2)
    IER |= M_INT9;

    SciaRegs.SCICTL1.bit.SWRESET = 1; // 解除软件复位,SCI 开始运行
}

//
// 入队待发:把数据塞进 TX 软件 FIFO,然后开发送中断让 ISR 去搬。返回实际入队字节数
//
Uint16 scia_send(const Uint16 *data, Uint16 len)
{
    Uint16 i;
    Uint16 sent = 0;
    Uint16 tx_int_enabled = SciaRegs.SCIFFTX.bit.TXFFIENA;

    SciaRegs.SCIFFTX.bit.TXFFIENA = 0;

    //
    // scia_send 接口按 Uint16/格 传入(每格低 8 位有效),软件 FIFO 按字节保存。
    //
    for (i = 0; i < len; i++)
    {
        lwrb_data_t b = (lwrb_data_t)(data[i] & 0x00FF);
        if (lwrb_write(&s_tx_rb, &b, 1U) == 0U) // 软件 FIFO 满,后面发不进去
        {
            break;
        }
        sent++;
    }

    if (sent > 0 || tx_int_enabled != 0) // 有新数据或之前正在发送,都要保持发送中断可用
    {
        SciaRegs.SCIFFTX.bit.TXFFIENA = 1;
    }

    return sent;
}

//
// 发送字符串(到 '\0' 为止)
//
Uint16 scia_send_str(const char *s)
{
    Uint16 sent = 0;
    Uint16 tx_int_enabled = SciaRegs.SCIFFTX.bit.TXFFIENA;

    SciaRegs.SCIFFTX.bit.TXFFIENA = 0;

    while (s[0] != '\0')
    {
        lwrb_data_t b = (lwrb_data_t)((Uint16)s[0] & 0x00FF);
        if (lwrb_write(&s_tx_rb, &b, 1U) == 0U)
        {
            break;
        }
        sent++;
        s++;
    }

    if (sent > 0 || tx_int_enabled != 0)
    {
        SciaRegs.SCIFFTX.bit.TXFFIENA = 1;
    }

    return sent;
}

//
// 取出已收数据:从 RX 软件 FIFO 搬到用户缓冲。返回实际取出字节数
//
Uint16 scia_recv(Uint16 *data, Uint16 len)
{
    Uint16 i;
    Uint16 rx_int_enabled = SciaRegs.SCIFFRX.bit.RXFFIENA;

    SciaRegs.SCIFFRX.bit.RXFFIENA = 0;

    for (i = 0; i < len; i++)
    {
        lwrb_data_t b;
        if (lwrb_read(&s_rx_rb, &b, 1U) != 1U) // 软件 FIFO 空,没数据了
        {
            break;
        }
        data[i] = (Uint16)b;
    }

    SciaRegs.SCIFFRX.bit.RXFFIENA = rx_int_enabled;

    return i;
}

//
// 接收缓冲区当前可读字节数
//
Uint16 scia_rx_count(void)
{
    Uint16 count;
    Uint16 rx_int_enabled = SciaRegs.SCIFFRX.bit.RXFFIENA;

    SciaRegs.SCIFFRX.bit.RXFFIENA = 0;
    count = (Uint16)lwrb_get_full(&s_rx_rb);
    SciaRegs.SCIFFRX.bit.RXFFIENA = rx_int_enabled;

    return count;
}
