#ifndef __BTIM_H__
#define __BTIM_H__ 

#include "./SYSTEM/sys/sys.h"

extern TIM_HandleTypeDef h_pwm_tim;

void GTIM_PWM_Init(void);
void GTIM_PWM_SetDuty(uint16_t duty);


extern TIM_HandleTypeDef h_ic_tim;

#endif
