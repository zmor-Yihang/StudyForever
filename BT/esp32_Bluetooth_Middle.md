# ESP32 蓝牙通信 API 完整函数参考

## 1. 蓝牙栈初始化和管理函数

### esp_bluedroid_init
**函数原型：**
```c
esp_err_t esp_bluedroid_init(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：状态不正确
ESP_ERR_NO_MEM：内存不足
**作用：**初始化Bluedroid蓝牙协议栈

### esp_bluedroid_deinit
**函数原型：**
```c
esp_err_t esp_bluedroid_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：栈未初始化或仍在运行
**作用：**反初始化Bluedroid蓝牙协议栈

### esp_bluedroid_enable
**函数原型：**
```c
esp_err_t esp_bluedroid_enable(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：栈未初始化或已启用
ESP_ERR_NO_MEM：内存不足
**作用：**使能Bluedroid蓝牙协议栈

### esp_bluedroid_disable
**函数原型：**
```c
esp_err_t esp_bluedroid_disable(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：栈未启用
**作用：**禁用Bluedroid蓝牙协议栈

### esp_bluedroid_get_status
**函数原型：**
```c
esp_bluedroid_status_t esp_bluedroid_get_status(void);
```
**参数：**
无
**返回值：**
ESP_BLUEDROID_STATUS_UNINITIALIZED：蓝牙未初始化
ESP_BLUEDROID_STATUS_INITIALIZED：蓝牙已初始化但未启用
ESP_BLUEDROID_STATUS_ENABLED：蓝牙已初始化并启用
**作用：**获取Bluedroid蓝牙协议栈状态

---

## 2. BLE GAP（通用访问配置文件）函数

### esp_ble_gap_register_callback
**函数原型：**
```c
esp_err_t esp_ble_gap_register_callback(esp_ble_gap_cb_t callback);
```
**参数：**
callback：GAP事件回调函数指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注册BLE GAP事件回调函数

### esp_ble_gap_set_device_name
**函数原型：**
```c
esp_err_t esp_ble_gap_set_device_name(const char *name);
```
**参数：**
name：设备名称字符串（最大长度31字节）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**设置BLE设备名称

### esp_ble_gap_get_device_name
**函数原型：**
```c
const char *esp_ble_gap_get_device_name(void);
```
**参数：**
无
**返回值：**
返回设备名称字符串指针，如果未设置则返回NULL
**作用：**获取当前BLE设备名称

### esp_ble_gap_config_adv_data
**函数原型：**
```c
esp_err_t esp_ble_gap_config_adv_data(esp_ble_adv_data_t *adv_data);
```
**参数：**
adv_data：指向esp_ble_adv_data_t结构体的指针，包含广告数据
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
ESP_ERR_NO_MEM：内存不足
**作用：**配置BLE广告数据

### esp_ble_gap_start_advertising
**函数原型：**
```c
esp_err_t esp_ble_gap_start_advertising(const esp_ble_adv_params_t *adv_params);
```
**参数：**
adv_params：指向esp_ble_adv_params_t结构体的指针，包含广告参数
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
ESP_ERR_NO_MEM：内存不足
**作用：**开始BLE广播

### esp_ble_gap_stop_advertising
**函数原型：**
```c
esp_err_t esp_ble_gap_stop_advertising(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：广播未启动
**作用：**停止BLE广播

### esp_ble_gap_set_scan_params
**函数原型：**
```c
esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params);
```
**参数：**
scan_params：指向esp_ble_scan_params_t结构体的指针，包含扫描参数
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**设置BLE扫描参数

### esp_ble_gap_start_scanning
**函数原型：**
```c
esp_err_t esp_ble_gap_start_scanning(uint32_t duration);
```
**参数：**
duration：扫描持续时间（单位：毫秒，0表示无限扫描）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：扫描已在进行中
**作用：**开始BLE设备扫描

### esp_ble_gap_stop_scanning
**函数原型：**
```c
esp_err_t esp_ble_gap_stop_scanning(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：扫描未进行
**作用：**停止BLE设备扫描

### esp_ble_gap_connect
**函数原型：**
```c
esp_err_t esp_ble_gap_connect(esp_ble_addr_type_t remote_addr_type, 
                             esp_bd_addr_t remote_bda);
```
**参数：**
remote_addr_type：远程设备地址类型
remote_bda：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**连接到指定的BLE设备

### esp_ble_gap_disconnect
**函数原型：**
```c
esp_err_t esp_ble_gap_disconnect(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**断开与指定BLE设备的连接

### esp_ble_gap_set_security_param
**函数原型：**
```c
esp_err_t esp_ble_gap_set_security_param(esp_ble_gap_sec_param_type_t param_type,
                                         uint8_t *value, uint8_t len);
```
**参数：**
param_type：安全参数类型
value：参数值指针
len：参数长度
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**设置BLE安全参数（配对、加密等）

### esp_ble_gap_passkey_reply
**函数原型：**
```c
esp_err_t esp_ble_gap_passkey_reply(esp_bd_addr_t bd_addr, bool accept, uint32_t passkey);
```
**参数：**
bd_addr：设备蓝牙地址
accept：是否接受配对
passkey：配对密钥（6位数字）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**回复BLE配对密钥请求

### esp_ble_gap_confirm_reply
**函数原型：**
```c
esp_err_t esp_ble_gap_confirm_reply(esp_bd_addr_t bd_addr, bool accept);
```
**参数：**
bd_addr：设备蓝牙地址
accept：是否确认
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**回复BLE配对确认请求

---

## 3. BLE GATT 客户端（GATTC）函数

### esp_ble_gattc_register_callback
**函数原型：**
```c
esp_err_t esp_ble_gattc_register_callback(esp_gattc_cb_t callback);
```
**参数：**
callback：GATT客户端事件回调函数指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注册BLE GATT客户端事件回调

### esp_ble_gattc_app_register
**函数原型：**
```c
esp_err_t esp_ble_gattc_app_register(uint16_t app_id);
```
**参数：**
app_id：应用ID（0-15）
**返回值：**
ESP_OK：操作成功
ESP_ERR_NO_MEM：内存不足
**作用：**注册GATT客户端应用

### esp_ble_gattc_app_unregister
**函数原型：**
```c
esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gattc_if);
```
**参数：**
gattc_if：GATT客户端接口句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注销GATT客户端应用

### esp_ble_gattc_open
**函数原型：**
```c
esp_err_t esp_ble_gattc_open(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda,
                             esp_ble_addr_type_t remote_addr_type, bool is_direct);
