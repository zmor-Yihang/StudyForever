#include "main.h"

static const char *TAG = "main";

void app_main(void)
{
    wifi_init();
    vTaskDelay(pdMS_TO_TICKS(10000));
    wifi_disconnect();
    ESP_LOGI(TAG, "WiFi 断开命令已发送");
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "主任务运行中...");
    }
}
