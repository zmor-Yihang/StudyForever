#include "led.h"
#include "DSP28x_Project.h"

// 28335 GPIO 端口资源分布
// GPIO 编号范围     分组/端口      寄存器前缀
// GPIO0～GPIO31   GPIO Port A        GPA
// GPIO32～GPIO63  GPIO Port B        GPB
// GPIO64～GPIO87  GPIO Port C        GPC

void led_init(led_id_t led)
{
    EALLOW;

    switch (led)
    {
    case LED_D8:
        GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;
        GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;
        GpioCtrlRegs.GPBPUD.bit.GPIO60 = 1;
        GpioDataRegs.GPBSET.bit.GPIO60 = 1;
        break;

    case LED_D9:
        GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;
        GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;
        GpioCtrlRegs.GPBPUD.bit.GPIO61 = 1;
        GpioDataRegs.GPBSET.bit.GPIO61 = 1;
        break;

    case LED_D10:
        GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;
        GpioCtrlRegs.GPCDIR.bit.GPIO64 = 1;
        GpioCtrlRegs.GPCPUD.bit.GPIO64 = 1;
        GpioDataRegs.GPCSET.bit.GPIO64 = 1;
        break;

    case LED_D11:
        GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;
        GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;
        GpioCtrlRegs.GPCPUD.bit.GPIO65 = 1;
        GpioDataRegs.GPCSET.bit.GPIO65 = 1;
        break;

    case LED_D12:
        GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;
        GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1;
        GpioCtrlRegs.GPCPUD.bit.GPIO66 = 1;
        GpioDataRegs.GPCSET.bit.GPIO66 = 1;
        break;

    case LED_D13:
        GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
        GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;
        GpioCtrlRegs.GPCPUD.bit.GPIO67 = 1;
        GpioDataRegs.GPCSET.bit.GPIO67 = 1;
        break;

    case LED_D14:
        GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 0;
        GpioCtrlRegs.GPCDIR.bit.GPIO68 = 1;
        GpioCtrlRegs.GPCPUD.bit.GPIO68 = 1;
        GpioDataRegs.GPCSET.bit.GPIO68 = 1;
        break;

    default:
        break;
    }

    EDIS;
}

void led_on(led_id_t led)
{
    switch (led)
    {
    case LED_D8:
        GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;
        break;

    case LED_D9:
        GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;
        break;

    case LED_D10:
        GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;
        break;

    case LED_D11:
        GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;
        break;

    case LED_D12:
        GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;
        break;

    case LED_D13:
        GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
        break;

    case LED_D14:
        GpioDataRegs.GPCCLEAR.bit.GPIO68 = 1;
        break;

    default:
        break;
    }
}

void led_off(led_id_t led)
{
    switch (led)
    {
    case LED_D8:
        GpioDataRegs.GPBSET.bit.GPIO60 = 1;
        break;

    case LED_D9:
        GpioDataRegs.GPBSET.bit.GPIO61 = 1;
        break;

    case LED_D10:
        GpioDataRegs.GPCSET.bit.GPIO64 = 1;
        break;

    case LED_D11:
        GpioDataRegs.GPCSET.bit.GPIO65 = 1;
        break;

    case LED_D12:
        GpioDataRegs.GPCSET.bit.GPIO66 = 1;
        break;

    case LED_D13:
        GpioDataRegs.GPCSET.bit.GPIO67 = 1;
        break;

    case LED_D14:
        GpioDataRegs.GPCSET.bit.GPIO68 = 1;
        break;

    default:
        break;
    }
}

void led_toggle(led_id_t led)
{
    switch (led)
    {
    case LED_D8:
        GpioDataRegs.GPBTOGGLE.bit.GPIO60 = 1;
        break;

    case LED_D9:
        GpioDataRegs.GPBTOGGLE.bit.GPIO61 = 1;
        break;

    case LED_D10:
        GpioDataRegs.GPCTOGGLE.bit.GPIO64 = 1;
        break;

    case LED_D11:
        GpioDataRegs.GPCTOGGLE.bit.GPIO65 = 1;
        break;

    case LED_D12:
        GpioDataRegs.GPCTOGGLE.bit.GPIO66 = 1;
        break;

    case LED_D13:
        GpioDataRegs.GPCTOGGLE.bit.GPIO67 = 1;
        break;

    case LED_D14:
        GpioDataRegs.GPCTOGGLE.bit.GPIO68 = 1;
        break;

    default:
        break;
    }
}
