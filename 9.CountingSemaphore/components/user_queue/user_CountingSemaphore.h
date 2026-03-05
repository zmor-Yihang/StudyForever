#ifndef USE_COUNTING_SEMAPHORE_H__
#define USE_COUNTING_SEMAPHORE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t counting_semaphore_handle;

void counting_semaphore_init(void);

#endif /* USE_COUNTING_SEMAPHORE_H__ */
