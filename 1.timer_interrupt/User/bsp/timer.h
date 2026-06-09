#ifndef __TIMER_H
#define __TIMER_H

#include "DSP28x_Project.h"

typedef void (*timer_cb_t)(void);

void timer0_init(void);
void timer0_start(void);
void timer0_stop(void);
void timer0_reload(void);
Uint32 timer0_get_counter(void);
Uint32 timer0_get_period(void);

void timer0_irq_enable(void);
void timer0_register_isr_callback(timer_cb_t cb);

#endif
