# ESP32 Bluetooth API 完整函数参考

## 文件说明

ESP32的蓝牙功能由蓝牙控制器（Controller）和蓝牙主机（Host）两部分组成。

### 包含的头文件
- **esp_bt.h** - 蓝牙控制器核心API
- **esp_bt_vs.h** - 蓝牙供应商特定（Vendor Specific）HCI命令和事件定义

---

## 0. 常用数据结构

### esp_bt_controller_config_t

蓝牙控制器配置结构体（主要参数）：

```c
typedef struct {
    uint16_t controller_task_stack_size;    // 控制器任务栈大小（字节）
    uint8_t controller_task_prio;           // 控制器任务优先级
    uint8_t hci_uart_no;                    // HCI UART编号（1或2）
    uint32_t hci_uart_baudrate;             // HCI UART波特率（115200-921600）
    uint8_t scan_duplicate_mode;            // 扫描重复过滤模式（0-普通，1-BLE Mesh）
    uint8_t scan_duplicate_type;            // 扫描重复过滤类型（0-仅地址，1-仅数据，2-地址和数据）
    uint16_t normal_adv_size;               // 普通广播扫描重复过滤列表最大设备数（10-1000）
    uint16_t mesh_adv_size;                 // Mesh广播扫描重复过滤列表最大数（10-1000）
    uint16_t send_adv_reserved_size;        // 控制器最小内存值（内部使用）
    uint32_t controller_debug_flag;         // 控制器调试日志标志（内部使用）
    uint8_t mode;                           // 控制器模式（1-BLE，2-经典蓝牙，3-双模）
    uint8_t ble_max_conn;                   // BLE最大连接数（1-9，默认3）
    uint8_t bt_max_acl_conn;                // BR/EDR最大ACL连接数（1-7，默认2）
    uint8_t bt_sco_datapath;                // SCO数据路径（0-HCI，1-PCM）
    bool auto_latency;                      // BLE自动延迟（双模下增强经典蓝牙性能）
    bool bt_legacy_auth_vs_evt;             // BR/EDR传统认证供应商特定事件
    uint8_t bt_max_sync_conn;               // BR/EDR最大同步连接数（0-3，默认0）
    uint8_t ble_sca;                        // BLE低功耗晶振精度索引（0-500ppm，1-250ppm）
    uint8_t pcm_role;                       // PCM角色（0-主机，1-从机）
    uint8_t pcm_polar;                      // PCM极性（0-下降沿，1-上升沿）
    bool hli;                               // 高级链路接口
    uint8_t enc_key_sz_min;                 // 加密密钥最小大小
    uint16_t dup_list_refresh_period;       // 重复列表刷新周期
    bool ble_scan_backoff;                  // BLE扫描退避
    uint8_t ble_llcp_disc_flag;             // BLE LLCP断开标志
    bool ble_aa_check;                      // BLE访问地址检查
    uint8_t ble_chan_ass_en;                // BLE信道评估使能
    uint8_t ble_ping_en;                    // BLE Ping使能
    uint32_t magic;                         // 魔术值（ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL）
} esp_bt_controller_config_t;
```

### esp_vhci_host_callback_t

VHCI回调函数结构体：

```c
typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               // 通知主机可以发送数据
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   // 通知主机接收到数据
} esp_vhci_host_callback_t;
```

## 1. 蓝牙控制器初始化和配置函数

### esp_bt_controller_init
**函数原型：**
```c
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);
```
**参数：**
cfg：指向蓝牙控制器初始配置结构体的指针，建议使用BT_CONTROLLER_INIT_CONFIG_DEFAULT()宏
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NO_MEM：内存不足
**作用：**初始化蓝牙控制器，分配任务和资源（此函数应只调用一次，在所有其他蓝牙函数之前）

