#include "user_CountingSemaphore.h"

SemaphoreHandle_t counting_semaphore_handle = NULL;

void counting_semaphore_init(void)
{
    /* 创建计数信号量，最大计数为2，初始计数为0 */
    counting_semaphore_handle = xSemaphoreCreateCounting(2, 0);
}