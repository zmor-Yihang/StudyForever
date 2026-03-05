#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gentimer.h"

void app_main(void)
{
    // queue_init();
    // task_init();
    led_init();
    gentimer_init();
    gentimer_set_alarm();
    gentimer_callback_register(NULL);
    gentimer_enable_and_start();
    vTaskDelete(NULL); // 删除主任务main
}
