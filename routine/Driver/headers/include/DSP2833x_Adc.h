// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:51:50 $
//###########################################################################
//
// FILE:   DSP2833x_Adc.h
//
// TITLE:  DSP2833x Device ADC Register Definitions.
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

#ifndef DSP2833x_ADC_H
#define DSP2833x_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

//
// ADC Individual Register Bit Definitions:
//
struct ADCTRL1_BITS {     // bits  description
    Uint16  rsvd1:4;      // 3:0   reserved
    Uint16  SEQ_CASC:1;   // 4     Cascaded sequencer mode 排序器连接模式控制，级联还是双排序器同时工作
    Uint16  SEQ_OVRD:1;   // 5     Sequencer override 排序器覆盖，连续运行下指针复位依据：0=转完MAX_CONVn个通道后复位 / 1=走到排序表最后再复位
    Uint16  CONT_RUN:1;   // 6     Continuous run 连续运行模式，写1时，adc会连续进行转换
    Uint16  CPS:1;        // 7     ADC core clock pre-scalar 时钟分频控制位，对外设时钟总线再次分频
    Uint16  ACQ_PS:4;     // 11:8  Acquisition window size 采样时间控制，决定采样时间持续多久
    Uint16  SUSMOD:2;     // 13:12 Emulation suspend mode 仿真暂停控制位，cpu挂起时，adc是继续工作 / 完成当前转换后暂停 / 立即暂停工作
    Uint16  RESET:1;      // 14    ADC reset  adc复位，写1复位
    Uint16  rsvd2:1;      // 15    reserved
};

union ADCTRL1_REG {
   Uint16                all;
   struct ADCTRL1_BITS   bit;
};

struct ADCTRL2_BITS {         // bits  description
    Uint16  EPWM_SOCB_SEQ2:1; // 0     EPWM compare B SOC mask for SEQ2 epwmA触发使能，写1允许EPWMA 触发ADC采样转换
    Uint16  rsvd1:1;          // 1     reserved
    Uint16  INT_MOD_SEQ2:1;   // 2     SEQ2 Interrupt mode 排序器2中断模式，0=每次中断都触发中断 / 1=隔一次触发一次中断
    Uint16  INT_ENA_SEQ2:1;   // 3     SEQ2 Interrupt enable 排序器2中断使能位
    Uint16  rsvd2:1;          // 4     reserved
    Uint16  SOC_SEQ2:1;       // 5     Start of conversion for SEQ2 写1触发ADC采样转换（软件触发直接写1，外部触发该位自动置1）
    Uint16  RST_SEQ2:1;       // 6     Reset SEQ2 写1复位排序器2

    // 级联模式下，由SEQ1 排序器控制，SEQ2 排序器控制寄存器不起作用
    Uint16  EXT_SOC_SEQ1:1;   // 7     External start of conversion for SEQ1 外部信号触发使能位
    Uint16  EPWM_SOCA_SEQ1:1; // 8     EPWM compare B SOC mask for SEQ1 epwmA触发使能，写1允许EPWMA 触发ADC采样转换
    Uint16  rsvd3:1;          // 9     reserved
    Uint16  INT_MOD_SEQ1:1;   // 10    SEQ1 Interrupt mode   排序器1中断模式，0=每次中断都触发中断 / 1=隔一次触发一次中断
    Uint16  INT_ENA_SEQ1:1;   // 11    SEQ1 Interrupt enable 排序器1中断使能位
    Uint16  rsvd4:1;          // 12    reserved
    Uint16  SOC_SEQ1:1;       // 13    Start of conversion trigger for SEQ1，写1触发ADC采样转换（软件触发直接写1，外部触发该位自动置1）
    Uint16  RST_SEQ1:1;       // 14    Restart sequencer 1      写1复位排序器1
    Uint16  EPWM_SOCB_SEQ:1;  // 15    EPWM compare B SOC enable，（级联模式专用）epwmB触发使能，写1允许EPWMB 触发ADC采样转换
};

union ADCTRL2_REG {
   Uint16                all;
   struct ADCTRL2_BITS   bit;
};

struct ADCASEQSR_BITS {       // bits   description
    Uint16  SEQ1_STATE:4;     // 3:0    SEQ1 state   排序器1还有几个通道没转换
    Uint16  SEQ2_STATE:3;     // 6:4    SEQ2 state   排序器2还有几个通道没转换
    Uint16  rsvd1:1;          // 7      reserved
    Uint16  SEQ_CNTR:4;       // 11:8   Sequencing counter status 级联模式，还剩几个通道没转换
    Uint16  rsvd2:4;          // 15:12  reserved  
};

union ADCASEQSR_REG {
    Uint16                 all;
    struct ADCASEQSR_BITS  bit;
};

struct ADCMAXCONV_BITS {      // bits  description
    Uint16  MAX_CONV1:4;      // 3:0   Max number of conversions 排序器1转换通道数
    Uint16  MAX_CONV2:3;      // 6:4   Max number of conversions 排序器2转换通道数
    Uint16  rsvd1:9;          // 15:7  reserved 
};

