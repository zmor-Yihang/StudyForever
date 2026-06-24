#include "spi.h"
#include "../tools/lwrb.h"

//
// 双层缓冲 + 全双工引擎
// 硬件 FIFO(16级) 之上再叠一层软件 FIFO(lwrb)，应用只碰软件 FIFO，不直接读写 FIFO 寄存器。
//
//   发送: spia_send() →[TX lwrb]→ 补发 →[发送FIFO]→ SPICLK + SIMO 引脚
//   接收: SOMI 引脚 →[接收FIFO]→ RX中断 →[RX lwrb]→ spia_recv()
//
// SPI 与 SCI 的根本差异：SPI 同步全双工，主机每往发送 FIFO 塞 1 个字，就产生一串 SPICLK 并从
// SOMI 移入 1 个字。所以"发出字数 ≡ 收回字数"，发送驱动时钟、接收被动伴随，收发不可分离。
//
// 命名约定：异步无后缀（spia_send / spia_recv），入队/取走立即返回，中断后台接力；
//          阻塞加 _block 后缀（spia_send_block / spia_recv_block），调用线程自旋等完成。
//          SPI 接收必须由主机发送空字产生时钟，spia_recv_block 内部发送 SPIA_DUMMY。
//
// 保命不变量：s_inflight ≤ 接收FIFO深度(16)
//   s_inflight = 已塞进发送FIFO但还没从接收FIFO收回的字数(覆盖 发送FIFO+移位寄存器+接收FIFO)。
//   补发(spia_pump)只在 s_inflight<16 时进行，据此保证接收 FIFO 永不溢出——这是全双工下最易踩的坑。
//
// 节拍：接收 FIFO 触发深度=1，每收 1 字进中断 → 排空接收FIFO搬进 RX lwrb → 顺手补发。
//   只用接收中断当心跳(全双工保证收发 1:1，发送中断是多余的)。
//
// 点火：空闲态没有中断流，靠应用侧亲自补发一次来启动；补发期间屏蔽 PIE6.1 独占。
//   软件 FIFO 单生产者单消费者，天然 lock-free；s_inflight 只在补发与中断里改，二者互斥不并发。
//
// C28x 最小可寻址单元 16 位。SPI API 按 Uint16/格 传递，每格低 SPIA_CHAR_BITS 位有效。
//

static lwrb_t      s_rx_rb;
static lwrb_t      s_tx_rb;
static lwrb_data_t s_rx_rb_data[SPIA_RX_BUF_SIZE + 1];
static lwrb_data_t s_tx_rb_data[SPIA_TX_BUF_SIZE + 1];

static volatile Uint16 s_inflight = 0;    // 在途字数：塞进发送FIFO未收回，守接收FIFO不溢出
static volatile Uint16 s_rx_overflow = 0; // RX 软件FIFO 满丢弃计数，调试用

//
// 屏蔽/恢复 SPI-A 接收中断(PIE 6.1)。应用侧补发时用它独占，
// 避免与接收中断里的补发并发改 s_inflight。PIEIER6 只有应用改，无 RMW 竞争。
//
static void spia_lock(void) {
    PieCtrlRegs.PIEIER6.bit.INTx1 = 0;
}
static void spia_unlock(void) {
    PieCtrlRegs.PIEIER6.bit.INTx1 = 1;
}

//
// 补发引擎：把 TX 软件 FIFO 的字搬进硬件发送 FIFO，每搬 1 字 s_inflight++。
// 两道闸门缺一不可：
//   s_inflight < SPIA_INFLIGHT_MAX —— 不让在途字数超过接收 FIFO 容量，杜绝接收溢出
//   TXFFST < 16                    —— 发送 FIFO 还有空位
// 短路求值保证闸门未过时不会从 lwrb 取走数据(取了又塞不下就丢了)。
// 调用者负责互斥：接收中断里调，或应用 spia_lock 后调，二者不会重入。
//
static void spia_pump(void) {
    lwrb_data_t b;

    while (s_inflight < SPIA_INFLIGHT_MAX && SpiaRegs.SPIFFTX.bit.TXFFST < 16 && lwrb_read(&s_tx_rb, &b, 1U) == 1U) {
        SpiaRegs.SPITXBUF = (Uint16)((b & SPIA_DATA_MASK) << SPIA_TX_SHIFT); // 左对齐写入
        s_inflight++;
    }
}

