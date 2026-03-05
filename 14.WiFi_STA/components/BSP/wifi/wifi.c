#include "wifi.h"

static const char *TAG = "WiFi";

static int s_retry_num = 0; /* < WiFi 重连计数器，记录当前重连次数 */

static bool s_manual_disconnect = false; /* 主动断开标志位，用于区分主动断开和意外断开 */

/**
 * @brief WiFi 和 IP 事件处理器
 * @details 处理 WiFi 状态变化、连接/断开、IP 获取等事件
 *          运行在 FreeRTOS 事件处理任务中
 *
 * @param arg 用户参数（未使用）
 * @param event_base 事件基础类型（WIFI_EVENT 或 IP_EVENT）
 * @param event_id 具体事件 ID
 * @param event_data 事件数据指针
 *
 * @note 该函数会被事件循环自动调用，不应手动调用
 */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    /* 处理 WiFi STA 启动事件 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "WiFi初始化完成, 正在连接...");
        wifi_connect(); // WiFi 启动后立即尝试连接
    }
    /* 处理 WiFi 断开连接事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        /* 判断是否为主动断开 */
        if (s_manual_disconnect)
        {
            ESP_LOGI(TAG, "已手动断开 WiFi 连接");
            s_manual_disconnect = false; // 重置主动断开标志
            s_retry_num = 0;             // 重置重连计数器
            return;                      // 主动断开不自动重连
        }

        /* 意外断开，尝试自动重连机制 */
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect(); // 发起重连
            s_retry_num++;      // 增加重连计数
            ESP_LOGI(TAG, "WiFi 意外断开，重新连接... 重试次数: %d", s_retry_num);
        }
        else
        {
            /* 超过最大重试次数，停止重连 */
            ESP_LOGE(TAG, "达到最大重试次数，停止尝试连接");
        }
    }
    /* 处理获取 IP 地址事件 */
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        /* 获取 IP 地址信息并打印 */
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "获取到 IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0; // 连接成功，重置重连计数器

        /* 连接成功后扫描周围的 WiFi 网络 */
        ESP_LOGI(TAG, "开始扫描周围的 WiFi 网络...");
        wifi_scan(); // 扫描会阻塞事件处理任务
    }
}

/**
 * @brief 初始化 WiFi 并连接到配置的网络
 * @details 完成以下步骤：
 *          1. 初始化 NVS（用于存储 WiFi 配置）
 *          2. 初始化网络接口（ESP-NETIF）
 *          3. 创建事件循环
 *          4. 创建 WiFi STA 接口
 *          5. 初始化 WiFi 驱动
 *          6. 注册事件处理器
 *          7. 配置 WiFi 参数（SSID、密码等）
 *          8. 启动 WiFi
 * @note 该函数是异步的，启动后会立即返回
 *       WiFi 连接结果通过事件处理器异步通知
 * @warning 必须在使用其他 WiFi 功能前调用此函数
 */
void wifi_init(void)
{
    /* 用于在 Flash 中存储 WiFi 配置信息，如已保存的 SSID 和密码 */
    nvs_flash_init();

    /* ESP-NETIF 是网络接口抽象层，管理 WiFi、以太网等网络接口
     * 为上层协议栈（如 LwIP）提供统一的接口 */
    esp_netif_init();

    /* 事件循环用于处理系统事件（WiFi 连接、断开、IP 获取等）
     * 所有 WiFi 相关事件都通过这个循环分发 */
    esp_event_loop_create_default();

    /* STA (Station) 模式：ESP32 作为客户端连接到路由器 */
    esp_netif_create_default_wifi_sta();

    /* 使用默认配置初始化 WiFi 底层驱动 */
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);

    /* - WIFI_EVENT: 处理所有 WiFi 事件（启动、连接、断开等）
     * - IP_EVENT: 处理 IP 获取事件 */
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL);

    /* 配置 WiFi 连接参数 */
    wifi_config_t sta_config = {0};
    strcpy((char *)sta_config.sta.ssid, WIFI_SSID);         // 目标网络 SSID
    strcpy((char *)sta_config.sta.password, WIFI_PASSWORD); // 网络密码
    sta_config.sta.threshold.rssi = -70;                    // RSSI 阈值（信号强度）
    sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; // 最低加密方式要求

    /* 设置 WiFi 工作模式为 STA（客户端模式）*/
    esp_wifi_set_mode(WIFI_MODE_STA);

    /* 应用 WiFi 配置 */
    esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config);

    /* 启动后会触发 WIFI_EVENT_STA_START 事件，在事件处理器中发起连接 */
    esp_wifi_start();
}

