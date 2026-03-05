#include "task2.h"

/* 任务句柄 */
TaskHandle_t task2_handle = NULL;

void task2(void *pvParameters)
{
    while (1)
    {
        /* 获取计数信号量，超时时间1000ms */
        if (xSemaphoreTake(counting_semaphore_handle, 1000) == pdTRUE)
        {
            led_off();
            printf("Task2 get semaphore...\n");
            vUtilsDelayWithoutTaskSwitch(500); // 使用忙等待延时，防止任务切换
            xSemaphoreGive(counting_semaphore_handle); /* 释放计数信号量 */
        }
        else
        {
            printf("Task2 failed to get semaphore (timeout)...\n");
            xSemaphoreGive(counting_semaphore_handle); /* 释放计数信号量 */
        }
        vTaskDelay(2000);
    }
}
