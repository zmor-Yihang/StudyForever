// TI File $Revision: /main/3 $
// Checkin $Date: April 17, 2008   11:08:27 $
//###########################################################################
//
// FILE:   DSP2833x_Spi.h
//
// TITLE:  DSP2833x Device SPI Register Definitions.
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

#ifndef DSP2833x_SPI_H
#define DSP2833x_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

//
// SPI Individual Register Bit Definitions
//

//
// SPI FIFO Transmit register bit definitions
//
struct  SPIFFTX_BITS {        // bit    description
    Uint16 TXFFIL:5;          // 4:0    Interrupt level 发送FIFO中断触发水位，还有多少数据没有发送时触发中断
    Uint16 TXFFIENA:1;        // 5      Interrupt enable 发送FIFO中断使能，0表示禁用，1表示使能
    Uint16 TXFFINTCLR:1;      // 6      Clear INT flag 清除发送FIFO中断标志位，初始化前将该位置1，初始化后将该位置0
    Uint16 TXFFINT:1;         // 7      INT flag 发送FIFO中断标志位，0表示无中断，1表示有中断
    Uint16 TXFFST:5;          // 12:8   FIFO status 发送FIFO还有几个数据没有发送
    Uint16 TXFIFO:1;          // 13     FIFO reset 发送FIFO复位，0：复位，1：使能
    Uint16 SPIFFENA:1;        // 14     Enhancement enable 增强模式使能，0表示禁用，1表示使能
    Uint16 SPIRST:1;          // 15     Reset SPI 复位SPI，初始化前将该位置1，初始化后将该位置0
};

union SPIFFTX_REG {
    Uint16               all;
    struct SPIFFTX_BITS  bit;
};

//
// SPI FIFO recieve register bit definitions
//
struct  SPIFFRX_BITS {        // bits   description
    Uint16 RXFFIL:5;          // 4:0    Interrupt level 接收FIFO中断触发水位
    Uint16 RXFFIENA:1;        // 5      Interrupt enable 接收FIFO中断使能，0表示禁用，1表示使能
    Uint16 RXFFINTCLR:1;      // 6      Clear INT flag 清除接收FIFO中断标志位，初始化前将该位置1，初始化后将该位置0
    Uint16 RXFFINT:1;         // 7      INT flag 接收FIFO中断标志位，0表示无中断，1表示有中断
    Uint16 RXFFST:5;          // 12:8   FIFO status 接收FIFO还有几个空位
    Uint16 RXFIFORESET:1;     // 13     FIFO reset 接收FIFO复位 0：复位，1：使能
    Uint16 RXFFOVFCLR:1;      // 14     Clear overflow 清除溢出标志位，初始化前将该位置1，初始化后将该位置0
    Uint16 RXFFOVF:1;         // 15     FIFO overflow 溢出标志位，0表示无溢出，1表示有溢出
};

union SPIFFRX_REG {
    Uint16               all;
    struct SPIFFRX_BITS  bit;
};

//
// SPI FIFO control register bit definitions
//
struct  SPIFFCT_BITS {        // bits   description
    Uint16 TXDLY:8;           // 7:0    FIFO transmit delay 发送FIFO延迟：每个字符从 FIFO 发出后，下一个字符开始发送前的等待时间。
    Uint16 rsvd:8;            // 15:8   reserved
}; 

union SPIFFCT_REG {
    Uint16               all;
    struct SPIFFCT_BITS  bit;
};

//
// SPI configuration register bit definitions
//
struct  SPICCR_BITS {         // bits   description
    Uint16 SPICHAR:4;         // 3:0    Character length control 字符长度控制，数据传输每个数据的位数
    Uint16 SPILBK:1;          // 4      Loop-back enable/disable 环回模式使能/禁用：0表示禁用，1表示使能
    Uint16 rsvd1:1;           // 5      reserved
    Uint16 CLKPOLARITY:1;     // 6      Clock polarity 时钟线空闲极性配置：0表示空闲时为低电平，1表示空闲时为高电平
    Uint16 SPISWRESET:1;      // 7      SPI SW Reset 软件复位，初始化前将改位清零，初始化后将该位置1
    Uint16 rsvd2:8;           // 15:8   reserved
};

union SPICCR_REG {
    Uint16              all;
    struct SPICCR_BITS  bit;
};

//
// SPI operation control register bit definitions
//
struct  SPICTL_BITS {         // bits   description
    Uint16 SPIINTENA:1;       // 0      发送/接收中断使能(SPISTS.6不受此位控制)
    Uint16 TALK:1;            // 1      发送使能(主从模式均生效): 0=禁止SOMI输出(高阻), 1=允许发送，控制SPI是否能说话
    Uint16 MASTER_SLAVE:1;    // 2      主从模式选择(Network control mode): 0=从机(SPICLK外部输入), 1=主机(本机产生SPICLK)
    Uint16 CLK_PHASE:1;       // 3      时钟相位(CPHA): 需与CPOL配合使用
                               //        CPHA=0(CPOL任意): 数据在时钟第一个边沿采样, 第二个边沿变化; Mode0/Mode2
                               //        CPHA=1(CPOL任意): 数据在时钟第二个边沿采样, 第一个边沿变化; Mode1/Mode3
                               //        Mode0(CPOL=0,CPHA=0) Mode1(CPOL=0,CPHA=1) Mode2(CPOL=1,CPHA=0) Mode3(CPOL=1,CPHA=1)
    Uint16 OVERRUNINTENA:1;   // 4      接收溢出(OVERRUN)中断使能: 0=禁用, 1=使能
    Uint16 rsvd:11;           // 15:5   reserved
};