union ADCMAXCONV_REG {
    Uint16                  all;
    struct ADCMAXCONV_BITS  bit;
};

struct ADCCHSELSEQ1_BITS {    // bits   description
    Uint16  CONV00:4;         // 3:0    Conversion selection 00
    Uint16  CONV01:4;         // 7:4    Conversion selection 01
    Uint16  CONV02:4;         // 11:8   Conversion selection 02
    Uint16  CONV03:4;         // 15:12  Conversion selection 03
};

union  ADCCHSELSEQ1_REG{
    Uint16                    all;
    struct ADCCHSELSEQ1_BITS  bit;
};

struct ADCCHSELSEQ2_BITS {    // bits   description
    Uint16  CONV04:4;         // 3:0    Conversion selection 04
    Uint16  CONV05:4;         // 7:4    Conversion selection 05
    Uint16  CONV06:4;         // 11:8   Conversion selection 06
    Uint16  CONV07:4;         // 15:12  Conversion selection 07
};

union  ADCCHSELSEQ2_REG{
    Uint16                    all;
    struct ADCCHSELSEQ2_BITS  bit;
};

struct ADCCHSELSEQ3_BITS {    // bits   description
    Uint16  CONV08:4;         // 3:0    Conversion selection 08
    Uint16  CONV09:4;         // 7:4    Conversion selection 09
    Uint16  CONV10:4;         // 11:8   Conversion selection 10
    Uint16  CONV11:4;         // 15:12  Conversion selection 11
};

union  ADCCHSELSEQ3_REG{
    Uint16                    all;
    struct ADCCHSELSEQ3_BITS  bit;
};

struct ADCCHSELSEQ4_BITS {    // bits   description
    Uint16  CONV12:4;         // 3:0    Conversion selection 12
    Uint16  CONV13:4;         // 7:4    Conversion selection 13
    Uint16  CONV14:4;         // 11:8   Conversion selection 14
    Uint16  CONV15:4;         // 15:12  Conversion selection 15
};

union  ADCCHSELSEQ4_REG {
    Uint16                    all;
    struct ADCCHSELSEQ4_BITS  bit;
};

struct ADCTRL3_BITS {         // bits   description
    Uint16   SMODE_SEL:1;     // 0      Sampling mode select  采样模式选择，0：顺序采样，1：同步采样
    Uint16   ADCCLKPS:4;      // 4:1    ADC core clock divider 外设时钟分频控制位，对外设时钟总线再次分频
    Uint16   ADCPWDN:1;       // 5      ADC powerdown  休眠控制位，写1休眠adc（除去参考基准电压电路，参考基准电压电路的供电由下面单独控制）
    Uint16   ADCBGRFDN:2;     // 7:6    ADC bandgap/ref power down 参考基准电压休眠控制位，写1休眠参考基准电压
    Uint16   rsvd1:8;         // 15:8   reserved
}; 

union  ADCTRL3_REG {
    Uint16                all;
    struct ADCTRL3_BITS   bit;
};

struct ADCST_BITS {           // bits   description
    Uint16   INT_SEQ1:1;      // 0      SEQ1 Interrupt flag  排序器1中断标志位，这里标志的是转换完成
    Uint16   INT_SEQ2:1;      // 1      SEQ2 Interrupt flag  排序器2中断标志位，这里标志的是转换完成
    Uint16   SEQ1_BSY:1;      // 2      SEQ1 busy status 排序器1忙标志位，表示ADC是否正在转换
    Uint16   SEQ2_BSY:1;      // 3      SEQ2 busy status 排序器2忙标志位，表示ADC是否正在转换
    Uint16   INT_SEQ1_CLR:1;  // 4      SEQ1 Interrupt clear  排序器1中断清除位，写1清除排序器1中断标志位
    Uint16   INT_SEQ2_CLR:1;  // 5      SEQ2 Interrupt clear  排序器2中断清除位，写1清除排序器2中断标志位
    Uint16   EOS_BUF1:1;      // 6      End of sequence buffer1 记录“已经完成过一次序列”。在中断模式2下，第一次EOS先置位，第二次EOS才真正产生 INT_SEQ1 中断
    Uint16   EOS_BUF2:1;      // 7      End of sequence buffer2 SEQ2的序列结束缓冲标志
    Uint16   rsvd1:8;         // 15:8   reserved
};
                           
union  ADCST_REG {            
    Uint16             all;    
    struct ADCST_BITS  bit;    
};                           

struct ADCREFSEL_BITS {       // bits   description
	Uint16   rsvd1:14;        // 13:0   reserved  
	Uint16   REF_SEL:2;       // 15:14  Reference select
};
union ADCREFSEL_REG {
	Uint16		all;
	struct ADCREFSEL_BITS bit;
};

struct ADCOFFTRIM_BITS{       // bits   description
	int16	OFFSET_TRIM:9;    // 8:0    Offset Trim  
	Uint16	rsvd1:7;          // 15:9   reserved
};

union ADCOFFTRIM_REG{
	Uint16		all;
	struct ADCOFFTRIM_BITS bit;
};

