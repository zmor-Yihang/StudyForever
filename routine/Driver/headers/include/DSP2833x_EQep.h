// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:52:13 $
//###########################################################################
//
// FILE:   DSP2833x_EQep.h
//
// TITLE:  DSP2833x Enhanced Quadrature Encoder Pulse Module 
//         Register Bit Definitions.
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

#ifndef DSP2833x_EQEP_H
#define DSP2833x_EQEP_H


#ifdef __cplusplus
extern "C" {
#endif

//
// Capture decoder control register bit definitions
// 正交解码单元控制寄存器
//
struct QDECCTL_BITS {       // bits  description
    Uint16 rsvd1:5;         // 4:0   reserved
    Uint16 QSP:1;           // 5     QEPS input polarity 选通输入信号极性
    Uint16 QIP:1;           // 6     QEPI input polarity Z相输入信号极性
    Uint16 QBP:1;           // 7     QEPB input polarity B相输入信号极性
    Uint16 QAP:1;           // 8     QEPA input polarity A相输入信号极性
    Uint16 IGATE:1;         // 9     Index pulse gating option
                  // 索引门控：QEPI 引脚信号直接作为索引事件，只要边沿到来就触发；使能门控：QEPI 信号必须与正交信号的有效电平组合同时成立才被识别为索引事件
                  // IGATE 是索引信号 QEPI 的"抗噪门控开关"：置 1 后，索引脉冲必须与正交信号有效状态对齐才被识别，可滤除噪声毛刺。
    Uint16 SWAP:1;          // 10    CLK/DIR signal source for Position Counter AB相交换使能，类似三相电交换相序
    Uint16 XCR:1;           // 11    External clock rate 外部时钟计数倍率：0=双边沿(2x), 1=单边沿(1x)，仅方向计数模式有效
    Uint16 SPSEL:1;         // 12    Sync output pin select 同步输出引脚选择（把QEPI或QEPS作为同步输出引脚）
    Uint16 SOEN:1;          // 13    Enable position compare sync 使能位置比较同步
    Uint16 QSRC:2;          // 15:14 Position counter source 位置计数模式配置：正交计数模式：QEPA/QEPB 当作 A/B 相，靠相位判断方向
                                                          // 方向计数模式：QEPA/XCLK 给脉冲，QEPB/XDIR 给方向
                                                          // 递增 / 递减计数模式：只计数，不判断方向 
};

union QDECCTL_REG {
    Uint16              all;
    struct QDECCTL_BITS   bit;
};

//
// QEP control register bit definitions
//
struct QEPCTL_BITS {            // bits   description
    Uint16 WDE:1;               // 0      QEP watchdog enable
    Uint16 UTE:1;               // 1      QEP unit timer enable
    Uint16 QCLM:1;              // 2      QEP capture latch mode
    Uint16 QPEN:1;              // 3      Quadrature position counter enable 正交位置计数器使能
    Uint16 IEL:2;               // 5:4    Index event latch 索引事件锁存位置计数器
    Uint16 SEL:1;               // 6      Strobe event latch 选通事件锁存位置计数器
    Uint16 SWI:1;               // 7      Software init position counter 软件初始化位置计数器：0=不动作，1=软件初始化
    Uint16 IEI:2;               // 9:8    Index event init of position count 索引事件初始化计数值：00 / 01 不动作，10索引计数上升沿初始化，11=索引计数下降沿初始化
    Uint16 SEI:2;               // 11:10  Strobe event init 选通事件初始化计数值：00 / 01 不动作，10选通计数上升沿初始化，11=选通计数下降沿初始化
    Uint16 PCRM:2;              // 13:12  Position counter reset 计数器复位事件选择,
                                                            // PCRM=00：索引事件复位，每次 QI 到来复位
                                                            // PCRM=01：最大值复位，到达 QPOSMAX 上溢/下溢回卷
                                                            // PCRM=10：第一次索引事件复位，只在第一次 QI 复位
                                                            // PCRM=11：单位时间事件复位，每个单位时间事件复位
    Uint16 FREE_SOFT:2;         // 15:14  Emulation mode
};

union QEPCTL_REG {
    Uint16               all;
    struct QEPCTL_BITS   bit;
};         

//
// Quadrature capture control register bit definitions
//
struct QCAPCTL_BITS {          // bits   description
    Uint16 UPPS:4;              // 3:0    Unit position pre-scale         
    Uint16 CCPS:3;              // 6:4    QEP capture timer pre-scale
    Uint16 rsvd1:8;             // 14:7   reserved
    Uint16 CEN:1;               // 15     Enable QEP capture
};

union QCAPCTL_REG {
    Uint16               all;
    struct QCAPCTL_BITS  bit;
}; 

//
// Position compare control register bit definitions
//
struct QPOSCTL_BITS {           // bits   description
    Uint16 PCSPW:12;            // 11:0   Position compare sync pulse width
    Uint16 PCE:1;               // 12     Position compare enable/disable
    Uint16 PCPOL:1;             // 13     Polarity of sync output
    Uint16 PCLOAD:1;            // 14     Position compare of shadow load
    Uint16 PCSHDW:1;            // 15     Position compare shadow enable
};

union QPOSCTL_REG {
    Uint16               all;
    struct QPOSCTL_BITS  bit;
};         

//
// QEP interrupt control register bit definitions
//
struct QEINT_BITS {           // bits   description
    Uint16 rsvd1:1;           // 0      reserved
    Uint16 PCE:1;             // 1      Position counter error
    Uint16 QPE:1;             // 2      Quadrature phase error
    Uint16 QDC:1;             // 3      Quadrature dir change
    Uint16 WTO:1;             // 4      Watchdog timeout
    Uint16 PCU:1;             // 5      Position counter underflow
    Uint16 PCO:1;             // 6      Position counter overflow
    Uint16 PCR:1;             // 7      Position compare ready
    Uint16 PCM:1;             // 8      Position compare match
    Uint16 SEL:1;             // 9      Strobe event latch
    Uint16 IEL:1;             // 10     Event latch
    Uint16 UTO:1;             // 11     Unit timeout
    Uint16 rsvd2:4;           // 15:12  reserved
};

union QEINT_REG {
    Uint16               all;
    struct QEINT_BITS    bit;
};

//
// QEP interrupt status register bit definitions
//
struct QFLG_BITS {           // bits   description
    Uint16 INT:1;            // 0      Global interrupt 
    Uint16 PCE:1;            // 1      Position counter error
    Uint16 PHE:1;            // 2      Quadrature phase error
    Uint16 QDC:1;            // 3      Quadrature dir change
    Uint16 WTO:1;            // 4      Watchdog timeout
    Uint16 PCU:1;            // 5      Position counter underflow
    Uint16 PCO:1;            // 6      Position counter overflow
    Uint16 PCR:1;            // 7      Position compare ready
    Uint16 PCM:1;            // 8      Position compare match
    Uint16 SEL:1;            // 9      Strobe event latch
    Uint16 IEL:1;            // 10     Event latch
    Uint16 UTO:1;            // 11     Unit timeout
    Uint16 rsvd2:4;          // 15:12  reserved
};

union QFLG_REG {
    Uint16               all;
    struct QFLG_BITS     bit;
};

//
// QEP interrupt force register bit definitions
//
struct QFRC_BITS {           // bits   description
    Uint16 reserved:1;       // 0      Reserved
    Uint16 PCE:1;            // 1      Position counter error
    Uint16 PHE:1;            // 2      Quadrature phase error
    Uint16 QDC:1;            // 3      Quadrature dir change
    Uint16 WTO:1;            // 4      Watchdog timeout
    Uint16 PCU:1;            // 5      Position counter underflow
    Uint16 PCO:1;            // 6      Position counter overflow
    Uint16 PCR:1;            // 7      Position compare ready
    Uint16 PCM:1;            // 8      Position compare match
    Uint16 SEL:1;            // 9      Strobe event latch
    Uint16 IEL:1;            // 10     Event latch
    Uint16 UTO:1;            // 11     Unit timeout
    Uint16 rsvd2:4;          // 15:12  reserved
};


union QFRC_REG {
    Uint16               all;
    struct QFRC_BITS     bit;
};

//
// V1.1 Added UPEVNT (bit 7) This reflects changes 
// made as of F2833x Rev A devices
//

//
// QEP status register bit definitions
//
struct QEPSTS_BITS {           // bits   description
    Uint16 PCEF:1;             // 0      Position counter error
    Uint16 FIMF:1;             // 1      First index marker
    Uint16 CDEF:1;             // 2      Capture direction error
    Uint16 COEF:1;             // 3      Capture overflow error
    Uint16 QDLF:1;             // 4      QEP direction latch
    Uint16 QDF:1;              // 5      Quadrature direction
    Uint16 FIDF:1;             // 6      Direction on first index marker
    Uint16 UPEVNT:1;           // 7      Unit position event flag
    Uint16 rsvd1:8;            // 15:8   reserved
};

union QEPSTS_REG {
    Uint16               all;
    struct QEPSTS_BITS   bit;
};

struct EQEP_REGS {
    Uint32              QPOSCNT;   // Position counter 位置计数器：反映当前编码器位置（脉冲累计值）
    Uint32              QPOSINIT;  // Position counter init 位置计数器初始值
    Uint32              QPOSMAX;   // Maximum position count 最大位置计数：记录编码器最大可计数的脉冲数，用于防止位置计数器溢出
    Uint32              QPOSCMP;   // Position compare 位置比较：用于比较位置计数器和目标位置，当位置计数器达到目标位置时，产生事件
    Uint32              QPOSILAT;  // Index position latch Z相脉冲发生时锁存：记录编码器Z相脉冲位置
    Uint32              QPOSSLAT;  // Strobe position latch 选通(Strobe)事件发生时锁存
    Uint32              QPOSLAT;   // Position latch 单位时间事件锁存
    Uint32              QUTMR;     // Unit timer 
    Uint32              QUPRD;     // Unit period
    Uint16              QWDTMR;    // QEP watchdog timer
    Uint16              QWDPRD;    // QEP watchdog period
    union  QDECCTL_REG  QDECCTL;   // Quadrature decoder control 解码单元控制寄存器
    union  QEPCTL_REG   QEPCTL;    // QEP control  位置计数器 + 定时器 + 看门狗 控制寄存器
    union  QCAPCTL_REG  QCAPCTL;   // Quadrature capture control     边沿捕获单元控制寄存器
    union  QPOSCTL_REG  QPOSCTL;   // Position compare control 位置比较单元控制寄存器
    union  QEINT_REG    QEINT;     // QEP interrupt control        
    union  QFLG_REG     QFLG;      // QEP interrupt flag
    union  QFLG_REG     QCLR;      // QEP interrupt clear                   
    union  QFRC_REG     QFRC;      // QEP interrupt force                   
    union  QEPSTS_REG   QEPSTS;    // QEP status
    Uint16              QCTMR;     // QEP capture timer
    Uint16              QCPRD;     // QEP capture period
    Uint16              QCTMRLAT;  // QEP capture latch
    Uint16              QCPRDLAT;  // QEP capture period latch
    Uint16              rsvd1[30]; // reserved
};

//
// GPI/O External References & Function Declarations
//
extern volatile struct EQEP_REGS EQep1Regs;
extern volatile struct EQEP_REGS EQep2Regs;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2833x_EQEP_H definition

//
// End of file
//

