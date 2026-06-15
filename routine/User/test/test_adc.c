#include "test/test_adc.h"
#include "DSP28x_Project.h"
#include "bsp/adc.h"
#include "bsp/sci.h"

//
// ADC 测试
//
// 上电后每隔约 500ms 采集一次 ADCINA0，
// 将原始值(0~4095)与换算电压(参考 3.0V)通过 SCIA 打印。
//

#define ADC_VREF_MV 3000UL

static void put_u16(char *buf, Uint16 *idx, Uint16 val)
{
    char tmp[5];
    Uint16 i = 0;

    if (val == 0)
    {
        buf[(*idx)++] = '0';
        return;
    }

    while (val > 0)
    {
        tmp[i++] = (char)('0' + (val % 10));
        val /= 10;
    }
    while (i > 0)
    {
        buf[(*idx)++] = tmp[--i];
    }
}

void test_adc_init(void)
{
    scia_init();
    adc_init();
    scia_send_str("\r\nADC test ready. Sampling ADCINA0...\r\n");
}

void test_adc_loop(void)
{
    Uint16 raw = adc_read();
    Uint16 mv  = (Uint16)((Uint32)raw * ADC_VREF_MV / 4095UL);

    char line[48];
    Uint16 idx = 0;

    line[idx++] = 'r';
    line[idx++] = 'a';
    line[idx++] = 'w';
    line[idx++] = '=';
    put_u16(line, &idx, raw);

    line[idx++] = ' ';
    line[idx++] = 'V';
    line[idx++] = '=';
    put_u16(line, &idx, mv / 1000);
    line[idx++] = '.';
    put_u16(line, &idx, (mv % 1000) / 100);
    put_u16(line, &idx, (mv % 100) / 10);
    put_u16(line, &idx, mv % 10);

    line[idx++] = '\r';
    line[idx++] = '\n';
    line[idx]   = '\0';

    scia_send_str(line);
    DELAY_MS(500);
}