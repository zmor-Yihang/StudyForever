#ifndef __ADC_H__
#define __ADC_H__ 

#include "./SYSTEM/sys/sys.h"

extern ADC_HandleTypeDef hadc1;

void ADC_Init(void);
uint16_t ADC_GetValue(void);

#endif
