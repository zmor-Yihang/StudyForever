# ESP32 蓝牙控制器 API 完整函数参考

---

## 0. 常用数据结构

### esp_bt_mode_t

蓝牙控制器模式枚举：

```c
typedef enum {
    ESP_BT_MODE_IDLE       = 0x00,   // 蓝牙未运行
    ESP_BT_MODE_BLE        = 0x01,   // 蓝牙以BLE模式运行
    ESP_BT_MODE_CLASSIC_BT = 0x02,   // 不支持的模式
    ESP_BT_MODE_BTDM       = 0x03,   // 不支持的模式
} esp_bt_mode_t;
```

### esp_bt_controller_status_t

蓝牙控制器状态枚举：

```c
typedef enum {
    ESP_BT_CONTROLLER_STATUS_IDLE = 0,     // 控制器未初始化或已反初始化
    ESP_BT_CONTROLLER_STATUS_INITED,       // 控制器已初始化但未启用或已禁用
    ESP_BT_CONTROLLER_STATUS_ENABLED,      // 控制器已初始化并启用
    ESP_BT_CONTROLLER_STATUS_NUM,          // 控制器状态数量
} esp_bt_controller_status_t;
```

### esp_bt_sleep_mode_t

蓝牙睡眠模式枚举：

```c
typedef enum {
    ESP_BT_SLEEP_MODE_NONE = 0,      // 禁用睡眠模式
    ESP_BT_SLEEP_MODE_1    = 1,      // 启用睡眠模式
} esp_bt_sleep_mode_t;
```

### esp_bt_sleep_clock_t

蓝牙睡眠时钟源枚举：

```c
typedef enum {
    ESP_BT_SLEEP_CLOCK_NONE            = 0,   // 未配置睡眠时钟
    ESP_BT_SLEEP_CLOCK_MAIN_XTAL       = 1,   // SoC主晶振
    ESP_BT_SLEEP_CLOCK_EXT_32K_XTAL    = 2,   // 外部32.768kHz晶振
    ESP_BT_SLEEP_CLOCK_RTC_SLOW        = 3,   // 内部136kHz RC振荡器
    ESP_BT_SLEEP_CLOCK_FPGA_32K        = 4,   // FPGA临时使用的硬连线32KHz时钟
} esp_bt_sleep_clock_t;
```

### esp_ble_power_type_t

BLE TX功率类型枚举（用于基础API）：

```c
typedef enum {
    ESP_BLE_PWR_TYPE_CONN_HDL0  = 0,    // 连接句柄0的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL1  = 1,    // 连接句柄1的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL2  = 2,    // 连接句柄2的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL3  = 3,    // 连接句柄3的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL4  = 4,    // 连接句柄4的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL5  = 5,    // 连接句柄5的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL6  = 6,    // 连接句柄6的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL7  = 7,    // 连接句柄7的TX功率
    ESP_BLE_PWR_TYPE_CONN_HDL8  = 8,    // 连接句柄8的TX功率
    ESP_BLE_PWR_TYPE_ADV        = 9,    // 广播状态的TX功率
    ESP_BLE_PWR_TYPE_SCAN       = 10,   // 扫描状态的TX功率
    ESP_BLE_PWR_TYPE_DEFAULT    = 11,   // 未设置状态的默认TX功率
    ESP_BLE_PWR_TYPE_NUM        = 12,   // 保留
} esp_ble_power_type_t;
```

### esp_ble_enhanced_power_type_t

BLE TX功率增强类型枚举（推荐使用）：

```c
typedef enum {
    ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT = 0,   // 未设置状态的默认TX功率
    ESP_BLE_ENHANCED_PWR_TYPE_ADV,           // 广播状态的TX功率
    ESP_BLE_ENHANCED_PWR_TYPE_SCAN,          // 扫描状态的TX功率
    ESP_BLE_ENHANCED_PWR_TYPE_INIT,          // 初始化状态的TX功率
    ESP_BLE_ENHANCED_PWR_TYPE_CONN,          // 连接状态的TX功率
    ESP_BLE_ENHANCED_PWR_TYPE_MAX,           // 保留
} esp_ble_enhanced_power_type_t;
```

