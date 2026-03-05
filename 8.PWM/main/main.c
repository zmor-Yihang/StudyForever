#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pwm.h"


void app_main(void)
{
    // queue_init();
    // task_init();
    led_init();
    pwm_init();
    pwm_fade();

    vTaskDelete(NULL); // 删除主任务main
}
