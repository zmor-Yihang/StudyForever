#include "wifi_ap.h"

static const char *TAG = "WiFi_AP";

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    /* 处理 WiFi AP 启动事件 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START)
    {
        ESP_LOGI(TAG, "WiFi AP 启动成功");
    }
    /* 处理 WiFi AP 断开连接事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        ESP_LOGI(TAG, "WiFi AP 断开连接");
    }
    /* 处理分配 IP 地址事件 */    
    else if (event_base == IP_EVENT && event_id == IP_EVENT_AP_STAIPASSIGNED)
    {
        ip_event_ap_staipassigned_t *event = (ip_event_ap_staipassigned_t *)event_data;
        ESP_LOGI(TAG, "客户端已连接，分配 IP: " IPSTR, IP2STR(&event->ip));
    }
}

void wifiap_init(void)
{
    /* 用于在 Flash 中存储 WiFi 配置信息，如已保存的 SSID 和密码 */
    nvs_flash_init();

    /* ESP-NETIF 是网络接口抽象层，管理 WiFi、以太网等网络接口
     * 为上层协议栈（如 LwIP）提供统一的接口 */
    esp_netif_init();

    /* 事件循环用于处理系统事件（WiFi 连接、断开、IP 获取等）
     * 所有 WiFi 相关事件都通过这个循环分发 */
    esp_event_loop_create_default();

    /* AP 模式：ESP32 作为接入点(路由器) */
    esp_netif_create_default_wifi_ap();

    /* 使用默认配置初始化 WiFi 底层驱动 */
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);

    /* - WIFI_EVENT: 处理所有 WiFi 事件（启动、连接、断开等）
     * - IP_EVENT: 处理 IP 获取事件 */
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &event_handler, NULL, NULL);

    /* 配置 WiFi AP 参数（只需设置核心参数，其他使用默认值）*/
    wifi_config_t ap_config = {0};
    strcpy((char *)ap_config.ap.ssid, WIFI_SSID);         // AP 名称
    strcpy((char *)ap_config.ap.password, WIFI_PASSWORD); // AP 密码
    ap_config.ap.ssid_len = strlen(WIFI_SSID);            // SSID 长度
    ap_config.ap.channel = 1;                             // WiFi 信道（1-13）
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;           // 加密方式（WPA2）
    ap_config.ap.max_connection = 4;                      // 最大连接设备数

    /* 设置 WiFi 工作模式为 AP（接入点模式）*/
    esp_wifi_set_mode(WIFI_MODE_AP);

    /* 应用 WiFi 配置 */
    esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config);

    /* 启动后会触发 WIFI_EVENT_AP_START 事件，在事件处理器中发起连接 */
    esp_wifi_start();
}