### esp_power_level_t

蓝牙TX功率级别枚举：

```c
typedef enum {
    ESP_PWR_LVL_N24 = 0,      // 对应 -24 dBm
    ESP_PWR_LVL_N21 = 1,      // 对应 -21 dBm
    ESP_PWR_LVL_N18 = 2,      // 对应 -18 dBm
    ESP_PWR_LVL_N15 = 3,      // 对应 -15 dBm
    ESP_PWR_LVL_N12 = 4,      // 对应 -12 dBm
    ESP_PWR_LVL_N9  = 5,      // 对应 -9 dBm
    ESP_PWR_LVL_N6  = 6,      // 对应 -6 dBm
    ESP_PWR_LVL_N3  = 7,      // 对应 -3 dBm
    ESP_PWR_LVL_N0  = 8,      // 对应 0 dBm
    ESP_PWR_LVL_P3  = 9,      // 对应 +3 dBm
    ESP_PWR_LVL_P6  = 10,     // 对应 +6 dBm
    ESP_PWR_LVL_P9  = 11,     // 对应 +9 dBm
    ESP_PWR_LVL_P12 = 12,     // 对应 +12 dBm
    ESP_PWR_LVL_P15 = 13,     // 对应 +15 dBm
    ESP_PWR_LVL_P18 = 14,     // 对应 +18 dBm
    ESP_PWR_LVL_P20 = 15,     // 对应 +20 dBm
    ESP_PWR_LVL_INVALID = 0xFF, // 表示无效值
} esp_power_level_t;
```

### esp_bt_controller_config_t

蓝牙控制器配置结构体（主要字段）：

```c
typedef struct {
    uint32_t magic;                         // 魔数
    uint32_t version;                       // 版本号
    uint16_t controller_task_stack_size;    // 控制器任务堆栈大小（字节）
    uint8_t controller_task_prio;           // 控制器任务优先级
    uint8_t controller_task_run_cpu;        // 控制器任务运行的CPU编号
    uint8_t bluetooth_mode;                 // BLE模式
    uint8_t ble_max_act;                    // BLE实例的最大数量（1-10，默认6）
    uint8_t sleep_mode;                     // 睡眠模式（0-禁用，1-启用）
    uint8_t sleep_clock;                    // 睡眠时钟源
    uint8_t hci_tl_type;                    // HCI传输层类型（0-UART，1-VHCI）
    esp_bt_hci_tl_t *hci_tl_funcs;          // HCI传输层函数
    uint8_t txpwr_dft;                      // 默认TX功率
    // ... 更多配置字段
} esp_bt_controller_config_t;
```

### esp_vhci_host_callback_t

VHCI主机回调函数结构体：

```c
typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               // 通知主机可以发送HCI数据
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   // 通知主机控制器有HCI数据要发送
} esp_vhci_host_callback_t;
```

## 1. 蓝牙控制器初始化和反初始化函数

### esp_bt_controller_init
**函数原型：**
```c
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);
```
**参数：**
cfg：指向蓝牙控制器初始配置结构体的指针
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
ESP_ERR_NOT_SUPPORTED：不支持的蓝牙控制器模式
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NO_MEM：内存不足
ESP_FAIL：其他原因导致的失败
**作用：**
初始化蓝牙控制器，分配任务和其他资源。此函数应该只调用一次，在任何其他蓝牙函数之前调用

