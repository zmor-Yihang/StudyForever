#include "adc.h"

void adc_init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
    SysCtrlRegs.HISPCP.all = 0x1; // HSPCLK = SYSCLKOUT/2 = 75MHz
    EDIS;

    InitAdc();

    AdcRegs.ADCTRL3.bit.ADCCLKPS = 3; // ADCCLK = 75MHz/(2*3) = 12.5MHz
    AdcRegs.ADCTRL1.bit.ACQ_PS = 6;   // 采样窗口 = 7个ADCCLK
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1; // 级联模式
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0; // 单次运行

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;  // 把ADCINA0添加到排序器，放在第0位
    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x0; // 每轮转换一个通道

    AdcRegs.ADCTRL2.all = 0x2000; // 软件启动 SEQ
}

Uint16 adc_read(void)
{
    AdcRegs.ADCTRL2.all = 0x2000; // 软件启动 SEQ
    while (AdcRegs.ADCST.bit.INT_SEQ1 == 0)
        ; // 等待转换完成
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    return AdcMirror.ADCRESULT0; // 左对齐→右对齐，结果范围 0~4095
}