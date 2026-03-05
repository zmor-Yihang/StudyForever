#include "task2.h"

/* 任务句柄 */
TaskHandle_t task2_handle = NULL;

void task2(void *pvParameters)
{
    while (1)
    {
        MutexSemaphore_take();
        vUtilsDelayWithoutTaskSwitch(100); 
        printf("Task2 is running\n");
        led_off();

        MutexSemaphore_give();
    }
}