union SPICTL_REG {
    Uint16              all;
    struct SPICTL_BITS  bit;
};

//
// SPI status register bit definitions
//
struct  SPISTS_BITS {         // bits   description
    Uint16 rsvd1:5;           // 4:0    reserved
    Uint16 BUFFULL_FLAG:1;    // 5      SPI transmit buffer full flag 发送缓冲区满标志位，若启用FIFO模式，该位由SPIFFTX.TXFFST替代
    Uint16 INT_FLAG:1;        // 6      SPI interrupt flag 中断标志位: 0=无中断, 1=有中断
    Uint16 OVERRUN_FLAG:1;    // 7      SPI reciever overrun flag 接收溢出标志位: 0=正常, 1=接收溢出
    Uint16 rsvd2:8;           // 15:8   reserved
};

union SPISTS_REG {
    Uint16              all;
    struct SPISTS_BITS  bit;
};

//
// SPI priority control register bit definitions
// 说明: 此寄存器仅用于控制仿真挂起(Emulation Suspend, 即断点/单步/暂停)时 SPI 的行为,
//       正常运行时不起作用。不影响通信速率、中断优先级或数据收发。
//
// FREE/SOFT 组合行为(仿真挂起时):
//   FREE=0, SOFT=0: 立即停止 - 传输立刻冻结, SPICLK 停在当前电平, 从机可能收到半截字符(不推荐)
//   FREE=0, SOFT=1: 完成当前字符后停止 - 等当前帧发完再停, SPICLK 归空闲(推荐调试用)
//   FREE=1, SOFT=0: 自由运行 - 完全忽略挂起, SPI 继续按 FIFO 节奏工作(实时系统用)
//   FREE=1, SOFT=1: 自由运行 - 同上
//
struct  SPIPRI_BITS {         // bits   description
    Uint16 rsvd1:4;           // 3:0    reserved
    Uint16 FREE:1;            // 4      自由运行模式控制: 0=响应挂起, 1=忽略挂起继续运行
    Uint16 SOFT:1;            // 5      软停止模式控制: 配合FREE使用, FREE=0时 SOFT=0立即停止/SOFT=1完成当前字符后停止
    Uint16 rsvd2:1;           // 6      reserved
    Uint16 rsvd3:9;           // 15:7   reserved
};

union SPIPRI_REG {
    Uint16              all;
    struct SPIPRI_BITS  bit;
};

//
// SPI Register File
//
struct  SPI_REGS {
    union SPICCR_REG     SPICCR;      // Configuration register
    union SPICTL_REG     SPICTL;      // Operation control register
    union SPISTS_REG     SPISTS;      // Status register
    Uint16               rsvd1;       // reserved
    Uint16               SPIBRR;      // Baud Rate 波特率设置 SPICLK = LSPCLK /（SPIBRR+1）
    Uint16               rsvd2;       // reserved 
    Uint16               SPIRXEMU;    // Emulation buffer 调试专用，是SPIRXBUF的镜像寄存器
    Uint16               SPIRXBUF;    // Serial input buffer 串行接收缓冲区,数据从SPIDAT中取到SPIRXBUF，等待用户取走，
                                      // FIFO模式由SPIFFRX取代该缓冲区,但仍可读取（实际从 FIFO 弹出）
    Uint16               SPITXBUF;    // Serial output buffer 串行发送缓冲区(发送缓冲区，数据先暂存在这里，等待装载到SPIDAT中，
                                      // FIFO模式由FIFO取代该缓冲区,但仍可写入（实际压入 FIFO）)
    Uint16               SPIDAT;      // Serial data 串行数据移位寄存器，移位寄存器，用于与外部设备交换数据
    union SPIFFTX_REG    SPIFFTX;     // FIFO transmit register 发送FIFO控制寄存器，用于控制发送FIFO的
    union SPIFFRX_REG    SPIFFRX;     // FIFO recieve register 接收FIFO控制寄存器，用于控制接收FIFO的
    union SPIFFCT_REG    SPIFFCT;     // FIFO control register 发送FIFO控制寄存器，用于控制发送FIFO的
    Uint16               rsvd3[2];    // reserved
    union SPIPRI_REG     SPIPRI;      // FIFO Priority control 发送FIFO优先级控制寄存器，用于控制发送FIFO的优先级
};

//
// SPI External References & Function Declarations
//
extern volatile struct SPI_REGS SpiaRegs;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2833x_SPI_H definition

//
// End of file
//

