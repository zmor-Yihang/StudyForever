#ifndef __LED_H__
#define __LED_H__

#include "DSP28x_Project.h"

typedef enum {
    LED_D8 = 0,
    LED_D9,
    LED_D10,
    LED_D11,
    LED_D12,
    LED_D13,
    LED_D14
} led_id_t;

void led_init(led_id_t led);
void led_on(led_id_t led);
void led_off(led_id_t led);
void led_toggle(led_id_t led);

#endif
