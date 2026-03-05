#ifndef __GENTIMER_H__
#define __GENTIMER_H__

#include "driver/gptimer.h"
#include "esp_attr.h"
#include "led.h"

extern gptimer_handle_t gentimer_handle;

void gentimer_init(void);
void gentimer_set_alarm(void);
void gentimer_callback_register(void *user_data);
void gentimer_enable_and_start(void);

#endif // __GENTIMER_H__    