```
**参数：**
gattc_if：GATT客户端接口
remote_bda：远程设备蓝牙地址
remote_addr_type：远程设备地址类型
is_direct：是否直接连接
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**打开与远程GATT服务器的连接

### esp_ble_gattc_close
**函数原型：**
```c
esp_err_t esp_ble_gattc_close(esp_gatt_if_t gattc_if, uint16_t conn_id);
```
**参数：**
gattc_if：GATT客户端接口
conn_id：连接ID
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**关闭与远程GATT服务器的连接

### esp_ble_gattc_search_service
**函数原型：**
```c
esp_err_t esp_ble_gattc_search_service(esp_gatt_if_t gattc_if, uint16_t conn_id,
                                       esp_bt_uuid_t *filter_uuid);
```
**参数：**
gattc_if：GATT客户端接口
conn_id：连接ID
filter_uuid：过滤UUID，NULL表示搜索所有服务
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**搜索远程GATT服务

### esp_ble_gattc_read_char
**函数原型：**
```c
esp_err_t esp_ble_gattc_read_char(esp_gatt_if_t gattc_if, uint16_t conn_id,
                                  uint16_t handle);
```
**参数：**
gattc_if：GATT客户端接口
conn_id：连接ID
handle：特征值句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**读取远程特征值

### esp_ble_gattc_write_char
**函数原型：**
```c
esp_err_t esp_ble_gattc_write_char(esp_gatt_if_t gattc_if, uint16_t conn_id,
                                   uint16_t handle, uint16_t len, uint8_t *value,
                                   esp_gatt_write_type_t write_type);
```
**参数：**
gattc_if：GATT客户端接口
conn_id：连接ID
handle：特征值句柄
len：写入数据长度
value：写入数据指针
write_type：写入类型（ESP_GATT_WRITE_TYPE_RSP或ESP_GATT_WRITE_TYPE_NO_RSP）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**写入远程特征值

### esp_ble_gattc_register_for_notify
**函数原型：**
```c
esp_err_t esp_ble_gattc_register_for_notify(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda,
                                            uint16_t handle);
```
**参数：**
gattc_if：GATT客户端接口
remote_bda：远程设备蓝牙地址
handle：特征值句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注册特征通知

### esp_ble_gattc_unregister_for_notify
**函数原型：**
```c
esp_err_t esp_ble_gattc_unregister_for_notify(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda,
                                              uint16_t handle);
```
**参数：**
gattc_if：GATT客户端接口
remote_bda：远程设备蓝牙地址
handle：特征值句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注销特征通知

---

## 4. BLE GATT 服务器（GATTS）函数

### esp_ble_gatts_register_callback
**函数原型：**
```c
esp_err_t esp_ble_gatts_register_callback(esp_gatts_cb_t callback);
```
**参数：**
callback：GATT服务器事件回调函数指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注册BLE GATT服务器事件回调

### esp_ble_gatts_app_register
**函数原型：**
```c
esp_err_t esp_ble_gatts_app_register(uint16_t app_id);
```
**参数：**
app_id：应用ID（0-15）
**返回值：**
ESP_OK：操作成功
ESP_ERR_NO_MEM：内存不足
**作用：**注册GATT服务器应用

### esp_ble_gatts_app_unregister
**函数原型：**
```c
esp_err_t esp_ble_gatts_app_unregister(esp_gatt_if_t gatts_if);
```
**参数：**
gatts_if：GATT服务器接口句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注销GATT服务器应用

### esp_ble_gatts_create_service
**函数原型：**
```c
esp_err_t esp_ble_gatts_create_service(esp_gatt_if_t gatts_if, esp_gatt_srvc_id_t *service_id,
                                       uint16_t num_handles);
```
**参数：**
gatts_if：GATT服务器接口
service_id：指向服务ID的指针
num_handles：服务中属性的数量
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**创建GATT服务

### esp_ble_gatts_start_service
**函数原型：**
```c
esp_err_t esp_ble_gatts_start_service(uint16_t service_handle);
```
**参数：**
service_handle：服务句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**启动GATT服务

### esp_ble_gatts_stop_service
**函数原型：**
```c
esp_err_t esp_ble_gatts_stop_service(uint16_t service_handle);
```
**参数：**
service_handle：服务句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**停止GATT服务

### esp_ble_gatts_delete_service
**函数原型：**
```c
esp_err_t esp_ble_gatts_delete_service(uint16_t service_handle);
```
**参数：**
service_handle：服务句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**删除GATT服务

### esp_ble_gatts_add_char
**函数原型：**
```c
esp_err_t esp_ble_gatts_add_char(uint16_t service_handle, esp_bt_uuid_t *char_uuid,
                                 esp_gatt_perm_t perm, esp_gatt_char_prop_t property,
                                 esp_attr_value_t *attr_value, esp_attr_control_t *control);
