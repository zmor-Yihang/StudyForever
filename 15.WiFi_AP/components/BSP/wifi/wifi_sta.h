#ifndef __WIFI_STA_H__
#define __WIFI_STA_H__

#include "esp_wifi.h"
#include "string.h"
#include "stdlib.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

// WiFi 配置
#define WIFI_SSID "ESP32_WiFi_AP"
#define WIFI_PASSWORD "123654789"

// 最大重试次数
#define EXAMPLE_ESP_MAXIMUM_RETRY  10

void wifista_init(void);
void wifista_scan(void);
void wifista_print_scan_results(void);
void wifista_connect(void);
void wifista_disconnect(void);
void wifista_get_mac(void);

#endif  /* __WIFI_STA_H__ */