### esp_bt_controller_deinit
**函数原型：**
```c
esp_err_t esp_bt_controller_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
**作用：**反初始化蓝牙控制器，释放资源并删除任务

### esp_bt_controller_enable
**函数原型：**
```c
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode);
```
**参数：**
mode：蓝牙控制器模式（ESP_BT_MODE_BLE-仅BLE, ESP_BT_MODE_CLASSIC_BT-仅经典蓝牙, ESP_BT_MODE_BTDM-双模）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
ESP_ERR_INVALID_ARG：参数无效
**作用：**启用蓝牙控制器（必须先初始化）

### esp_bt_controller_disable
**函数原型：**
```c
esp_err_t esp_bt_controller_disable(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
**作用：**禁用蓝牙控制器

### esp_bt_controller_get_status
**函数原型：**
```c
esp_bt_controller_status_t esp_bt_controller_get_status(void);
```
**参数：**
无
**返回值：**
ESP_BT_CONTROLLER_STATUS_IDLE：控制器未初始化或已反初始化
ESP_BT_CONTROLLER_STATUS_INITED：控制器已初始化但未启用或已禁用
ESP_BT_CONTROLLER_STATUS_ENABLED：控制器已初始化并启用
**作用：**获取蓝牙控制器当前状态

## 2. 蓝牙控制器内存管理函数

### esp_bt_controller_mem_release
**函数原型：**
```c
esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode);
```
**参数：**
mode：蓝牙控制器模式（指定要释放哪种模式的内存）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
ESP_ERR_NOT_FOUND：未找到请求的资源
**作用：**根据模式释放控制器内存（BSS、数据段等），总大小约70KB（此过程不可逆）

### esp_bt_mem_release
**函数原型：**
```c
esp_err_t esp_bt_mem_release(esp_bt_mode_t mode);
```
**参数：**
mode：蓝牙控制器模式
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
ESP_ERR_NOT_FOUND：未找到请求的资源
**作用：**释放控制器和主机栈的内存（BSS和数据段）

## 3. 蓝牙低功耗模式函数

### esp_bt_sleep_enable
**函数原型：**
```c
esp_err_t esp_bt_sleep_enable(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
ESP_ERR_NOT_SUPPORTED：不支持的操作或功能
**作用：**启用蓝牙调制解调器睡眠模式（有ORIG模式和EVED模式两种）

### esp_bt_sleep_disable
**函数原型：**
```c
esp_err_t esp_bt_sleep_disable(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
ESP_ERR_NOT_SUPPORTED：不支持调制解调器睡眠模式
**作用：**禁用蓝牙调制解调器睡眠模式

### esp_bt_get_lpclk_src
**函数原型：**
```c
esp_bt_sleep_clock_t esp_bt_get_lpclk_src(void);
```
**参数：**
无
**返回值：**
蓝牙低功耗模式使用的时钟源
**作用：**获取蓝牙模块睡眠时钟源

### esp_bt_set_lpclk_src
**函数原型：**
```c
esp_err_t esp_bt_set_lpclk_src(esp_bt_sleep_clock_t lpclk);
```
**参数：**
lpclk：蓝牙睡眠时钟源
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数无效
**作用：**设置蓝牙模块睡眠时钟源（应在esp_bt_controller_init之前调用）

## 4. BLE发射功率控制函数

### esp_ble_tx_power_set
**函数原型：**
```c
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level);
```
**参数：**
power_type：发射功率类型（广播/连接/默认等）
power_level：功率级别（索引），对应绝对值（dBm）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数无效
**作用：**设置BLE发射功率（连接发射功率应在建立连接后设置）

### esp_ble_tx_power_get
**函数原型：**
```c
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);
```
**参数：**
power_type：发射功率类型
**返回值：**
功率级别
**作用：**获取BLE发射功率（连接发射功率应在建立连接后获取）

### esp_ble_scan_duplicate_list_flush
**函数原型：**
```c
esp_err_t esp_ble_scan_duplicate_list_flush(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
**作用：**手动清除BLE扫描重复列表

### esp_ble_scan_dupilcate_list_flush
**函数原型：**
```c
esp_err_t esp_ble_scan_dupilcate_list_flush(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
**作用：**与esp_ble_scan_duplicate_list_flush相同（拼写错误的版本，将在5.x版本修复）

## 5. BR/EDR（经典蓝牙）发射功率控制函数

### esp_bredr_tx_power_set
**函数原型：**
```c
esp_err_t esp_bredr_tx_power_set(esp_power_level_t min_power_level, esp_power_level_t max_power_level);
```
**参数：**
min_power_level：最小功率级别（默认为ESP_PWR_LVL_N0）
max_power_level：最大功率级别（默认为ESP_PWR_LVL_P3）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
**作用：**设置BR/EDR发射功率范围

### esp_bredr_tx_power_get
**函数原型：**
```c
esp_err_t esp_bredr_tx_power_get(esp_power_level_t *min_power_level, esp_power_level_t *max_power_level);
```
**参数：**
min_power_level：存储最小功率级别的指针
max_power_level：存储最大功率级别的指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取BR/EDR发射功率范围

### esp_bredr_sco_datapath_set
**函数原型：**
```c
esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path);
```
**参数：**
data_path：SCO数据路径（ESP_SCO_DATA_PATH_HCI或ESP_SCO_DATA_PATH_PCM）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：蓝牙控制器状态无效
**作用：**设置BR/EDR默认SCO数据路径（应在控制器启用后、(e)SCO链接建立前调用）

## 6. VHCI（虚拟HCI）接口函数

### esp_vhci_host_check_send_available
**函数原型：**
```c
bool esp_vhci_host_check_send_available(void);
```
**参数：**
无
**返回值：**
true：控制器准备好接收HCI数据
false：控制器未准备好
**作用：**检查控制器是否准备好从主机接收HCI数据

### esp_vhci_host_send_packet
**函数原型：**
```c
void esp_vhci_host_send_packet(uint8_t *data, uint16_t len);
```
**参数：**
data：指向HCI数据的指针
len：HCI数据长度
**返回值：**
无
**作用：**向控制器发送HCI数据（应先调用esp_vhci_host_check_send_available检查）

### esp_vhci_host_register_callback
**函数原型：**
```c
esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback);
```
**参数：**
callback：esp_vhci_host_callback_t类型的回调函数结构体
**返回值：**
ESP_OK：操作成功
ESP_FAIL：操作失败
**作用：**注册VHCI回调函数，用于通知主机下一步操作

## 供应商特定HCI命令（esp_bt_vs.h）

### 用于Espressif蓝牙主机的VS命令

#### ESP_BT_VS_COMMON_ECHO_OCF (0x0081)
**作用：**测试供应商HCI功能，控制器返回命令中的值
**参数结构：**bt_hci_vs_common_echo
**初始化函数：**bt_stack_enableEchoVsCmd(true)

#### ESP_BT_VS_SET_COEX_STATUS_OCF (0x0082)
**作用：**设置/清除共存状态
**参数结构：**bt_hci_vs_set_coex_status
**初始化函数：**bt_stack_enableCoexVsCmd(true)

#### ESP_BT_VS_CONFIG_DUP_EXC_LIST_OCF (0x0108)
**作用：**配置扫描重复异常列表
**参数结构：**bt_hci_vs_update_exc_list
**初始化函数：**advFilter_stack_enableDupExcListCmd(true)

#### ESP_BT_VS_SET_ADV_REPORT_FLOW_CTRL_OCF (0x0109)
**作用：**启用/禁用广播报告流量控制
**参数结构：**bt_hci_vs_init_adv_flow_ctrl
**初始化函数：**scan_stack_enableAdvFlowCtrlVsCmd(true)

#### ESP_BT_VS_UPD_ADV_REPORT_FLOW_CTRL_NUM_OCF (0x010A)
**作用：**更新广播报告流量控制数量
**参数结构：**bt_hci_vs_update_adv_flow_ctrl
**初始化函数：**scan_stack_enableAdvFlowCtrlVsCmd(true)

#### ESP_BT_VS_CLR_LEGACY_ADV_OCF (0x010C)
**作用：**清除传统广播（与HCI_LE_Clear_Advertising_Sets相同）
**参数结构：**bt_hci_vs_ble_clr_legacy_adv
**初始化函数：**adv_stack_enableClearLegacyAdvVsCmd(true)

#### ESP_BT_VS_SET_MIN_ENC_KEY_SIZE_OCF (0x0182)
**作用：**设置经典蓝牙最小加密密钥大小
**参数结构：**bt_hci_vs_set_min_enc_key_size
**初始化函数：**bt_stack_enableSecCtrlVsCmd(true)

### 用于Espressif蓝牙主机的VS事件

#### ESP_BT_VS_LE_ADV_LOST_EVT_SUBCODE (0xF0)
**作用：**BLE广播报告丢失事件（用于流量控制）
**事件代码：**0x3E（LE Meta Event），子代码：0xF0
**参数结构：**bt_hci_vs_le_adv_lost_evt
**初始化函数：**scan_stack_enableAdvFlowCtrlVsCmd(true)

#### ESP_BT_VS_LEGACY_REM_AUTH_EVT_SUBCODE (0x03)
**作用：**远程设备完成传统认证事件
**事件代码：**0xFF（Vendor Specific Event），子代码：0x03
**参数结构：**bt_hci_vs_legacy_rem_auth_evt
**初始化函数：**bt_stack_enableSecCtrlVsCmd(true)

## 典型使用场景

### 基础蓝牙初始化流程
```c
// 1. 初始化NVS（蓝牙需要NVS存储绑定信息）
esp_err_t ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
}
ESP_ERROR_CHECK(ret);

// 2. 释放不使用的蓝牙模式内存（可选）
// 如果只使用BLE，可以释放经典蓝牙内存
ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

// 3. 初始化蓝牙控制器
esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
ret = esp_bt_controller_init(&bt_cfg);
if (ret) {
    ESP_LOGE(TAG, "Bluetooth controller initialize failed: %s", esp_err_to_name(ret));
    return;
}

// 4. 启用蓝牙控制器
ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
if (ret) {
    ESP_LOGE(TAG, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
    return;
}

// 5. 初始化蓝牙主机栈（Bluedroid或NimBLE）
// 这部分代码在esp_bluedroid.h或nimble中
```

### BLE发射功率设置
```c
// 设置广播发射功率为最大（+9dBm）
esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);

// 设置扫描发射功率
esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P3);

// 获取当前广播发射功率
esp_power_level_t adv_power = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_ADV);
ESP_LOGI(TAG, "ADV power level: %d", adv_power);
```

### 经典蓝牙发射功率设置
```c
// 设置BR/EDR发射功率范围（最小-3dBm，最大+9dBm）
esp_bredr_tx_power_set(ESP_PWR_LVL_N3, ESP_PWR_LVL_P9);

// 获取BR/EDR发射功率范围
esp_power_level_t min_power, max_power;
esp_bredr_tx_power_get(&min_power, &max_power);
ESP_LOGI(TAG, "BR/EDR power range: %d to %d", min_power, max_power);
```

### 蓝牙低功耗模式配置
```c
// 启用蓝牙modem睡眠
esp_err_t ret = esp_bt_sleep_enable();
if (ret == ESP_OK) {
    ESP_LOGI(TAG, "Bluetooth modem sleep enabled");
} else {
    ESP_LOGE(TAG, "Failed to enable Bluetooth modem sleep");
}

// 禁用蓝牙modem睡眠
esp_bt_sleep_disable();
```

### 清除BLE扫描重复列表
```c
// 手动清除扫描重复列表
esp_ble_scan_duplicate_list_flush();
```

### 完整的蓝牙反初始化流程
```c
// 假设已经完成蓝牙功能，现在要释放所有资源

// 1. 反初始化蓝牙主机栈（Bluedroid）
esp_bluedroid_disable();
esp_bluedroid_deinit();

// 2. 禁用蓝牙控制器
esp_bt_controller_disable();

// 3. 反初始化蓝牙控制器
esp_bt_controller_deinit();

// 4. 释放蓝牙内存（双模）
esp_bt_mem_release(ESP_BT_MODE_BTDM);
```

### VHCI接口使用示例
```c
// VHCI回调函数
void notify_host_send_available_callback(void) {
    // 通知主机可以发送HCI数据
}

int notify_host_recv_callback(uint8_t *data, uint16_t len) {
    // 处理接收到的HCI数据
    return 0;
}

// 注册VHCI回调
esp_vhci_host_callback_t callback = {
    .notify_host_send_available = notify_host_send_available_callback,
    .notify_host_recv = notify_host_recv_callback
};
esp_vhci_host_register_callback(&callback);

// 发送HCI数据
if (esp_vhci_host_check_send_available()) {
    esp_vhci_host_send_packet(hci_data, data_len);
}
```

## 使用注意事项

1. **初始化顺序**：
   - 必须先调用esp_bt_controller_init()初始化控制器
   - 然后调用esp_bt_controller_enable()启用控制器
   - 最后初始化蓝牙主机栈（Bluedroid或NimBLE）

2. **内存管理**：
   - 蓝牙控制器占用约70KB内存
   - 如果不使用某种模式（BLE或经典蓝牙），可以调用esp_bt_controller_mem_release()释放内存
   - 内存释放是不可逆的，释放后无法再使用该模式

3. **模式切换**：
   - 由于已知问题，不能动态切换控制器模式
   - 要切换模式需要先disable、deinit，然后重新init、enable

4. **配置宏**：
   - 使用BT_CONTROLLER_INIT_CONFIG_DEFAULT()宏初始化配置结构体
   - 可以通过menuconfig调整参数，不建议直接修改

5. **发射功率**：
   - BLE连接发射功率必须在建立连接后设置
   - BR/EDR发射功率应在esp_bt_controller_enable()之后、RF传输操作之前调用

6. **低功耗模式**：
   - 调制解调器睡眠有ORIG和EVED两种模式，可在menuconfig中配置
   - esp_bt_sleep_enable()必须在esp_bt_controller_enable()之后调用

7. **扫描重复过滤**：
   - 扫描重复列表会在达到最大设备数时自动清除
   - 可以通过esp_ble_scan_duplicate_list_flush()手动清除

8. **VHCI使用**：
   - 发送HCI数据前必须先调用esp_vhci_host_check_send_available()检查
   - 不要在临界区或调度器挂起时调用esp_vhci_host_send_packet()

9. **SCO数据路径**：
   - 默认SCO数据通过HCI传输
   - 可设置为PCM传输，应在控制器启用后、(e)SCO链接建立前设置

10. **供应商特定命令**：
    - esp_bt_vs.h中的VS命令仅供Espressif蓝牙主机使用
    - 应用开发者不应调用初始化函数
    - 内部调试命令仅供Espressif内部使用

11. **线程安全**：
    - 蓝牙控制器API在正确使用时是线程安全的
    - 不要在ISR中调用蓝牙API

12. **电源管理**：
    - 蓝牙会影响系统电源管理
    - 使用自动光睡眠前请确保蓝牙低功耗模式已配置

## 常用配置参数

### 控制器任务配置
- **controller_task_stack_size**：建议使用ESP_TASK_BT_CONTROLLER_STACK（默认值）
- **controller_task_prio**：建议使用ESP_TASK_BT_CONTROLLER_PRIO（默认值）

### 连接数配置
- **ble_max_conn**：BLE最大连接数（1-9，默认3）
- **bt_max_acl_conn**：经典蓝牙最大ACL连接数（1-7，默认2）
- **bt_max_sync_conn**：经典蓝牙最大同步连接数（0-3，默认0）

### 扫描配置
- **scan_duplicate_mode**：0-普通模式，1-BLE Mesh模式
- **scan_duplicate_type**：0-仅地址，1-仅数据，2-地址和数据
- **normal_adv_size**：普通广播重复过滤列表大小（10-1000，默认100）
- **mesh_adv_size**：Mesh广播重复过滤列表大小（10-1000，默认100）

### 功率配置
发射功率级别对照表：
| 索引 | dBm值（典型） | 说明 |
|------|--------------|------|
| ESP_PWR_LVL_N12 | -12 dBm | 最低功率 |
| ESP_PWR_LVL_N9  | -9 dBm  | |
| ESP_PWR_LVL_N6  | -6 dBm  | |
| ESP_PWR_LVL_N3  | -3 dBm  | |
| ESP_PWR_LVL_N0  | 0 dBm   | BR/EDR默认最小值 |
| ESP_PWR_LVL_P3  | +3 dBm  | BR/EDR默认最大值 |
| ESP_PWR_LVL_P6  | +6 dBm  | |
| ESP_PWR_LVL_P9  | +9 dBm  | 最高功率 |

### HCI UART配置
- **hci_uart_no**：UART编号（1或2，默认1）
- **hci_uart_baudrate**：波特率（115200-921600，默认921600）