//
// 接收中断(心跳)
// 每收满 SPIA_RX_FIFO_LVL(=1) 个字进来：先把接收 FIFO 全部搬进 RX 软件 FIFO(每字 s_inflight--)，
// 再补发让管子继续流动。必须把接收 FIFO 读空，否则残留维持中断条件造成反复触发。
//
static interrupt void spiaRxIsr(void) {
    Uint16 n = SpiaRegs.SPIFFRX.bit.RXFFST; // 接收 FIFO 内现有字数

    while (n--) {
        lwrb_data_t b = (lwrb_data_t)(SpiaRegs.SPIRXBUF & SPIA_DATA_MASK); // 右对齐，取有效位

        if (lwrb_write(&s_rx_rb, &b, 1U) == 0U) // RX 软件FIFO 满则丢弃并计数
        {
            s_rx_overflow++;
        }
        if (s_inflight > 0) {
            s_inflight--;
        }
    }

    spia_pump(); // 在途腾出空位，继续补发后续待发字

    SpiaRegs.SPIFFRX.bit.RXFFOVFCLR = 1;    // 清接收 FIFO 溢出标志
    SpiaRegs.SPIFFRX.bit.RXFFINTCLR = 1;    // 清接收中断标志(不清会反复进中断)
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6; // 应答 PIE 组6
}