/**
 * @brief 扫描周围的 WiFi 网络
 * @details 使用阻塞模式扫描所有可见的 WiFi 网络（包括隐藏网络）
 *          扫描完成后自动打印结果
 *
 * @note 该函数会阻塞当前任务约 3 秒（扫描所有信道的时间）
 *       如果在事件处理器中调用，会阻塞事件处理任务，不影响其他任务
 * @warning 必须在 WiFi 初始化完成后才能调用
 */
void wifi_scan(void)
{
    /* 配置扫描参数 */
    wifi_scan_config_t scan_config = {0};
    scan_config.ssid = NULL;                       /* NULL = 扫描所有 SSID */
    scan_config.bssid = NULL;                      /* NULL = 不指定特定路由器 MAC */
    scan_config.channel = 0;                       /* 0 = 扫描所有信道 (2.4GHz: 1-13) */
    scan_config.channel_bitmap.ghz_2_channels = 0; /* 2.4GHz 信道位图，0 = 全部 */
    scan_config.channel_bitmap.ghz_5_channels = 0; /* 5GHz 信道位图，0 = 全部 */
    scan_config.coex_background_scan = true;       /* 允许后台扫描（不影响已连接的 WiFi）*/
    scan_config.home_chan_dwell_time = 0;          /* 主信道停留时间，0 = 使用默认值 */
    scan_config.scan_time.active.min = 100;        /* 主动扫描每个信道的最小时间 (ms) */
    scan_config.scan_time.active.max = 200;        /* 主动扫描每个信道的最大时间 (ms) */
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE; /* 主动扫描：发送探测请求 */
    scan_config.show_hidden = true;                /* 包含隐藏的 WiFi 网络 */

    /* true = 阻塞模式：函数会等待扫描完成后才返回
     * false = 非阻塞模式：函数立即返回，通过 WIFI_EVENT_SCAN_DONE 事件通知完成 */
    esp_wifi_scan_start(&scan_config, true);

    /* 扫描完成，打印结果 */
    wifi_print_scan_results();
}

/**
 * @brief 打印 WiFi 扫描结果
 * @details 格式化输出扫描到的所有 WiFi 网络信息，包括：
 *          - SSID (网络名称)
 *          - BSSID (MAC 地址)
 *          - 信道 (Channel)
 *          - RSSI (信号强度)
 *          - 加密方式 (AuthMode)
 *
 * @note 必须在 wifi_scan() 阻塞模式扫描完成后调用
 *       或在 WIFI_EVENT_SCAN_DONE 事件中调用
 */
