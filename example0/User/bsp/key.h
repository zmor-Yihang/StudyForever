#ifndef __KEY_H__
#define __KEY_H__

#include "DSP28x_Project.h"

typedef enum {
    KEY_NONE = 0,
    KEY_SW1,
    KEY_SW2,
    KEY_SW3,
    KEY_SW4,
    KEY_SW5,
    KEY_SW6,
    KEY_SW7,
    KEY_SW8,
    KEY_SW9
} key_id_t;

void key_init(void);
key_id_t key_scan(void);

#endif
