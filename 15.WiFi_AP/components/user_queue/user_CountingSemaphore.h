#ifndef USE_COUNTING_SEMAPHORE_H__
#define USE_COUNTING_SEMAPHORE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t CountingSemaphore_handle;

void CountingSemaphore_init(void);

void CountingSemaphore_take(void);

void CountingSemaphore_give(void);

void CountingSemaphore_delete(void);

UBaseType_t CountingSemaphore_getCount(void);

#endif /* USE_COUNTING_SEMAPHORE_H__ */