### esp_bt_controller_deinit
**函数原型：**
```c
esp_err_t esp_bt_controller_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
ESP_ERR_NO_MEM：内存不足
**作用：**
反初始化蓝牙控制器，释放资源并删除任务。调用前应确保控制器处于空闲状态

---

## 2. 蓝牙控制器启用和禁用函数

### esp_bt_controller_enable
**函数原型：**
```c
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode);
```
**参数：**
mode：要启用的蓝牙控制器模式（ESP_BT_MODE_BLE等）
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
ESP_ERR_INVALID_ARG：参数无效
**作用：**
启用蓝牙控制器。控制器必须先初始化才能启用

**注意事项：**
1. 蓝牙控制器不能在`ESP_BT_CONTROLLER_STATUS_IDLE`状态下启用，必须先初始化
2. 由于已知问题，不能第二次调用此函数动态更改控制器模式。要更改模式，需先调用`esp_bt_controller_disable()`，然后再使用新模式调用此函数

### esp_bt_controller_disable
**函数原型：**
```c
esp_err_t esp_bt_controller_disable(void);
```
**参数：**
无
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
**作用：**
禁用蓝牙控制器

---

## 3. 蓝牙控制器状态查询函数

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
**作用：**
获取蓝牙控制器当前状态

---

## 4. 蓝牙控制器内存管理函数

### esp_bt_controller_mem_release
**函数原型：**
```c
esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode);
```
**参数：**
mode：蓝牙控制器模式
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
ESP_ERR_NOT_FOUND：未找到请求的资源
**作用：**
根据模式释放控制器内存（BSS、数据等），总大小约70KB

**注意事项：**
1. 此函数是可选的，仅在需要为其他组件释放内存时调用
2. 只能在控制器处于`ESP_BT_CONTROLLER_STATUS_IDLE`状态时调用
3. 内存释放后不可逆，无法再使用已释放的蓝牙控制器模式
4. 如果固件将来需要升级蓝牙控制器模式，请勿调用此函数

### esp_bt_mem_release
**函数原型：**
```c
esp_err_t esp_bt_mem_release(esp_bt_mode_t mode);
```
**参数：**
mode：蓝牙控制器模式
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
ESP_ERR_NOT_FOUND：未找到请求的资源
**作用：**
释放控制器内存、BLE主机栈的BSS和数据段。首先内部调用`esp_bt_controller_mem_release()`释放控制器内存，然后释放主机内存

**使用示例：**
如果仅使用蓝牙进行WiFi配置，配置完成后不再使用蓝牙，可按以下顺序释放内存：
```c
esp_bluedroid_disable();
esp_bluedroid_deinit();
esp_bt_controller_disable();
esp_bt_controller_deinit();
esp_bt_mem_release(ESP_BT_MODE_BLE);
```

---

## 5. 蓝牙睡眠模式函数

### esp_bt_sleep_enable
**函数原型：**
```c
esp_err_t esp_bt_sleep_enable(void);
```
**参数：**
无
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
ESP_ERR_NOT_SUPPORTED：不支持的操作或特性
**作用：**
启用蓝牙调制解调器睡眠。在ORIG模式下，如果没有事件要处理，控制器会定期关闭某些组件并暂停操作，然后按计划间隔唤醒并恢复工作

**注意事项：**
此函数不应在`esp_bt_controller_enable()`之前调用

### esp_bt_sleep_disable
**函数原型：**
```c
esp_err_t esp_bt_sleep_disable(void);
```
**参数：**
无
**返回值：**
ESP_OK：成功
ESP_ERR_INVALID_STATE：无效的蓝牙控制器状态
ESP_ERR_NOT_SUPPORTED：不支持的操作或特性
**作用：**
禁用蓝牙调制解调器睡眠。调用后控制器不允许进入睡眠模式

**注意事项：**
1. 控制器在调用此函数后不允许进入睡眠模式
2. 在ORIG模式下，如果控制器当前处于休眠状态，调用此函数可能不会立即唤醒
3. 此函数不应在`esp_bt_controller_enable()`之前调用

### esp_bt_get_lpclk_src
**函数原型：**
```c
esp_bt_sleep_clock_t esp_bt_get_lpclk_src(void);
```
**参数：**
无
**返回值：**
蓝牙低功耗模式下使用的时钟源（esp_bt_sleep_clock_t枚举值）
**作用：**
获取蓝牙睡眠时钟源

**注意事项：**
此函数不应在`esp_bt_controller_init()`之前调用

### esp_bt_controller_is_sleeping
**函数原型：**
```c
bool esp_bt_controller_is_sleeping(void);
```
**参数：**
无
**返回值：**
true：控制器处于睡眠状态
false：控制器未处于睡眠状态
**作用：**
检查启用睡眠模式时蓝牙控制器是否处于睡眠状态

**注意事项：**
1. 此函数不应在`esp_bt_controller_enable()`之前调用
2. 此函数应该在ORIG模式的睡眠模式下使用

### esp_bt_controller_wakeup_request
**函数原型：**
```c
void esp_bt_controller_wakeup_request(void);
```
**参数：**
无
**返回值：**
无
**作用：**
请求控制器在睡眠模式下从睡眠状态唤醒。性能分析显示，唤醒需要几毫秒时间

**注意事项：**
1. 此函数不应在`esp_bt_controller_enable()`之前调用
2. 此函数应该在ORIG模式的睡眠模式下使用
3. 请求后，只要睡眠模式仍然启用，控制器可以重新进入睡眠

---

## 6. BLE发射功率设置函数

### esp_ble_tx_power_set
**函数原型：**
```c
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level);
```
**参数：**
power_type：TX功率类型（广播、连接或默认）
power_level：功率级别索引（对应绝对值dBm）
**返回值：**
ESP_OK：成功
ESP_ERR_NOT_SUPPORTED：无效的TX功率类型
ESP_FAIL：其他原因导致的失败
**作用：**
为指定的功率类型设置BLE TX功率

**注意事项：**
1. 连接TX功率只能在连接建立后设置
2. 建议使用`esp_ble_tx_power_set_enhanced`为单个广播和连接句柄设置TX功率

### esp_ble_tx_power_get
**函数原型：**
```c
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);
```
**参数：**
power_type：TX功率类型（广播/连接/默认等）
**返回值：**
功率级别（esp_power_level_t枚举值），如果功率类型无效则返回ESP_PWR_LVL_INVALID
**作用：**
获取指定功率类型的BLE TX功率

**注意事项：**
1. 连接TX功率只能在连接建立后获取
2. 建议使用`esp_ble_tx_power_get_enhanced`获取单个广播和连接句柄的TX功率

### esp_ble_tx_power_set_enhanced
**函数原型：**
```c
esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle, esp_power_level_t power_level);
```
**参数：**
power_type：TX功率类型（增强型）
handle：广播或连接的句柄
power_level：功率级别索引（对应绝对值dBm）
**返回值：**
ESP_OK：成功
ESP_ERR_NOT_SUPPORTED：无效的TX功率类型
ESP_FAIL：其他原因导致的失败
**作用：**
为指定的广播或连接句柄设置BLE TX功率

**注意事项：**
1. 对于`ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT`、`ESP_BLE_ENHANCED_PWR_TYPE_SCAN`、`ESP_BLE_ENHANCED_PWR_TYPE_INIT`类型，此API会忽略输入的句柄号，内部设置为0
2. 对于`ESP_BLE_ENHANCED_PWR_TYPE_ADV`和`ESP_BLE_ENHANCED_PWR_TYPE_CONN`类型，此API会为目标句柄设置TX功率
3. 连接TX功率只能在连接创建后设置

### esp_ble_tx_power_get_enhanced
**函数原型：**
```c
esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle);
```
**参数：**
power_type：TX功率类型（增强型）
handle：广播或连接的句柄
**返回值：**
功率级别（esp_power_level_t枚举值），如果功率类型无效则返回ESP_PWR_LVL_INVALID
**作用：**
获取指定广播或连接句柄的BLE TX功率

**注意事项：**
1. 对于`ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT`、`ESP_BLE_ENHANCED_PWR_TYPE_SCAN`、`ESP_BLE_ENHANCED_PWR_TYPE_INIT`类型，此API会忽略输入的句柄号
2. 对于`ESP_BLE_ENHANCED_PWR_TYPE_ADV`和`ESP_BLE_ENHANCED_PWR_TYPE_CONN`类型，此API会返回目标句柄的TX功率
3. 连接TX功率只能在连接创建后获取

---

## 7. VHCI（虚拟HCI）接口函数

### esp_vhci_host_check_send_available
**函数原型：**
```c
bool esp_vhci_host_check_send_available(void);
```
**参数：**
无
**返回值：**
true：控制器准备好接收HCI数据
false：控制器未准备好接收HCI数据
**作用：**
检查控制器是否准备好接收HCI数据

**注意事项：**
此函数应在每次`esp_vhci_host_send_packet()`之前调用

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
**作用：**
向控制器发送HCI数据

**注意事项：**
1. 此函数不应在临界区内或调度器挂起时调用
2. 此函数只应在`esp_vhci_host_check_send_available`返回true时调用

### esp_vhci_host_register_callback
**函数原型：**
```c
esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback);
```
**参数：**
callback：`esp_vhci_host_callback`类型的回调函数结构体指针
**返回值：**
ESP_OK：成功
ESP_FAIL：失败
**作用：**
注册VHCI回调函数，用于通知主机下一步操作

---

## 8. HCI传输层通知函数

### esp_bt_h4tl_eif_io_event_notify
**函数原型：**
```c
int esp_bt_h4tl_eif_io_event_notify(int event);
```
**参数：**
event：事件值（当前被忽略）
**返回值：**
返回值（具体含义依实现而定）
**作用：**
通知蓝牙控制器任务在TX或RX完成时处理事件

**注意事项：**
1. 此函数不应在`esp_bt_controller_enable()`之前调用
2. 此函数可以在ISR和非ISR上下文中调用
3. 此函数当前忽略传递的事件值

---

## 9. 日志调试函数

### esp_ble_controller_log_dump_all
**函数原型：**
```c
void esp_ble_controller_log_dump_all(bool output);
```
**参数：**
output：是否输出日志
**返回值：**
无
**作用：**
转储所有BLE控制器日志

---

## 典型使用流程

### 基本初始化流程
```c
// 1. 初始化NVS（用于存储蓝牙配置）
esp_err_t ret = nvs_flash_init();

