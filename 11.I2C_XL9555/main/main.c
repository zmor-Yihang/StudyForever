#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "xl9555.h"

static const char *TAG = "main"; 

void app_main(void)
{
    uint8_t key_state = 1;      // 当前按键状态，初始为高电平（未按下）
    uint8_t key_last_state = 1; // 上一次按键状态
    uint8_t led_state = 0;      // LED状态，0为关闭，1为开启

    // 初始化硬件
    led_init();
    xl9555_init();
    
    // 配置P1.7为输入模式（按键）
    xl9555_set_pin_mode(1, 7, 1); 

    while (1)
    {
        // 读取按键状态 (P1.7)
        xl9555_read_pin(1, 7, &key_state);
        
        // 检测按键按下
        if (key_last_state == 1 && key_state == 0)
        {
            // 按键被按下，翻转LED状态
            led_state = !led_state;
            
            if (led_state)
            {
                led_on();   // 点亮LED
                ESP_LOGI(TAG, "LED ON");
            }
            else
            {
                led_off();  // 熄灭LED
                ESP_LOGI(TAG, "LED OFF");
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
