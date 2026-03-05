#ifndef __PWM_H__
#define __PWM_H__

#include "driver/ledc.h"
#include "led.h"

void pwm_init(void);

void pwm_fade(void);

#endif /* __PWM_H__ */
