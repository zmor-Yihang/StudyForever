#ifndef __TIMER_H
#define __TIMER_H

#include "DSP28x_Project.h"

typedef void (*timer_cb_t)(void);

void timer0_init(void);
void timer0_start(void);
void timer0_stop(void);
void timer0_reload(void);
void timer0_set_callback(timer_cb_t cb);
void timer0_irq_enable(void);
Uint32 timer0_read_counter(void);
Uint32 timer0_read_period(void);

__interrupt void timer0_isr(void);

#endif
