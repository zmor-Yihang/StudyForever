#include "main.h"

static const char *TAG = "main";

void app_main(void)
{
    uint8_t rx_buffer[256];
    wifiap_init();                    /* 初始化 WiFi AP */
    vTaskDelay(pdMS_TO_TICKS(20000)); /* 20s确保手机连接热点 */

    tcp_server_init();              /* ESP32 作为服务器端监听手机连接, 这里会阻塞直到客户端成功连接 */
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (1)
    {
        /* 定期发送数据到客户端 */
        tcp_server_send_data(client_sock, (uint8_t *)"Hello Phone!\n");
        vTaskDelay(pdMS_TO_TICKS(2000));

        /* 接收手机发来的数据 */
        int len = tcp_server_receive_data(client_sock, rx_buffer, sizeof(rx_buffer));

        ESP_LOGI(TAG, "Received from phone (%d bytes): %s", len, rx_buffer);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}