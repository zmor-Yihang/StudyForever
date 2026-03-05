#ifndef __ADC_H__
#define __ADC_H__ 

#include "./SYSTEM/sys/sys.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma1;

extern uint16_t buffer[100];

void ADC_Init(void);

#endif
