#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "user_task.h"
#include "user_MutexSemaphore.h"

void app_main(void)
{
    led_init();
    
    MutexSemaphore_init();
    
    task_init();

    vTaskDelete(NULL); // 删除主任务main
}
