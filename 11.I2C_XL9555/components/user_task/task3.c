#include "task3.h"

TaskHandle_t task3_handle = NULL;

void task3(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(100); // 添加延时，让出CPU给IDLE任务
    }
}
