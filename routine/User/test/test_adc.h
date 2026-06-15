#ifndef __TEST_ADC_H__
#define __TEST_ADC_H__

//
// ADC 测试用例
// init: 初始化 ADC(ADCINA0) 与 SCI-A，发送启动提示
// loop: 周期性采集 ADCINA0，将原始值与换算电压通过串口打印
//
void test_adc_init(void);
void test_adc_loop(void);

#endif