//
// ── 初始化:主机 / 8位 / SPICLK=1MHz / 接收中断 + FIFO / 软件片选 ──
//
void spia_init(void) {
    //
    // 先复位软件 FIFO,务必在使能中断之前:否则接收中断可能先到,操作未准备好的软件缓冲。
    //
    lwrb_init(&s_rx_rb, s_rx_rb_data, SPIA_RX_BUF_SIZE + 1);
    lwrb_init(&s_tx_rb, s_tx_rb_data, SPIA_TX_BUF_SIZE + 1);

    InitSpiaGpio(); // GPIO54=SIMO,55=SOMI,56=CLK,57=STEA

    //
    // GPIO57 从 SPISTEA 收回作普通输出做软件片选(低有效)。改 SPIA_CS_GPIO 须同步改这两行。
    //
    EALLOW;
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;
    EDIS;
    spia_cs_high(); // 初始释放从机

    SpiaRegs.SPICCR.bit.SPISWRESET = 0; // 配置前复位 SPI，配置完成后再置位

    //
    // 帧格式：字长、时钟空闲极性、不环回
    //
    SpiaRegs.SPICCR.bit.SPICHAR = SPIA_CHAR_BITS - 1;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = SPIA_CLKPOL;
    SpiaRegs.SPICCR.bit.SPILBK = 0;

    //
    // 主机模式：本机产生 SPICLK；TALK 允许 SIMO 输出；时钟相位；
    // 收发中断走 FIFO，故 SPIINTENA/OVERRUNINTENA(基本中断)关闭
    //
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.CLK_PHASE = SPIA_CLKPHASE;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;
    SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;

    SpiaRegs.SPIBRR = SPIA_BRR; // SPICLK = LSPCLK/(BRR+1)

    //
    // 仿真挂起时完成当前字符后停止，避免断点处给从机半截帧
    //
    SpiaRegs.SPIPRI.bit.FREE = 0;
    SpiaRegs.SPIPRI.bit.SOFT = 1;

    //
    // 启用 FIFO，复位收发 FIFO 指针。
    // 不开发送中断(TXFFIENA=0)：全双工下只用接收中断当心跳。
    //
    SpiaRegs.SPIFFTX.bit.SPIRST = 0;     // 配置前复位 FIFO 增强模块
    SpiaRegs.SPIFFTX.bit.SPIFFENA = 1;   // FIFO 模式总开关
    SpiaRegs.SPIFFTX.bit.TXFIFO = 0;     // 复位发送 FIFO 指针
    SpiaRegs.SPIFFTX.bit.TXFFINTCLR = 1; // 清发送中断标志
    SpiaRegs.SPIFFTX.bit.TXFFIENA = 0;   // 发送中断不用
    SpiaRegs.SPIFFTX.bit.TXFIFO = 1;     // 释放发送 FIFO 指针

    //
    // 接收 FIFO：触发深度=1(每收 1 字进中断当心跳)。
    // 关键:RXFFINTCLR 必须紧靠 RXFFIENA 之前!否则两者间若恰好收到数据,RXFFINT 被置位
    // 但中断尚未使能,之后不会再触发,接收永久失效。
    //
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;           // 复位接收 FIFO 指针
    SpiaRegs.SPIFFRX.bit.RXFFIL = SPIA_RX_FIFO_LVL; // 触发深度=1
    SpiaRegs.SPIFFRX.bit.RXFFOVFCLR = 1;            // 清溢出标志
    SpiaRegs.SPIFFRX.bit.RXFFINTCLR = 1;            // 清接收中断标志
    SpiaRegs.SPIFFRX.bit.RXFFIENA = 1;              // 接收中断常开(心跳)
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;           // 释放接收 FIFO 指针

    SpiaRegs.SPIFFCT.bit.TXDLY = 0; // 字间无额外延迟

    SpiaRegs.SPIFFTX.bit.SPIRST = 1; // 配置完成,释放 FIFO 增强模块

    //
    // 挂 PIE 向量。和 SCI 一样:中断函数是内部业务逻辑,故向量与函数都在内部实现。
    // 全局中断(EINT)已在 main 的 EnableInterrupts() 开启。
    //
    EALLOW;
    PieVectTable.SPIRXINTA = &spiaRxIsr;
    EDIS;

    PieCtrlRegs.PIEIER6.bit.INTx1 = 1; // SPIRXINTA (6.1)
    IER |= M_INT6;

    SpiaRegs.SPICCR.bit.SPISWRESET = 1; // 解除软件复位,SPI 开始运行
}

//
// 异步入队：把字塞进 TX 软件 FIFO，再亲自补发一次点火，后台中断接力发完。返回实际入队字数。
// 收回的字会进 RX 软件 FIFO(全双工)，若不关心可用 spia_flush_rx 丢弃。
// 写 lwrb 是生产者侧，与中断(消费者侧补发)是单生产单消费，写入阶段无需上锁。
//
Uint16 spia_send(Uint16 *data, Uint16 len) {
    Uint16 i;
    Uint16 sent = 0;

    for (i = 0; i < len; i++) {
        lwrb_data_t b = (data[i] & SPIA_DATA_MASK);
        if (lwrb_write(&s_tx_rb, &b, 1U) == 0U) // TX 软件FIFO 满则停
        {
            break;
        }
        sent++;
    }

    spia_lock(); // 独占补发，防与中断里的补发并发改 s_inflight
    spia_pump(); // 点火：空闲态此处启动时钟流；忙态则等中断接力
    spia_unlock();

    return sent;
}

//
// 异步取已收：从 RX 软件 FIFO 搬到用户缓冲，立即返回。返回实际取出数。
// 读 lwrb 是消费者侧，与中断(生产者侧)单生产单消费，无需上锁。
//
Uint16 spia_recv(Uint16 *data, Uint16 len) {
    Uint16 i;

    for (i = 0; i < len; i++) {
        lwrb_data_t b;
        if (lwrb_read(&s_rx_rb, &b, 1U) != 1U) // RX 软件FIFO 空
        {
            break;
        }
        data[i] = (Uint16)b;
    }

    return i;
}

