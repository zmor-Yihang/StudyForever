#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "xl9555.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    uint8_t key_state = 1;      // 当前按键状态，初始为高电平（未按下）
    uint8_t key_last_state = 1; // 上一次按键状态
    uint8_t led_state = 0;      // LED状态，0为关闭，1为开启

    ESP_LOGI(TAG, "开始初始化硬件...");

    // 初始化硬件
    led_init();
    
    // 初始化XL9555并检查结果
    esp_err_t ret = xl9555_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "XL9555初始化失败!");
        
        // 在错误情况下闪烁LED指示错误
        while (1) {
            led_on();
            vTaskDelay(pdMS_TO_TICKS(200));
            led_off();
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
    
    // 配置P1.7为输入模式（按键）
    ret = xl9555_set_mode(1, 0xFF); // 设置P1端口全部为输入模式
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "配置XL9555端口模式失败!");
    }
    
    ESP_LOGI(TAG, "硬件初始化完成，开始主循环...");

    while (1)
    {
        // 读取按键状态 (P1.7)
        xl9555_read_pin(1, 7, &key_state);
        
        // 检测按键按下（下降沿触发）
        if (key_last_state == 1 && key_state == 0)
        {
            ESP_LOGI(TAG, "检测到按键按下!");
            
            // 按键被按下，翻转LED状态
            led_state = !led_state;
            
            if (led_state)
            {
                led_on();   // 点亮LED
                ESP_LOGI(TAG, "LED已点亮");
            }
            else
            {
                led_off();  // 熄灭LED
                ESP_LOGI(TAG, "LED已熄灭");
            }
            
            // 简单延时防抖
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // 更新按键状态
        key_last_state = key_state;
        
        // 任务延时，避免占用过多CPU资源
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