void wifi_print_scan_results(void)
{
    /* 获取扫描到的网络数量 */
    uint16_t number = 0;
    esp_wifi_scan_get_ap_num(&number);
    ESP_LOGI(TAG, "扫描完成，找到 %d 个网络", number);

    /* 动态分配内存存储 AP 记录 */
    wifi_ap_record_t *ap_info = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * number);
    if (ap_info != NULL)
    {
        /* 获取所有 AP 记录 */
        esp_wifi_scan_get_ap_records(&number, ap_info);

        /* 打印表格标题 */
        ESP_LOGI(TAG, "========== WiFi 扫描结果 ==========");
        ESP_LOGI(TAG, "%-32s %-17s %-10s %-6s %-16s", "SSID", "MAC地址", "信道", "RSSI", "加密方式");

        /* 遍历并打印每个 AP 的信息 */
        for (int i = 0; i < number; i++)
        {
            /* 将加密方式枚举转换为字符串 */
            const char *auth_mode;
            switch (ap_info[i].authmode)
            {
            case WIFI_AUTH_OPEN: // 无加密（不安全）
                auth_mode = "OPEN";
                break;
            case WIFI_AUTH_WEP: // WEP 加密（已过时）
                auth_mode = "WEP";
                break;
            case WIFI_AUTH_WPA_PSK: // WPA 加密
                auth_mode = "WPA_PSK";
                break;
            case WIFI_AUTH_WPA2_PSK: // WPA2 加密（常用）
                auth_mode = "WPA2_PSK";
                break;
            case WIFI_AUTH_WPA_WPA2_PSK: // WPA/WPA2 混合模式
                auth_mode = "WPA/WPA2_PSK";
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE: // WPA2 企业级加密
                auth_mode = "WPA2_ENTERPRISE";
                break;
            case WIFI_AUTH_WPA3_PSK: // WPA3 加密（最新）
                auth_mode = "WPA3_PSK";
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK: // WPA2/WPA3 混合模式
                auth_mode = "WPA2/WPA3_PSK";
                break;
            default: // 未知加密方式
                auth_mode = "UNKNOWN";
                break;
            }

            /* 格式化打印 AP 信息 */
            ESP_LOGI(TAG, "%-32s %02X:%02X:%02X:%02X:%02X:%02X %-10d %-6d %-16s",
                     (char *)ap_info[i].ssid, // SSID (网络名称)
                     ap_info[i].bssid[0], ap_info[i].bssid[1], ap_info[i].bssid[2],
                     ap_info[i].bssid[3], ap_info[i].bssid[4], ap_info[i].bssid[5], // MAC 地址
                     ap_info[i].primary,                                            // 信道号
                     ap_info[i].rssi,                                               // 信号强度 (dBm，越接近 0 越好)
                     auth_mode);                                                    // 加密方式
        }

        /* 释放动态分配的内存 */
        free(ap_info);
    }
    else
    {
        ESP_LOGE(TAG, "内存分配失败");
    }
}

/**
 * @brief 断开 WiFi 连接
 * @details 主动断开当前的 WiFi 连接，不会触发自动重连机制
 *
 * @note 断开后会触发 WIFI_EVENT_STA_DISCONNECTED 事件
 *       但由于设置了 s_manual_disconnect 标志，不会自动重连
 */
void wifi_disconnect(void)
{
    s_manual_disconnect = true; // 设置主动断开标志，防止自动重连
    esp_wifi_disconnect();      // 调用 ESP-IDF API 断开连接
    ESP_LOGI(TAG, "正在断开 WiFi 连接...");
}

/**
 * @brief 连接到 WiFi 网络
 * @details 使用 wifi_init() 中配置的 SSID 和密码连接到 WiFi
 *
 * @note 连接是异步的，结果通过事件通知：
 *       - 成功: IP_EVENT_STA_GOT_IP
 *       - 失败: WIFI_EVENT_STA_DISCONNECTED
 */
void wifi_connect(void)
{
    esp_wifi_connect(); // 调用 ESP-IDF API 发起连接
}

/**
 * @brief 获取并打印设备的 MAC 地址
 * @details 获取 WiFi STA 接口的 MAC 地址并通过日志输出
 *
 * @note MAC 地址是设备的唯一硬件标识符
 *       每个 ESP32 设备的 MAC 地址都不同
 */
void wifi_get_mac(void)
{
    uint8_t mac[6];                         // MAC 地址数组 (6 字节)
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac); // 获取 STA 接口的 MAC 地址
    ESP_LOGI(TAG, "设备 MAC 地址: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

