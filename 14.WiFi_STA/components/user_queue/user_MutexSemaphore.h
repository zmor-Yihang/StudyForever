#ifndef USER_MUTEX_SEMAPHORE_H
#define USER_MUTEX_SEMAPHORE_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* 互斥信号量句柄 */
extern SemaphoreHandle_t MutexSemaphore_handle;

void MutexSemaphore_init(void);

void MutexSemaphore_take(void);

void MutexSemaphore_give(void);

void MutexSemaphore_delete(void);

TaskHandle_t MutexSemaphore_getMutexHolder(void);

#endif // USER_MUTEX_SEMAPHORE_H