// 2. 初始化蓝牙控制器
esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
ret = esp_bt_controller_init(&bt_cfg);

// 3. 启用蓝牙控制器
ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);

// 4. 初始化和启用Bluedroid主机栈
ret = esp_bluedroid_init();
ret = esp_bluedroid_enable();
```

### 完整关闭流程
```c
// 1. 禁用Bluedroid主机栈
esp_bluedroid_disable();

// 2. 反初始化Bluedroid
esp_bluedroid_deinit();

// 3. 禁用蓝牙控制器
esp_bt_controller_disable();

// 4. 反初始化蓝牙控制器
esp_bt_controller_deinit();

// 5. （可选）释放蓝牙内存
esp_bt_mem_release(ESP_BT_MODE_BLE);
```

### 睡眠模式使用
```c
// 启用蓝牙控制器
esp_bt_controller_enable(ESP_BT_MODE_BLE);

// 启用睡眠模式
esp_bt_sleep_enable();

// 检查是否在睡眠
if (esp_bt_controller_is_sleeping()) {
    // 请求唤醒
    esp_bt_controller_wakeup_request();
}

// 禁用睡眠模式
esp_bt_sleep_disable();
```

### 设置TX功率
```c
// 方法1：使用基础API设置广播功率
esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);

// 方法2：使用增强API为特定句柄设置功率
uint16_t adv_handle = 0;
esp_ble_tx_power_set_enhanced(ESP_BLE_ENHANCED_PWR_TYPE_ADV, adv_handle, ESP_PWR_LVL_P9);

// 获取当前功率
esp_power_level_t power = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_ADV);
```
