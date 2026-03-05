#include "main.h"

static const char *TAG = "main";

void app_main(void)
{
    wifiap_init(); // 初始化 WiFi AP 模式
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "主任务运行中...");
    }
}
