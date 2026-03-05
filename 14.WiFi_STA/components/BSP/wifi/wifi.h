#ifndef __WIFI_H__
#define __WIFI_H__

#include "esp_wifi.h"
#include "string.h"
#include "stdlib.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

// WiFi 配置
#define WIFI_SSID "zmor"
#define WIFI_PASSWORD "123654789"

// 最大重试次数
#define EXAMPLE_ESP_MAXIMUM_RETRY  10

void wifi_init(void);
void wifi_scan(void);
void wifi_print_scan_results(void);
void wifi_connect(void);
void wifi_disconnect(void);

void wifi_get_mac(void);

#endif  /* __WIFI_H__ */