```
**参数：**
service_handle：服务句柄
char_uuid：特征UUID
perm：属性权限（读/写/执行）
property：特征属性
attr_value：属性初始值
control：属性控制信息
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**添加特征到GATT服务

### esp_ble_gatts_add_char_descr
**函数原型：**
```c
esp_err_t esp_ble_gatts_add_char_descr(uint16_t service_handle, esp_bt_uuid_t *char_uuid,
                                       esp_gatt_perm_t perm, esp_attr_value_t *attr_value,
                                       esp_attr_control_t *control);
```
**参数：**
service_handle：服务句柄
char_uuid：描述符UUID
perm：属性权限
attr_value：属性初始值
control：属性控制信息
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**添加特征描述符到GATT服务

### esp_ble_gatts_set_attr_value
**函数原型：**
```c
esp_err_t esp_ble_gatts_set_attr_value(uint16_t attr_handle, uint16_t length,
                                       const uint8_t *value);
```
**参数：**
attr_handle：属性句柄
length：设置值的长度
value：设置值指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**设置GATT属性值

### esp_ble_gatts_send_indicate
**函数原型：**
```c
esp_err_t esp_ble_gatts_send_indicate(esp_gatt_if_t gatts_if, uint16_t conn_id,
                                      uint16_t attr_handle, uint16_t len, uint8_t *value,
                                      bool need_confirm);
```
**参数：**
gatts_if：GATT服务器接口
conn_id：连接ID
attr_handle：属性句柄
len：发送数据长度
value：发送数据指针
need_confirm：是否需要客户端确认
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**发送GATT指示或通知到客户端

### esp_ble_gatts_send_response
**函数原型：**
```c
esp_err_t esp_ble_gatts_send_response(esp_gatt_if_t gatts_if, uint16_t conn_id,
                                      uint32_t trans_id, esp_gatt_status_t status,
                                      esp_gatt_rsp_t *rsp);
