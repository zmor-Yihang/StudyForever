// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:52:07 $
//###########################################################################
//
// FILE:   DSP2833x_ECap.h
//
// TITLE:  DSP2833x Enhanced Capture Module Register Bit Definitions.
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

#ifndef DSP2833x_ECAP_H
#define DSP2833x_ECAP_H


#ifdef __cplusplus
extern "C" {
#endif

//
// Capture control register 1 bit definitions
//
struct ECCTL1_BITS {           // bits   description
    Uint16 CAP1POL:1;          // 0      Capture Event 1 Polarity select 捕获事件1的极性选择    
    Uint16 CTRRST1:1;          // 1      Counter Reset on Capture Event 1 重置计数器，当捕获事件 CAP1 发生时，重置计数器为0。
    Uint16 CAP2POL:1;          // 2      Capture Event 2 Polarity select 捕获事件2的极性选择
    Uint16 CTRRST2:1;          // 3      Counter Reset on Capture Event 2 重置计数器，当捕获事件 CAP2 发生时，重置计数器为0。
    Uint16 CAP3POL:1;          // 4      Capture Event 3 Polarity select 捕获事件3的极性选择
    Uint16 CTRRST3:1;          // 5      Counter Reset on Capture Event 3 重置计数器，当捕获事件 CAP3 发生时，重置计数器为0。
    Uint16 CAP4POL:1;          // 6      Capture Event 4 Polarity select 捕获事件4的极性选择
    Uint16 CTRRST4:1;          // 7      Counter Reset on Capture Event 4  重置计数器，当捕获事件 CAP4 发生时，重置计数器为0。
    Uint16 CAPLDEN:1;          // 8      Enable Loading CAP1-4 regs on a Cap 使能控制位，是否允许捕获事件把时间值装进 CAP1~CAP4 寄存器
                               //        Event
    Uint16 PRESCALE:5;         // 13:9   Event Filter prescale select //输入信号分频设置，就是隔几次捕获一次信号
    Uint16 FREE_SOFT:2;        // 15:14  Emulation mode // 仿真模式：CPU在断点暂停时 eCAP 计数器的行为，00=立即停止，01=当前周期结束后停止，1x=自由运行（调试时通常设为3）
};

union ECCTL1_REG {
    Uint16              all;
    struct ECCTL1_BITS  bit;
};

//
// In V1.1 the STOPVALUE bit field was changed to 
// STOP_WRAP.  This correlated to a silicon change from
// F2833x Rev 0 to Rev A. 
//

//
// Capture control register 2 bit definitions
//
struct ECCTL2_BITS {          // bits   description
    Uint16 CONT_ONESHT:1;      // 0      Continuous or one-shot 连续模式还是单次模式，连续模式就是循环记录捕获事件的时间值，单次模式就是记录1~4次捕获事件的时间值后停止
    //
    //    STOP_WRAP：00=CAP1，01=CAP2，10=CAP3，11=CAP4；该值与 MOD4 事件计数器比较。
    //    单次模式下，命中所选事件后停止捕获流程，CAP 寄存器不再装载新值；
    //    连续模式下，命中所选事件后从 CAP1 重新开始下一轮捕获，TSCTR 继续运行。
    //
    Uint16 STOP_WRAP:2;        // 2:1    Stop value for one-shot, Wrap for continuous 
    Uint16 REARM:1;            // 3      One-shot re-arm 单次模式下，单次模式下，捕获事件完成后，是否重新触发捕获事件
    Uint16 TSCTRSTOP:1;        // 4      TSCNT counter stop 停止计数器控制
    Uint16 SYNCI_EN:1;         // 5      Counter sync-in select 同步输入使能：是否允许输出同步信号
    Uint16 SYNCO_SEL:2;        // 7:6    Sync-out mode 同步输出信号选择：00=选择同步输入事件，01=计数值溢出时同步，10/11=不输出同步信号
    Uint16 SWSYNC:1;           // 8      SW forced counter sync 软件强制同步不同的 ecap 模块
    Uint16 CAP_APWM:1;         // 9      CAP/APWM operating mode select 选择捕获模式还是APWM模式
    Uint16 APWMPOL:1;          // 10     APWM output polarity select 输出极性配置，APWM模式才有效
    Uint16 rsvd1:5;            // 15:11  
};

union ECCTL2_REG {
    Uint16              all;
    struct ECCTL2_BITS  bit;
};

//
// ECAP interrupt enable register bit definitions
//
struct ECEINT_BITS {           // bits   description
    Uint16 rsvd1:1;            // 0      reserved
    Uint16 CEVT1:1;            // 1      Capture Event 1 Interrupt Enable 捕获事件1中断使能
    Uint16 CEVT2:1;            // 2      Capture Event 2 Interrupt Enable 捕获事件2中断使能
    Uint16 CEVT3:1;            // 3      Capture Event 3 Interrupt Enable 捕获事件3中断使能
    Uint16 CEVT4:1;            // 4      Capture Event 4 Interrupt Enable 捕获事件4中断使能
    Uint16 CTROVF:1;           // 5      Counter Overflow Interrupt Enable 计数器溢出中断使能
    Uint16 CTR_EQ_PRD:1;       // 6      Period Equal Interrupt Enable 计数值等于周期值中断使能 
    Uint16 CTR_EQ_CMP:1;       // 7      Compare Equal Interrupt Enable 计数值等于比较值中断使能
    Uint16 rsvd2:8;            // 15:8   reserved
};

union ECEINT_REG {
   Uint16              all;
   struct ECEINT_BITS  bit;
};

//
// ECAP interrupt flag register bit definitions
//
struct ECFLG_BITS {           // bits   description
    Uint16 INT:1;              // 0      Global Flag 全局中断标志位，有下面的中断产生时，该位被置1
    Uint16 CEVT1:1;            // 1      Capture Event 1 Interrupt Flag 捕获事件1中断标志位
    Uint16 CEVT2:1;            // 2      Capture Event 2 Interrupt Flag 捕获事件2中断标志位
    Uint16 CEVT3:1;            // 3      Capture Event 3 Interrupt Flag 捕获事件3中断标志位
    Uint16 CEVT4:1;            // 4      Capture Event 4 Interrupt Flag 捕获事件4中断标志位
    Uint16 CTROVF:1;           // 5      Counter Overflow Interrupt Flag 计数器溢出中断标志位
    Uint16 CTR_EQ_PRD:1;       // 6      Period Equal Interrupt Flag 计数值等于周期值中断标志位
    Uint16 CTR_EQ_CMP:1;       // 7      Compare Equal Interrupt Flag 计数值等于比较值中断标志位
    Uint16 rsvd2:8;            // 15:8   reserved
};

union ECFLG_REG {
    Uint16              all;
    struct ECFLG_BITS   bit;
};

struct ECAP_REGS {
    Uint32              TSCTR;    // Time stamp counter 捕获当前事件发生时刻的计数值
    Uint32              CTRPHS;   // Counter phase 计数器相位偏移值，就是给计数器设置一个初始值，然后计数器从该值开始计数
                                  // 一般用于多个计数器同步计数，比如需要同时测量多个信号的频率时

    // 将TSCTR寄存器中的值锁存到CAP1-4寄存器中，每个事件是上升沿还是下降沿科自由配置，
    // 比如四个事件全是上升沿，则四个寄存器记录的就是连续的四个上升沿的计数值
    //
    Uint32              CAP1;     // Capture 1 记录CAP1事件发生时刻的计数值，APWM模式下，起到ARPD的作用（周期值）
    Uint32              CAP2;     // Capture 2 记录CAP2事件发生时刻的计数值，APWM模式下，起到ACMP的作用（比较值）
    Uint32              CAP3;     // Capture 3 记录CAP3事件发生时刻的计数值，APWM模式下，ARPD的影子寄存器
    Uint32              CAP4;     // Capture 4 记录CAP4事件发生时刻的计数值，APWM模式下，ACMP的影子寄存器

    //
    Uint16              rsvd1[8]; // reserved
    union   ECCTL1_REG  ECCTL1;   // Capture Control Reg 1 / 捕获控制寄存器1
    union   ECCTL2_REG  ECCTL2;   // Capture Control Reg 2 / 捕获控制寄存器2
    union   ECEINT_REG  ECEINT;   // ECAP interrupt enable // 中断使能
    union   ECFLG_REG   ECFLG;    // ECAP interrupt flags  // 中断标志位
    union   ECFLG_REG   ECCLR;    // ECAP interrupt clear  // 中断清除
    union   ECEINT_REG  ECFRC;    // ECAP interrupt force  // 中断强制触发
    Uint16              rsvd2[6]; // reserved   
};

//
// GPI/O External References & Function Declarations
//
extern volatile struct ECAP_REGS ECap1Regs;
extern volatile struct ECAP_REGS ECap2Regs;
extern volatile struct ECAP_REGS ECap3Regs;
extern volatile struct ECAP_REGS ECap4Regs;
extern volatile struct ECAP_REGS ECap5Regs;
extern volatile struct ECAP_REGS ECap6Regs;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2833x_ECAP_H definition

//
// End of file
//

