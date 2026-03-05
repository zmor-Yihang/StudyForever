#include "task1.h"

/* 任务句柄 */
TaskHandle_t task1_handle = NULL;

void task1(void *pvParameters)
{
    while (1)
    {
        MutexSemaphore_take();
        utils_delay(100); // 延时1秒，模拟任务处理时间
        printf("Task1 is running\n");
        led_on();
        
        MutexSemaphore_give();
    }
}
