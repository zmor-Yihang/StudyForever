#ifndef __UTILS_H__
#define __UTILS_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void vUtilsDelayWithoutTaskSwitch(TickType_t xDelayTicks);

#endif // __UTILS_H__