```
**参数：**
gatts_if：GATT服务器接口
conn_id：连接ID
trans_id：事务ID
status：状态码
rsp：响应数据指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**发送GATT响应到客户端

---

## 5. 经典蓝牙 GAP 函数

### esp_bt_gap_register_callback
**函数原型：**
```c
esp_err_t esp_bt_gap_register_callback(esp_bt_gap_cb_t callback);
```
**参数：**
callback：GAP事件回调函数指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注册经典蓝牙GAP事件回调

### esp_bt_gap_set_device_name
**函数原型：**
```c
esp_err_t esp_bt_gap_set_device_name(const char *name);
```
**参数：**
name：设备名称字符串（最大长度248字节）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**设置经典蓝牙设备名称

### esp_bt_gap_get_device_name
**函数原型：**
```c
esp_err_t esp_bt_gap_get_device_name(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**获取经典蓝牙设备名称（通过回调返回）

### esp_bt_gap_set_scan_mode
**函数原型：**
```c
esp_err_t esp_bt_gap_set_scan_mode(esp_bt_connection_mode_t c_mode,
                                   esp_bt_discovery_mode_t d_mode);
```
**参数：**
c_mode：连接模式（可连接/不可连接）
d_mode：发现模式（可被发现/不可被发现）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**设置经典蓝牙扫描模式

### esp_bt_gap_start_discovery
**函数原型：**
```c
esp_err_t esp_bt_gap_start_discovery(esp_bt_inq_mode_t mode, uint8_t inq_len,
                                     uint8_t num_rsps);
```
**参数：**
mode：查询模式（标准模式或有限模式）
inq_len：查询长度（单位：1.28秒）
num_rsps：期望的响应数量
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**开始经典蓝牙设备发现

### esp_bt_gap_cancel_discovery
**函数原型：**
```c
esp_err_t esp_bt_gap_cancel_discovery(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_STATE：发现未进行
**作用：**停止经典蓝牙设备发现

### esp_bt_gap_get_remote_services
**函数原型：**
```c
esp_err_t esp_bt_gap_get_remote_services(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**获取远程设备的支持服务

### esp_bt_gap_remove_bond_device
**函数原型：**
```c
esp_err_t esp_bt_gap_remove_bond_device(esp_bd_addr_t bd_addr);
```
**参数：**
bd_addr：设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**移除已配对的蓝牙设备

### esp_bt_gap_get_bond_device_list
**函数原型：**
```c
esp_err_t esp_bt_gap_get_bond_device_list(int *dev_num, esp_bd_addr_t *dev_list);
```
**参数：**
dev_num：设备数量指针（输入/输出）
dev_list：设备列表指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
ESP_ERR_NO_MEM：内存不足
**作用：**获取已配对设备列表

### esp_bt_gap_set_pin
**函数原型：**
```c
esp_err_t esp_bt_gap_set_pin(esp_bt_pin_type_t pin_type, uint8_t pin_code_len,
                             esp_bt_pin_code_t pin_code);
```
**参数：**
pin_type：PIN码类型（变量或固定）
pin_code_len：PIN码长度
pin_code：PIN码数据
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**设置蓝牙PIN码

### esp_bt_gap_pin_reply
**函数原型：**
```c
esp_err_t esp_bt_gap_pin_reply(esp_bd_addr_t bd_addr, bool accept,
                               uint8_t pin_code_len, esp_bt_pin_code_t pin_code);
```
**参数：**
bd_addr：设备蓝牙地址
accept：是否接受配对
pin_code_len：PIN码长度
pin_code：PIN码数据
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**回复PIN码配对请求

### esp_bt_gap_ssp_passkey_reply
**函数原型：**
```c
esp_err_t esp_bt_gap_ssp_passkey_reply(esp_bd_addr_t bd_addr, bool accept,
                                       uint32_t passkey);
```
**参数：**
bd_addr：设备蓝牙地址
accept：是否接受配对
passkey：配对密钥（6位数字）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**回复简单配对(SSP)密钥请求

### esp_bt_gap_ssp_confirm_reply
**函数原型：**
```c
esp_err_t esp_bt_gap_ssp_confirm_reply(esp_bd_addr_t bd_addr, bool accept);
```
**参数：**
bd_addr：设备蓝牙地址
accept：是否确认
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**回复简单配对确认请求

---

## 6. SPP（串口配置文件）函数

### esp_spp_register_callback
**函数原型：**
```c
esp_err_t esp_spp_register_callback(esp_spp_cb_t callback);
```
**参数：**
callback：SPP事件回调函数指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注册SPP事件回调

### esp_spp_enhanced_init
**函数原型：**
```c
esp_err_t esp_spp_enhanced_init(const esp_spp_cfg_t *cfg);
```
**参数：**
cfg：指向esp_spp_cfg_t结构体的指针，包含SPP配置参数
**返回值：**
ESP_OK：操作成功
ESP_ERR_NO_MEM：内存不足
**作用：**初始化SPP配置文件（推荐使用）

### esp_spp_deinit
**函数原型：**
```c
esp_err_t esp_spp_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**反初始化SPP配置文件

### esp_spp_start_srv
**函数原型：**
```c
esp_err_t esp_spp_start_srv(esp_spp_sec_t sec_mask, esp_spp_role_t role,
                            uint8_t local_scn, const char *name);
```
**参数：**
sec_mask：安全掩码（认证、加密等）
role：角色（主机或从机）
local_scn：本地服务通道号
name：服务名称
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**启动SPP服务器

### esp_spp_stop_srv
**函数原型：**
```c
esp_err_t esp_spp_stop_srv(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**停止SPP服务器

### esp_spp_connect
**函数原型：**
```c
esp_err_t esp_spp_connect(esp_spp_sec_t sec_mask, esp_spp_role_t role,
                          uint8_t remote_scn, esp_bd_addr_t peer_bd_addr);
```
**参数：**
sec_mask：安全掩码
role：角色
remote_scn：远程服务通道号
peer_bd_addr：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**连接到远程SPP服务器

### esp_spp_disconnect
**函数原型：**
```c
esp_err_t esp_spp_disconnect(uint32_t handle);
```
**参数：**
handle：SPP连接句柄
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**断开SPP连接

### esp_spp_write
**函数原型：**
```c
esp_err_t esp_spp_write(uint32_t handle, int len, uint8_t *p_data);
```
**参数：**
handle：SPP连接句柄
len：发送数据长度
p_data：发送数据指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
ESP_ERR_NO_MEM：内存不足
**作用：**通过SPP发送数据

### esp_spp_vfs_register
**函数原型：**
```c
esp_err_t esp_spp_vfs_register(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**注册SPP虚拟文件系统接口

### esp_spp_vfs_unregister
**函数原型：**
```c
esp_err_t esp_spp_vfs_unregister(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**注销SPP虚拟文件系统接口

---

## 7. A2DP（音频分发配置文件）函数

### esp_a2d_source_init
**函数原型：**
```c
esp_err_t esp_a2d_source_init(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**初始化A2DP音频源

### esp_a2d_source_deinit
**函数原型：**
```c
esp_err_t esp_a2d_source_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**反初始化A2DP音频源

### esp_a2d_source_register_callback
**函数原型：**
```c
esp_err_t esp_a2d_source_register_callback(esp_a2d_source_cb_t callback);
```
**参数：**
callback：A2DP音频源事件回调函数指针
**返回值：**
ESP_OK：操作成功
**作用：**注册A2DP音频源事件回调

### esp_a2d_source_connect
**函数原型：**
```c
esp_err_t esp_a2d_source_connect(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址（音频汇）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**连接到远程A2DP音频汇设备

### esp_a2d_source_disconnect
**函数原型：**
```c
esp_err_t esp_a2d_source_disconnect(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**断开A2DP连接

### esp_a2d_source_start
**函数原型：**
```c
esp_err_t esp_a2d_source_start(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**开始A2DP音频流传输

### esp_a2d_source_stop
**函数原型：**
```c
esp_err_t esp_a2d_source_stop(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**停止A2DP音频流传输

---

## 8. HFP（免提配置文件）函数

### esp_hf_client_init
**函数原型：**
```c
esp_err_t esp_hf_client_init(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**初始化HFP免提客户端

### esp_hf_client_deinit
**函数原型：**
```c
esp_err_t esp_hf_client_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**反初始化HFP免提客户端

### esp_hf_client_register_callback
**函数原型：**
```c
esp_err_t esp_hf_client_register_callback(esp_hf_client_cb_t callback);
```
**参数：**
callback：HFP客户端事件回调函数指针
**返回值：**
ESP_OK：操作成功
**作用：**注册HFP客户端事件回调

### esp_hf_client_connect
**函数原型：**
```c
esp_err_t esp_hf_client_connect(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址（音频网关）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**连接到音频网关设备

### esp_hf_client_disconnect
**函数原型：**
```c
esp_err_t esp_hf_client_disconnect(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**断开HFP连接

### esp_hf_client_connect_audio
**函数原型：**
```c
esp_err_t esp_hf_client_connect_audio(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**建立HFP音频连接

### esp_hf_client_disconnect_audio
**函数原型：**
```c
esp_err_t esp_hf_client_disconnect_audio(esp_bd_addr_t remote_bda);
```
**参数：**
remote_bda：远程设备蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**断开HFP音频连接

### esp_hf_client_dial
**函数原型：**
```c
esp_err_t esp_hf_client_dial(const char *number);
```
**参数：**
number：要拨打的电话号码
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**拨打电话号码

### esp_hf_client_answer_call
**函数原型：**
```c
esp_err_t esp_hf_client_answer_call(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**接听电话

### esp_hf_client_reject_call
**函数原型：**
```c
esp_err_t esp_hf_client_reject_call(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**拒接电话

### esp_hf_client_volume_update
**函数原型：**
```c
esp_err_t esp_hf_client_volume_update(esp_hf_volume_control_target_t type, int volume);
```
**参数：**
type：音量类型（麦克风或扬声器）
volume：音量级别（0-15）
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**更新HFP音量

---

## 9. HID（人机界面设备）函数

### esp_bt_hid_device_init
**函数原型：**
```c
esp_err_t esp_bt_hid_device_init(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**初始化蓝牙HID设备

### esp_bt_hid_device_deinit
**函数原型：**
```c
esp_err_t esp_bt_hid_device_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**反初始化蓝牙HID设备

### esp_bt_hid_device_register_callback
**函数原型：**
```c
esp_err_t esp_bt_hid_device_register_callback(esp_hd_cb_t callback);
```
**参数：**
callback：HID设备事件回调函数指针
**返回值：**
ESP_OK：操作成功
**作用：**注册HID设备事件回调

### esp_bt_hid_device_register_app
**函数原型：**
```c
esp_err_t esp_bt_hid_device_register_app(esp_hidd_app_param_t *app_param,
                                         esp_hidd_qos_param_t *in_qos,
                                         esp_hidd_qos_param_t *out_qos);
```
**参数：**
app_param：应用参数指针
in_qos：输入QoS参数指针
out_qos：输出QoS参数指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**注册HID设备应用

### esp_bt_hid_device_unregister_app
**函数原型：**
```c
esp_err_t esp_bt_hid_device_unregister_app(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**注销HID设备应用

### esp_bt_hid_device_connect
**函数原型：**
```c
esp_err_t esp_bt_hid_device_connect(esp_bd_addr_t bd_addr);
```
**参数：**
bd_addr：主机蓝牙地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**连接到HID主机

### esp_bt_hid_device_disconnect
**函数原型：**
```c
esp_err_t esp_bt_hid_device_disconnect(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
**作用：**断开HID连接

### esp_bt_hid_device_send_report
**函数原型：**
```c
esp_err_t esp_bt_hid_device_send_report(esp_hidd_report_type_t type, uint8_t id,
                                        uint16_t len, uint8_t *data);
```
**参数：**
type：报告类型（输入/输出/功能）
id：报告ID
len：数据长度
data：报告数据指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数错误
**作用：**发送HID报告（按键、鼠标等事件）

### esp_bt_hid_device_report_error
**函数原型：**
```c
esp_err_t esp_bt_hid_device_report_error(esp_hidd_handshake_error_t error);
```
**参数：**
error：错误码
**返回值：**
ESP_OK：操作成功
**作用：**报告HID错误

---

## 11. 蓝牙常用回调事件详解

### 11.1 BLE GAP 事件

BLE GAP回调事件用于处理广播、扫描、连接等操作的状态变化。

#### 广播和扫描事件
- **ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT** (0)
  - 广告数据设置完成事件
  - 调用`esp_ble_gap_config_adv_data()`后触发

- **ESP_GAP_BLE_ADVERT_START_COMPLETE_EVT** (1)
  - 广告启动完成事件
  - 调用`esp_ble_gap_start_advertising()`后触发

- **ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT** (2)
  - 扫描响应数据设置完成事件

- **ESP_GAP_BLE_SCAN_START_COMPLETE_EVT** (4)
  - 扫描启动完成事件
  - 调用`esp_ble_gap_start_scanning()`后触发

#### 身份验证和加密事件
- **ESP_GAP_BLE_AUTH_CMPL_EVT** (8)
  - 身份验证完成事件
  - 配对完成时触发
  - 回调参数包含认证结果和密钥信息

- **ESP_GAP_BLE_PASSKEY_NOTIF_EVT** (9)
  - 密钥通知事件
  - 接收到配对请求时触发

- **ESP_GAP_BLE_OOB_REQ_EVT** (10)
  - OOB（带外）数据请求事件
  - 需要进行OOB验证时触发

#### 地址和通道事件
- **ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT** (19)
  - 设置静态随机地址完成事件

- **ESP_GAP_BLE_SET_CHANNELS_EVT** (29)
  - 设置BLE通道完成事件

#### 设备搜索事件
- **ESP_GAP_SEARCH_INQ_RES_EVT** (0)
  - 查询结果事件
  - 发现对端设备时触发

- **ESP_GAP_SEARCH_INQ_CMPL_EVT** (1)
  - 查询完成事件

- **ESP_GAP_SEARCH_DISC_RES_EVT** (2)
  - 发现结果事件（经典蓝牙服务发现）

- **ESP_GAP_SEARCH_DISC_BLE_RES_EVT** (3)
  - BLE GATT基础服务发现结果

- **ESP_GAP_SEARCH_DISC_CMPL_EVT** (4)
  - 发现完成事件

---

### 11.2 BLE GATT 客户端（GATTC）事件

#### 应用注册事件
- **ESP_GATTC_REG_EVT** (0)
  - 应用注册完成事件
  - 调用`esp_ble_gattc_app_register()`后触发
  - 回调中获取`gattc_if`句柄

- **ESP_GATTC_UNREG_EVT** (1)
  - 应用注销完成事件

#### 连接事件
- **ESP_GATTC_OPEN_EVT** (2)
  - 虚拟连接打开事件
  - 调用`esp_ble_gattc_open()`后触发
  - 回调参数包含连接ID和连接状态

- **ESP_GATTC_CLOSE_EVT** (5)
  - 虚拟连接关闭事件
  - 调用`esp_ble_gattc_close()`或物理连接断开时触发

- **ESP_GATTC_CONNECT_EVT** (40)
  - 物理连接建立事件
  - 物理层连接成功时触发

- **ESP_GATTC_DISCONNECT_EVT** (41)
  - 物理连接断开事件
  - 物理层连接断开时触发

#### 服务发现事件
- **ESP_GATTC_SEARCH_RES_EVT** (7)
  - 服务搜索结果事件
  - 每发现一个服务或特征时触发
  - 调用`esp_ble_gattc_search_service()`后触发

- **ESP_GATTC_SEARCH_CMPL_EVT** (6)
  - 服务搜索完成事件
  - 所有服务发现完成时触发

- **ESP_GATTC_DIS_SRVC_CMPL_EVT** (46)
  - GATT服务发现完成事件

#### 读写操作事件
- **ESP_GATTC_READ_CHAR_EVT** (3)
  - 特征值读取完成事件
  - 调用`esp_ble_gattc_read_char()`后触发
  - 回调参数包含读取的数据

- **ESP_GATTC_READ_DESCR_EVT** (8)
  - 特征描述符读取完成事件
  - 调用`esp_ble_gattc_read_char_descr()`后触发

- **ESP_GATTC_WRITE_CHAR_EVT** (4)
  - 特征值写入完成事件
  - 调用`esp_ble_gattc_write_char()`后触发

- **ESP_GATTC_WRITE_DESCR_EVT** (9)
  - 特征描述符写入完成事件

- **ESP_GATTC_PREP_WRITE_EVT** (11)
  - 预写操作完成事件
  - 调用`esp_ble_gattc_prepare_write()`后触发

- **ESP_GATTC_EXEC_EVT** (12)
  - 写入执行完成事件
  - 调用`esp_ble_gattc_execute_write()`后触发

- **ESP_GATTC_READ_MULTIPLE_EVT** (42)
  - 多个特征值读取完成事件

#### 通知和指示事件
- **ESP_GATTC_NOTIFY_EVT** (10)
  - 通知/指示事件
  - 从服务器接收通知或指示时触发

- **ESP_GATTC_REG_FOR_NOTIFY_EVT** (38)
  - 通知注册完成事件
  - 调用`esp_ble_gattc_register_for_notify()`后触发

- **ESP_GATTC_UNREG_FOR_NOTIFY_EVT** (39)
  - 通知注销完成事件

#### 配置和缓存事件
- **ESP_GATTC_CFG_MTU_EVT** (18)
  - MTU配置完成事件
  - 调用`esp_ble_gattc_send_mtu_req()`后触发

- **ESP_GATTC_SRVC_CHG_EVT** (15)
  - 服务变更通知事件
  - 服务器属性数据库改变时触发

- **ESP_GATTC_SET_ASSOC_EVT** (44)
  - 关联设置完成事件
  - 调用`esp_ble_gattc_cache_assoc()`后触发

- **ESP_GATTC_GET_ADDR_LIST_EVT** (45)
  - 获取地址列表完成事件

#### 拥塞事件
- **ESP_GATTC_CONGEST_EVT** (24)
  - 连接拥塞事件
  - 连接忙碌或恢复时触发

---

### 11.3 BLE GATT 服务器（GATTS）事件

#### 应用和服务事件
- **ESP_GATTS_REG_EVT** (0)
  - 应用注册完成事件
  - 调用`esp_ble_gatts_app_register()`后触发
  - 回调中获取`gatts_if`句柄

- **ESP_GATTS_CREATE_EVT** (7)
  - 服务创建完成事件
  - 调用`esp_ble_gatts_create_service()`后触发

- **ESP_GATTS_START_EVT** (12)
  - 服务启动完成事件
  - 调用`esp_ble_gatts_start_service()`后触发

- **ESP_GATTS_STOP_EVT** (13)
  - 服务停止完成事件
  - 调用`esp_ble_gatts_stop_service()`后触发

- **ESP_GATTS_DELETE_EVT** (11)
  - 服务删除完成事件

#### 特征和描述符事件
- **ESP_GATTS_ADD_CHAR_EVT** (9)
  - 特征添加完成事件
  - 调用`esp_ble_gatts_add_char()`后触发

- **ESP_GATTS_ADD_CHAR_DESCR_EVT** (10)
  - 特征描述符添加完成事件
  - 调用`esp_ble_gatts_add_char_descr()`后触发

- **ESP_GATTS_SET_ATTR_VAL_EVT** (23)
  - 属性值设置完成事件
  - 调用`esp_ble_gatts_set_attr_value()`后触发

#### 连接事件
- **ESP_GATTS_CONNECT_EVT** (14)
  - 物理连接建立事件
  - 客户端连接时触发
  - 回调参数包含连接ID和远程地址

- **ESP_GATTS_DISCONNECT_EVT** (15)
  - 物理连接断开事件
  - 客户端断开时触发

#### 读写操作事件
- **ESP_GATTS_READ_EVT** (1)
  - 客户端读取请求事件
  - 客户端尝试读取属性时触发
  - 需要调用`esp_ble_gatts_send_response()`回复

- **ESP_GATTS_WRITE_EVT** (2)
  - 客户端写入请求事件
  - 客户端尝试写入属性时触发
  - 回调参数包含写入的数据
  - 需要调用`esp_ble_gatts_send_response()`回复

- **ESP_GATTS_EXEC_WRITE_EVT** (3)
  - 客户端写入执行请求事件
  - 客户端执行待写入请求时触发

#### 通知和确认事件
- **ESP_GATTS_CONF_EVT** (5)
  - 客户端确认事件
  - 客户端确认通知或指示时触发

#### 其他事件
- **ESP_GATTS_MTU_EVT** (4)
  - MTU协商事件
  - 客户端请求MTU时触发

- **ESP_GATTS_UNREG_EVT** (6)
  - 应用注销完成事件

- **ESP_GATTS_ADD_INCL_SRVC_EVT** (8)
  - 包含服务添加完成事件

- **ESP_GATTS_OPEN_EVT** (16)
  - 虚拟连接打开事件
  - 调用`esp_ble_gatts_open()`后触发

- **ESP_GATTS_CLOSE_EVT** (18)
  - 虚拟连接关闭事件

- **ESP_GATTS_RESPONSE_EVT** (21)
  - 响应发送完成事件
  - 调用`esp_ble_gatts_send_response()`后触发

- **ESP_GATTS_CREAT_ATTR_TAB_EVT** (22)
  - 属性表创建完成事件
  - 调用`esp_ble_gatts_create_attr_tab()`后触发

- **ESP_GATTS_CONGEST_EVT** (20)
  - 连接拥塞事件

---

### 11.4 SPP（串口配置文件）事件

#### 初始化事件
- **ESP_SPP_INIT_EVT** (0)
  - SPP初始化完成事件
  - 调用`esp_spp_enhanced_init()`后触发

- **ESP_SPP_UNINIT_EVT** (1)
  - SPP反初始化完成事件

#### 服务器事件
- **ESP_SPP_START_EVT** (28)
  - SPP服务器启动完成事件
  - 调用`esp_spp_start_srv()`后触发

- **ESP_SPP_SRV_STOP_EVT** (35)
  - SPP服务器停止完成事件
  - 调用`esp_spp_stop_srv()`后触发

- **ESP_SPP_SRV_OPEN_EVT** (34)
  - 客户端连接到服务器事件
  - 新客户端连接时触发
  - 回调参数包含新的连接句柄

#### 客户端事件
- **ESP_SPP_CL_INIT_EVT** (29)
  - 客户端初始化事件

- **ESP_SPP_OPEN_EVT** (26)
  - 客户端连接打开事件
  - 连接成功时触发
  - 回调参数包含连接句柄

- **ESP_SPP_CLOSE_EVT** (27)
  - 连接关闭事件
  - 调用`esp_spp_disconnect()`或对端断开时触发

#### 数据传输事件
- **ESP_SPP_DATA_IND_EVT** (30)
  - 数据接收事件
  - 接收到来自对端的数据时触发
  - 仅在ESP_SPP_MODE_CB模式下触发
  - 回调参数包含接收的数据和长度

- **ESP_SPP_WRITE_EVT** (33)
  - 写入完成事件
  - 调用`esp_spp_write()`后触发
  - 仅在ESP_SPP_MODE_CB模式下触发

- **ESP_SPP_CONG_EVT** (31)
  - 拥塞状态变化事件
  - 连接忙碌或恢复时触发
  - 仅在ESP_SPP_MODE_CB模式下触发

#### 服务发现事件
- **ESP_SPP_DISCOVERY_COMP_EVT** (8)
  - 服务发现完成事件
  - 调用`esp_spp_start_discovery()`后触发

#### VFS事件
- **ESP_SPP_VFS_REGISTER_EVT** (36)
  - VFS注册完成事件

- **ESP_SPP_VFS_UNREGISTER_EVT** (37)
  - VFS注销完成事件

---

### 11.5 A2DP（音频分发配置文件）事件

#### 连接状态事件
- **ESP_A2D_CONNECTION_STATE_EVT** (0)
  - 连接状态变化事件
  - 连接建立或断开时触发
  - 回调参数包含连接状态和远程地址

- **ESP_A2D_AUDIO_STATE_EVT** (1)
  - 音频状态变化事件
  - 音频流启动或停止时触发

#### 媒体编码事件
- **ESP_A2D_MEDIA_CTRL_ACK_EVT** (2)
  - 媒体控制确认事件

- **ESP_A2D_PROF_STATE_EVT** (3)
  - A2DP配置文件状态事件
  - 初始化或反初始化完成时触发

---

### 11.6 HFP（免提配置文件）事件

#### HF 客户端事件
- **ESP_HF_CLIENT_CONNECTION_STATE_EVT** (0)
  - 连接状态变化事件
  - 连接建立或断开时触发

- **ESP_HF_CLIENT_AUDIO_STATE_EVT** (1)
  - 音频连接状态变化事件

- **ESP_HF_CLIENT_CALL_EVT** (2)
  - 来电事件

- **ESP_HF_CLIENT_CALL_HELD_EVT** (3)
  - 电话保持状态事件

- **ESP_HF_CLIENT_CALLSETUP_EVT** (4)
  - 通话设置事件

#### HF 音频网关事件
- **ESP_HF_CONNECTION_STATE_EVT** (0)
  - 连接状态变化事件

- **ESP_HF_AUDIO_STATE_EVT** (1)
  - 音频连接状态变化事件

---

### 11.7 HID（人机界面设备）事件

#### HID 设备事件
- **ESP_HIDD_INIT_EVT** (0)
  - HID设备初始化完成事件

- **ESP_HIDD_DEINIT_EVT** (1)
  - HID设备反初始化完成事件

- **ESP_HIDD_OPEN_EVT** (2)
  - 主机连接事件

- **ESP_HIDD_CLOSE_EVT** (3)
  - 主机断开事件

- **ESP_HIDD_SEND_REPORT_EVT** (4)
  - 报告发送完成事件

- **ESP_HIDD_REPORT_ERR_EVT** (5)
  - 报告错误事件

#### HID 主机事件
- **ESP_HIDH_INIT_EVT** (0)
  - HID主机初始化完成事件

---

### 11.8 AVRCP（音视频远程控制）事件

#### AVRCP 控制器事件
- **ESP_AVRC_CT_CONNECTION_STATE_EVT** (0)
  - 连接状态变化事件

- **ESP_AVRC_CT_PASSTHROUGH_RSP_EVT** (1)
  - 按键命令响应事件

- **ESP_AVRC_CT_METADATA_RSP_EVT** (2)
  - 元数据响应事件

- **ESP_AVRC_CT_PLAY_STATUS_RSP_EVT** (3)
  - 播放状态响应事件

- **ESP_AVRC_CT_CHANGE_NOTIFY_EVT** (4)
  - 状态变更通知事件

- **ESP_AVRC_CT_REMOTE_FEATURES_EVT** (5)
  - 远程设备功能指示事件

#### AVRCP 目标设备事件
- **ESP_AVRC_TG_CONNECTION_STATE_EVT** (0)
  - 连接状态变化事件

- **ESP_AVRC_TG_PASSTHROUGH_CMD_EVT** (2)
  - 按键命令事件

- **ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT** (3)
  - 绝对音量设置命令

---

### 11.9 经典蓝牙 GAP 事件

- **ESP_BT_GAP_DISC_RES_EVT** (0)
  - 设备发现结果事件
  - 发现对端设备时触发

- **ESP_BT_GAP_DISC_STATE_CHANGED_EVT** (1)
  - 发现状态变化事件

- **ESP_BT_GAP_RMT_SRVCS_EVT** (2)
  - 远程服务事件

- **ESP_BT_GAP_RMT_SRVC_REC_EVT** (3)
  - 远程服务记录事件

- **ESP_BT_GAP_AUTH_CMPL_EVT** (4)
  - 身份验证完成事件

- **ESP_BT_GAP_PIN_REQ_EVT** (5)
  - PIN码请求事件
  - 需要调用`esp_bt_gap_pin_reply()`回复

- **ESP_BT_GAP_CFM_REQ_EVT** (6)
  - 配对确认请求事件

- **ESP_BT_GAP_KEY_NOTIF_EVT** (7)
  - 密钥通知事件

- **ESP_BT_GAP_MODE_CHG_EVT** (8)
  - 模式变化事件

---

### 11.10 L2CAP 事件

- **ESP_BT_L2CAP_INIT_EVT** (0)
  - L2CAP初始化完成事件

- **ESP_BT_L2CAP_OPEN_EVT** (16)
  - L2CAP连接打开事件

- **ESP_BT_L2CAP_CLOSE_EVT** (17)
  - L2CAP连接关闭事件

- **ESP_BT_L2CAP_START_EVT** (18)
  - L2CAP服务器启动事件

---

### 11.11 回调事件处理最佳实践

#### 回调函数的原则
1. **保持简短**：不要在回调中执行耗时操作
2. **避免阻塞**：不要调用阻塞性的系统调用
3. **使用事件队列**：将处理逻辑通过事件发送给任务处理
4. **错误处理**：检查回调参数的有效性

#### 典型的回调处理模式
```c
// 回调函数模板
static void ble_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADVERT_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
                // 广告启动成功 - 发送事件到主任务
                xEventGroupSetBits(ble_event_group, ADV_STARTED_BIT);
            }
            break;
            
        case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
            // 收到密钥通知 - 显示给用户
            printf("Passkey: %06d\n", param->ble_security.key_notif.passkey);
            break;
            
        default:
            break;
    }
}

// GATTC回调示例
static void ble_gattc_callback(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                               esp_ble_gattc_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTC_OPEN_EVT:
            // 连接打开 - 保存连接ID进行后续操作
            g_conn_id = param->open.conn_id;
            esp_ble_gattc_search_service(gattc_if, g_conn_id, NULL);
            break;
            
        case ESP_GATTC_SEARCH_RES_EVT:
            // 发现服务 - 处理发现结果
            if (param->search_res.srvc_id.is_primary) {
                // 保存主服务信息
                save_service_info(&param->search_res.srvc_id);
            }
            break;
            
        case ESP_GATTC_NOTIFY_EVT:
            // 接收通知 - 处理接收的数据
            if (param->notify.is_notify) {
                process_notification(param->notify.value, param->notify.value_len);
            }
            break;
            
        default:
            break;
    }
}
```
