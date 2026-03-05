#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"

void LED_Init(void);
void LED_Toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x);
void LED_On(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x);
void LED_Off(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x);

#endif