struct ADC_REGS {
    union ADCTRL1_REG      ADCTRL1;       //ADC Control 1 
    union ADCTRL2_REG      ADCTRL2;       //ADC Control 2
    union ADCMAXCONV_REG   ADCMAXCONV;    //Max conversions 最大转换次数

    // 通道选择排列表：4个寄存器各含4个4位字段，共16步，每步填0~15指定采哪个通道（0~7=ADCINA0~7, 8~15=ADCINB0~7）
    union ADCCHSELSEQ1_REG ADCCHSELSEQ1;  //Channel select sequencing control 1 通道0~3的采样优先级设置
    union ADCCHSELSEQ2_REG ADCCHSELSEQ2;  //Channel select sequencing control 2 通道4~7的采样优先级设置
    union ADCCHSELSEQ3_REG ADCCHSELSEQ3;  //Channel select sequencing control 3 通道8~11的采样优先级设置
    union ADCCHSELSEQ4_REG ADCCHSELSEQ4;  //Channel select sequencing control 4 通道12~15的采样优先级设置
    union ADCASEQSR_REG    ADCASEQSR;     //Autosequence status register 排序器状态寄存器，表示还有几个通道没转换

    //
    // 转换结果缓冲区：16个16位寄存器，每个寄存器对应一个通道的转换结果
    // 注意：转换结果是12位，需要右移4位得到12位有效数据
    //
    Uint16                 ADCRESULT0;    //Conversion Result Buffer 0
    Uint16                 ADCRESULT1;    //Conversion Result Buffer 1
    Uint16                 ADCRESULT2;    //Conversion Result Buffer 2
    Uint16                 ADCRESULT3;    //Conversion Result Buffer 3
    Uint16                 ADCRESULT4;    //Conversion Result Buffer 4
    Uint16                 ADCRESULT5;    //Conversion Result Buffer 5
    Uint16                 ADCRESULT6;    //Conversion Result Buffer 6
    Uint16                 ADCRESULT7;    //Conversion Result Buffer 7
    Uint16                 ADCRESULT8;    //Conversion Result Buffer 8
    Uint16                 ADCRESULT9;    //Conversion Result Buffer 9
    Uint16                 ADCRESULT10;   //Conversion Result Buffer 10
    Uint16                 ADCRESULT11;   //Conversion Result Buffer 11
    Uint16                 ADCRESULT12;   //Conversion Result Buffer 12
    Uint16                 ADCRESULT13;   //Conversion Result Buffer 13
    Uint16                 ADCRESULT14;   //Conversion Result Buffer 14
    Uint16                 ADCRESULT15;   //Conversion Result Buffer 15

    union ADCTRL3_REG      ADCTRL3;       //ADC Control 3  

    union ADCST_REG        ADCST;         //ADC Status Register 状态寄存器
    Uint16				   rsvd1;
    Uint16                 rsvd2;

    // 
    // 这两个与ADC校准有关，
    // 系统初始化时在InitSysCtrl - InitPeripheralClocks - ADC_call中调用了
    // 校准值在芯片出厂时 TI 已经写入在OTP中
    //
    union ADCREFSEL_REG    ADCREFSEL;     //Reference Select Register
    union ADCOFFTRIM_REG   ADCOFFTRIM;    //Offset Trim Register
};


//
// 转换结果镜像寄存器，用于将转换结果从 ADC_REGS 复制到 ADC_RESULT_MIRROR_REGS
// 该结果是右对齐的，不需要处理，方便使用
// 
struct ADC_RESULT_MIRROR_REGS
{
    Uint16                 ADCRESULT0;    // Conversion Result Buffer 0
    Uint16                 ADCRESULT1;    // Conversion Result Buffer 1
    Uint16                 ADCRESULT2;    // Conversion Result Buffer 2
    Uint16                 ADCRESULT3;    // Conversion Result Buffer 3
    Uint16                 ADCRESULT4;    // Conversion Result Buffer 4
    Uint16                 ADCRESULT5;    // Conversion Result Buffer 5
    Uint16                 ADCRESULT6;    // Conversion Result Buffer 6
    Uint16                 ADCRESULT7;    // Conversion Result Buffer 7
    Uint16                 ADCRESULT8;    // Conversion Result Buffer 8
    Uint16                 ADCRESULT9;    // Conversion Result Buffer 9
    Uint16                 ADCRESULT10;   // Conversion Result Buffer 10
    Uint16                 ADCRESULT11;   // Conversion Result Buffer 11
    Uint16                 ADCRESULT12;   // Conversion Result Buffer 12
    Uint16                 ADCRESULT13;   // Conversion Result Buffer 13
    Uint16                 ADCRESULT14;   // Conversion Result Buffer 14
    Uint16                 ADCRESULT15;   // Conversion Result Buffer 15
};

//
// ADC External References & Function Declarations:
//
extern volatile struct ADC_REGS AdcRegs;
extern volatile struct ADC_RESULT_MIRROR_REGS AdcMirror;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP2833x_ADC_H definition

//
// End of file
//

