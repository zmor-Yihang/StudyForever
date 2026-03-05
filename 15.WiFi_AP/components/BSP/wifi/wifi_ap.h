#ifndef WIFI_AP_H__
#define WIFI_AP_H__

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

// WiFi 配置
#define WIFI_SSID "ESP32_WiFi_AP"
#define WIFI_PASSWORD "123654789"

void wifiap_init(void);

#endif  /* WIFI_AP_H__ */