//
// 阻塞底层流程：逐段入队并就地收取，直至完成 len 个字。
// tx==0 时发送空字产生时钟，rx==0 时丢弃收到的字。返回实际完成数。
// 分段是因为软件 FIFO 容量有限：发满一批→等中断收回腾位→再发下一批。
// 看门狗：长时间无任何进展(从机失联/未接)则退出，避免硬等死循环。
//
static Uint16 spia_block_io(Uint16 *tx, Uint16 *rx, Uint16 len) {
    const Uint32 STALL_LIMIT = 2000000UL; // 无进展容忍上限，远大于正常单字耗时
    Uint16       enq = 0;                 // 已入队待发数
    Uint16       got = 0;                 // 已收回数
    Uint16       last = 0;                // 上轮已收回数，用于判进展
    Uint32       stall = 0;

    while (got < len) {
        //
        // 尽量把还没入队的字塞进 TX 软件 FIFO(留意它也可能被前面 spia_send 占用)
        //
        while (enq < len && lwrb_get_free(&s_tx_rb) > 0U) {
            Uint16      w = (tx != 0) ? (Uint16)(tx[enq] & SPIA_DATA_MASK) : (Uint16)SPIA_DUMMY;
            lwrb_data_t b = (lwrb_data_t)w;
            if (lwrb_write(&s_tx_rb, &b, 1U) == 0U) {
                break;
            }
            enq++;
        }

        spia_lock();
        spia_pump(); // 点火/续流
        spia_unlock();

        //
        // 就地收取(消费者侧，无需上锁)
        //
        {
            lwrb_data_t b;
            while (got < len && lwrb_read(&s_rx_rb, &b, 1U) == 1U) {
                if (rx != 0) {
                    rx[got] = (Uint16)b;
                }
                got++;
            }
        }

        if (got != last) // 有进展则重置看门狗
        {
            last = got;
            stall = 0;
        } else if (++stall >= STALL_LIMIT) {
            break; // 疑似从机失联，放弃
        }
    }

    return got;
}

//
// 阻塞发送：发 data，等待全部发完，并丢弃同步收到的字。
//
Uint16 spia_send_block(Uint16 *data, Uint16 len) {
    return spia_block_io(data, 0, len);
}

//
// 阻塞接收：发送空字产生时钟，把收到的数据写入 data。
//
Uint16 spia_recv_block(Uint16 *data, Uint16 len) {
    return spia_block_io(0, data, len);
}

//
// 异步通道是否仍有未发完或在途数据。软件片选场景：事务尾部应等它归 0 再 spia_cs_high，
// 否则会在末几位还没移完时就撤片选。
//   s_inflight>0           —— 发送FIFO/移位寄存器/接收FIFO 里还有字没收回
//   tx 软件FIFO 非空        —— 还有待补发的字
// (FIFO 模式下 SPISTS.BUFFULL_FLAG 已被 TXFFST 取代，恒 0，故不查)
//
Uint16 spia_busy(void) {
    if (s_inflight > 0) {
        return 1;
    }
    return (lwrb_get_full(&s_tx_rb) > 0U) ? 1 : 0;
}

//
// 清空 RX 软件 FIFO：只写器件丢弃回读的"回声"。复位期间屏蔽中断(生产者)。
//
void spia_flush_rx(void) {
    spia_lock();
    lwrb_reset(&s_rx_rb);
    spia_unlock();
}

//
// 软件片选(低有效)。写 SET/CLEAR 寄存器只动目标位，不影响其他 GPIO。
//
void spia_cs_low(void) {
    GpioDataRegs.GPBCLEAR.all = ((Uint32)1 << (SPIA_CS_GPIO - 32));
}

void spia_cs_high(void) {
    GpioDataRegs.GPBSET.all = ((Uint32)1 << (SPIA_CS_GPIO - 32));
}
