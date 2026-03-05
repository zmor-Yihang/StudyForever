#ifndef __KEY_H__
#define __KEY_H__ 

#include "./sys/sys.h"

#define KEY_PRESS (uint16_t)1


void KEY_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x);
uint16_t KEY_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x);

#endif
