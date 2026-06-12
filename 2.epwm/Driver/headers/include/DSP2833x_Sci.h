// TI File $Revision: /main/2 $
// Checkin $Date: March 1, 2007   15:57:02 $
//###########################################################################
//
// FILE:	DSP2833x_Sci.h
//
// TITLE:	DSP2833x Device SCI Register Definitions.
//
//###########################################################################
// $TI Release: 2833x/2823x Header Files V1.32 $
// $Release Date: June 28, 2010 $
// $Copyright:
// Copyright (C) 2009-2026 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

#ifndef DSP2833x_SCI_H
#define DSP2833x_SCI_H

#ifdef __cplusplus
extern "C" {
#endif

//
// SCI Individual Register Bit Definitions
//

//
// SCICCR communication control register bit definitions
//
struct  SCICCR_BITS {         // bit    description
    Uint16 SCICHAR:3;         // 2:0    Character length control  数据位长度控制
    Uint16 ADDRIDLE_MODE:1;   // 3      ADDR/IDLE Mode control  地址/空闲模式控制，0为空闲模式，1为地址模式
                              //       空闲模式：下一帧前自动插入11位时间的空闲位;地址模式：下一帧数据会在第9位带上地址位标志
    Uint16 LOOPBKENA:1;       // 4      Loop Back enable     自测模式，自己发数据给自己接收
    Uint16 PARITYENA:1;       // 5      Parity enable        奇偶校验使能
    Uint16 PARITY:1;          // 6      Even or Odd Parity   奇偶校验，0奇校验，1偶校验
    Uint16 STOPBITS:1;        // 7      Number of Stop Bits  停止位的位数，0为1位，1为2位
    Uint16 rsvd1:8;           // 15:8   reserved
};  

union SCICCR_REG {
    Uint16              all;
    struct SCICCR_BITS  bit;
};

//
// SCICTL1 control register 1 bit definitions
//                       
struct  SCICTL1_BITS {        // bit    description
    Uint16 RXENA:1;           // 0      SCI receiver enable  接收使能
    Uint16 TXENA:1;           // 1      SCI transmitter enable  发送使能
    Uint16 SLEEP:1;           // 2      SCI sleep   睡眠模式，多机通信控制接收方是否进入睡眠模式，0位脱离睡眠，1位进入睡眠
    Uint16 TXWAKE:1;          // 3      Transmitter wakeup method  发送唤醒，多机通信控制是否唤醒接收方，0位不唤醒，1位唤醒
    Uint16 rsvd:1;            // 4      reserved
    Uint16 SWRESET:1;         // 5      Software reset  软件复位，复位后需要重新配置SCICCR寄存器，配置SCI时先复位再配置。
    Uint16 RXERRINTENA:1;     // 6      Recieve interrupt enable  接收错误中断使能
    Uint16 rsvd1:9;           // 15:7   reserved
}; 

union SCICTL1_REG {
    Uint16               all;
    struct SCICTL1_BITS  bit;
};

//
// SCICTL2 control register 2 bit definitions
//
struct  SCICTL2_BITS {        // bit    description
    Uint16 TXINTENA:1;        // 0      Transmit interrupt enable     发送缓冲区空中断使能
    Uint16 RXBKINTENA:1;      // 1      Receiver-buffer break enable  接收缓冲区满或者接收间断时间过长的中断使能
    Uint16 rsvd:4;            // 5:2    reserved
    Uint16 TXEMPTY:1;         // 6      Transmitter empty flag  发送缓冲区、发送移位寄存器均空标志位，表示发送完毕
    Uint16 TXRDY:1;           // 7      Transmitter ready flag  发送缓冲寄存器空标志位，只要发送缓冲寄存器无数据就置位，表示可写入下一个数据
    Uint16 rsvd1:8;           // 15:8   reserved
}; 

union SCICTL2_REG {
    Uint16               all;
    struct SCICTL2_BITS  bit;
};

//
// SCIRXST Receiver status register bit definitions
//
struct  SCIRXST_BITS {        // bit    description
    Uint16 rsvd:1;            // 0      reserved
    Uint16 RXWAKE:1;          // 1      Receiver wakeup detect flag 接收唤醒检测标志位，表示接收到了唤醒信号,多机通信使用
    Uint16 PE:1;              // 2      Parity error flag   奇偶校验错误标志位，表示接收到的数据奇偶校验错误
    Uint16 OE:1;              // 3      Overrun error flag  溢出错误标志位，接收缓冲区数据未被及时处理，导致数据覆盖
    Uint16 FE:1;              // 4      Framing error flag  帧错误标志位，没有检测到停止位
    Uint16 BRKDT:1;           // 5      Break-detect flag   间断错误标志位，数据传输线保持10位以上的低电平
    Uint16 RXRDY:1;           // 6      Receiver ready flag 接收缓冲寄存器满标志位，表示接收缓冲寄存器已满，需要读取数据
    Uint16 RXERROR:1;         // 7      Receiver error flag 接收错误标志位，表示接收到的数据有错误，包括奇偶校验错误、溢出错误、帧错误、间断错误
}; 

union SCIRXST_REG {
    Uint16               all;
    struct SCIRXST_BITS  bit;
};

//
// SCIRXBUF Receiver Data Buffer with FIFO bit definitions
// 
struct  SCIRXBUF_BITS {       // bits   description
    Uint16 RXDT:8;            // 7:0    Receive word
    Uint16 rsvd:6;            // 13:8   reserved
    // 
    // FIFO模式下，表示该字节数据是否发生了奇偶校验错误或者帧错误
    // 如果未使用FIFO，奇偶校验错误标志位和帧错误标志位在寄存器SCIRXST中
    // 
    Uint16 SCIFFPE:1;         // 14     SCI PE error in FIFO mode  FIFO模式下，奇偶校验错误标志位
    Uint16 SCIFFFE:1;         // 15     SCI FE error in FIFO mode  FIFO模式下，帧错误标志位
}; 

union SCIRXBUF_REG {
    Uint16                all;
    struct SCIRXBUF_BITS  bit;
};

//
// SCIPRI Priority control register bit definitions
//                                                    
struct  SCIPRI_BITS {         // bit    description
    Uint16 rsvd:3;            // 2:0    reserved
    Uint16 FREE:1;            // 3      Free emulation suspend mode 调试暂停时 SCI 继续运行
    Uint16 SOFT:1;            // 4      Soft emulation suspend mode 调试暂停时完成当前传输后停止
    Uint16 rsvd1:3;           // 7:5    reserved
};

union SCIPRI_REG {
    Uint16              all;
    struct SCIPRI_BITS  bit;
};

//
// SCI FIFO Transmit register bit definitions
//                                                 
struct  SCIFFTX_BITS {        // bit    description
    Uint16 TXFFIL:5;          // 4:0    Interrupt level          FIFO发送中断阈值： 发送 FIFO 中的空位数量达到某个条件时触发中断
    Uint16 TXFFIENA:1;        // 5      Interrupt enable         发送FIFO中断使能
    Uint16 TXFFINTCLR:1;      // 6      Clear INT flag           清除发送FIFO中断标志位，写1清除
    Uint16 TXFFINT:1;         // 7      INT flag                 发送FIFO中断标志位，置1表示有中断发生
    Uint16 TXFFST:5;          // 12:8   FIFO status              发送FIFO状态位，表示发送FIFO还有几个空位
    Uint16 TXFIFOXRESET:1;    // 13     FIFO reset               发送 FIFO 复位，写1复位
    Uint16 SCIFFENA:1;        // 14     Enhancement enable       FIFO 模式使能，写1使能
    Uint16 SCIRST:1;          // 15     SCI reset rx/tx channels SCI复位接收/发送通道：初始化时先保持复位，再写 1 释放 SCI 收发模块。
}; 

union SCIFFTX_REG {
    Uint16               all;
    struct SCIFFTX_BITS  bit;
};

//
// SCI FIFO recieve register bit definitions
//                                                
struct  SCIFFRX_BITS {        // bits   description
    Uint16 RXFFIL:5;          // 4:0    Interrupt level          FIFO接收中断阈值： 接收 FIFO 中的数据数量达到某个条件时触发中断
    Uint16 RXFFIENA:1;        // 5      Interrupt enable         FIFO接收中断使能
    Uint16 RXFFINTCLR:1;      // 6      Clear INT flag           清除接收FIFO中断标志位，写1清除
    Uint16 RXFFINT:1;         // 7      INT flag                 接收FIFO中断标志位，置1表示有中断发生
    Uint16 RXFFST:5;          // 12:8   FIFO status              接收FIFO状态位，表示接收FIFO还有几个数据
    Uint16 RXFIFORESET:1;     // 13     FIFO reset               接收 FIFO 复位，写1复位
    Uint16 RXFFOVRCLR:1;      // 14     Clear overflow           清除溢出标志位，写1清除
    Uint16 RXFFOVF:1;         // 15     FIFO overflow            溢出标志位，表示接收 FIFO 数据溢出
}; 

union SCIFFRX_REG {
    Uint16               all;
    struct SCIFFRX_BITS  bit;
};

//
// SCI FIFO control register bit definitions
//
struct  SCIFFCT_BITS {        // bits   description
    Uint16 FFTXDLY:8;         // 7:0    FIFO transmit delay   发送完一个字节后等几个时钟再发下一个
    Uint16 rsvd:5;            // 12:8   reserved
    Uint16 CDC:1;             // 13     Auto baud mode enable 自动波特率模式使能，写1使能
    Uint16 ABDCLR:1;          // 14     Auto baud clear       自动波特率清除，写1清除
    Uint16 ABD:1;             // 15     Auto baud detect      自动波特率检测，写1检测
};
    
union SCIFFCT_REG {
    Uint16               all;
    struct SCIFFCT_BITS  bit;
};

//
// SCI Register File
//
struct  SCI_REGS {
    union SCICCR_REG     SCICCR;     // Communications control register   通信控制寄存器
    union SCICTL1_REG    SCICTL1;    // Control register 1                SCI控制寄存器1
    Uint16               SCIHBAUD;   // Baud rate (high) register         波特率高寄存器
    Uint16               SCILBAUD;   // Baud rate (low) register          波特率低寄存器
    union SCICTL2_REG    SCICTL2;    // Control register 2                SCI控制寄存器2，也有状态寄存器功能

    union SCIRXST_REG    SCIRXST;    // Recieve status register           SCI接收状态寄存器

    //
    // SCIRXEMU 与 SCIRXBUF：地址独立，但共用同一块物理存储，数值完全相同。
    // 接收数据由 RXSHF(接收移位寄存器) 同时送入两者，到位后 RXRDY 置位表示可读。
    // 唯一区别在“读”的副作用：
    //   读 SCIRXBUF -> 清除 RXRDY(取走数据的正常路径)
    //   读 SCIRXEMU -> 不清除 RXRDY(仿真/调试用，可反复观察数据而不打乱接收流程)
    // 系统复位时 SCIRXEMU 被清空。
    //
    Uint16               SCIRXEMU;   // Recieve emulation buffer register SCI接收仿真缓冲区
    union SCIRXBUF_REG   SCIRXBUF;   // Recieve data buffer               SCI接收数据缓冲区寄存器

    Uint16               rsvd1;      // reserved
    Uint16               SCITXBUF;   // Transmit data buffer              SCI发送数据缓冲区
    union SCIFFTX_REG    SCIFFTX;    // FIFO transmit register            FIFO发送寄存器
    union SCIFFRX_REG    SCIFFRX;    // FIFO recieve register             FIFO接收寄存器
    union SCIFFCT_REG    SCIFFCT;    // FIFO control register             FIFO控制寄存器
    Uint16               rsvd2;      // reserved
    Uint16               rsvd3;      // reserved
    union SCIPRI_REG     SCIPRI;     // FIFO Priority control             FIFO优先级控制寄存器
};

//
// SCI External References & Function Declarations
//
extern volatile struct SCI_REGS SciaRegs;
extern volatile struct SCI_REGS ScibRegs;
extern volatile struct SCI_REGS ScicRegs;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2833x_SCI_H definition

//
// End of file
//

