#ifndef __BTIM_H__
#define __BTIM_H__ 

#include "./SYSTEM/sys/sys.h"

extern TIM_HandleTypeDef htim3;

void TIM_PWM_Init(void);
void TIM_PWM_SetDuty(uint16_t duty);

#endif
