# ESP32 WiFi API 完整函数参考

## WiFi头文件说明

ESP32的WiFi功能由多个头文件组成，每个头文件负责不同的功能模块：

### 核心头文件

#### esp_wifi.h
**作用：** WiFi驱动的主要API头文件  
**功能：**

- WiFi初始化和反初始化（init/deinit）
- WiFi模式设置（Station/AP/Station+AP/NAN）
- WiFi启动和停止
- 连接和断开AP
- 扫描AP
- 配置管理（带宽、协议、信道、功率等）
- 混杂模式
- 事件管理
- MAC地址和国家代码设置

**说明：** 这是使用WiFi功能的核心头文件，包含了大部分常用的WiFi API

#### esp_wifi_types.h & esp_wifi_types_generic.h
**作用：** WiFi数据类型定义  
**功能：**
- WiFi模式枚举（wifi_mode_t）
- WiFi接口类型（wifi_interface_t）
- 认证模式（wifi_auth_mode_t）
- 扫描配置（wifi_scan_config_t）
- AP记录（wifi_ap_record_t）
- Station配置（wifi_sta_config_t）
- SoftAP配置（wifi_ap_config_t）
- 各种WiFi参数结构体和枚举

**说明：** 定义了所有WiFi相关的数据结构和类型，是其他WiFi头文件的基础

### 高级功能头文件

#### esp_now.h
**作用：** ESP-NOW无连接通信协议  
**功能：**
- ESP-NOW初始化和反初始化
- 添加/删除/修改对等设备（peer）
- 发送和接收ESP-NOW数据
- 加密对等设备管理
- 回调函数注册

**说明：** ESP-NOW是一种无需连接到WiFi网络就能实现ESP设备间快速通信的协议，适用于智能灯控制、传感器数据采集等场景  
**最大数据长度：** v1.0为250字节，v2.0为1470字节

#### esp_mesh.h & esp_mesh_internal.h
**作用：** ESP-MESH自组网协议  
**功能：**
- Mesh网络初始化和配置
- Mesh网络启动和停止
- Mesh数据发送和接收
- Mesh拓扑管理
- 根节点选举
- 自愈和流量控制

**说明：** ESP-MESH允许ESP设备组成自组织网络，支持多跳通信，适用于智能照明、智能家居等大规模设备组网场景  
**esp_mesh_internal.h：** 包含Mesh的内部数据结构和高级配置

#### esp_smartconfig.h & smartconfig_ack.h
**作用：** SmartConfig智能配网协议  
**功能：**
- SmartConfig启动和停止
- 超时设置
- 支持ESPTouch、AirKiss、ESPTouch V2等协议
- 通过手机APP配置ESP设备连接WiFi

**说明：** SmartConfig用于帮助没有输入界面的设备快速获取WiFi账号密码，用户通过手机APP广播包含SSID和密码的特殊数据包  
**smartconfig_ack.h：** SmartConfig确认应答相关定义

### 网络接口头文件

#### esp_wifi_default.h
**作用：** WiFi默认网络接口配置  
**功能：**
- 创建默认的WiFi Station/AP/NAN网络接口
- 附加WiFi接口到esp_netif
- 设置默认的WiFi事件处理器
- 清除默认WiFi驱动和处理器

**说明：** 简化WiFi网络接口的创建和配置过程，提供开箱即用的网络接口

#### esp_wifi_netif.h
**作用：** WiFi网络接口驱动抽象层  
**功能：**
- 创建和销毁WiFi接口驱动实例
- WiFi接口与esp_netif的桥接
- 注册接收回调函数
- 获取接口MAC地址

**说明：** 提供WiFi驱动与网络协议栈（esp_netif）之间的抽象层

### 安全和高级特性头文件

#### esp_wifi_crypto_types.h
**作用：** WiFi加密算法类型定义  
**功能：**
- 定义哈希算法枚举（MD5、SHA1、SHA256等）
- 定义块加密算法枚举（AES、3DES、DES等）
- WPA/WPA2/WPA3加密相关的函数指针结构体

**说明：** 内部API，用于配置WiFi加密操作的实现，正常使用时不需要直接使用

#### esp_wifi_he.h & esp_wifi_he_types.h
**作用：** WiFi 6 (802.11ax) HE（高效率）特性支持  
**功能：**
- TWT（Target Wake Time）目标唤醒时间配置
- 个人和广播TWT协议管理
- HE特定参数配置

**说明：** 支持WiFi 6的高级省电和性能优化特性  
**esp_wifi_he_types.h：** HE相关的数据类型定义

#### esp_wifi_ap_get_sta_list.h
**作用：** 获取连接到AP的Station列表  
**功能：**
- 定义wifi_sta_list_t结构体
- 获取连接到SoftAP的所有Station信息

**说明：** 专门用于AP模式下管理连接的Station

## 0. 常用数据结构

### wifi_mode_t

WiFi工作模式枚举：

- WIFI_MODE_NULL：空模式
- WIFI_MODE_STA：Station模式
- WIFI_MODE_AP：SoftAP模式
- WIFI_MODE_APSTA：Station+SoftAP模式
- WIFI_MODE_NAN：NAN模式

### wifi_interface_t

WiFi接口类型枚举：

- WIFI_IF_STA：Station接口
- WIFI_IF_AP：SoftAP接口
- WIFI_IF_NAN：NAN接口

### wifi_auth_mode_t

WiFi认证模式枚举：

- WIFI_AUTH_OPEN：开放模式
- WIFI_AUTH_WEP：WEP模式
- WIFI_AUTH_WPA_PSK：WPA_PSK模式
- WIFI_AUTH_WPA2_PSK：WPA2_PSK模式
- WIFI_AUTH_WPA_WPA2_PSK：WPA_WPA2_PSK模式
- WIFI_AUTH_WPA3_PSK：WPA3_PSK模式
- WIFI_AUTH_WPA2_WPA3_PSK：WPA2_WPA3_PSK模式
- WIFI_AUTH_WAPI_PSK：WAPI_PSK模式

### wifi_ps_type_t

WiFi省电模式枚举：

- WIFI_PS_NONE：无省电
- WIFI_PS_MIN_MODEM：最小调制解调器省电
- WIFI_PS_MAX_MODEM：最大调制解调器省电

### wifi_bandwidth_t

WiFi带宽枚举：

- WIFI_BW_HT20：带宽20MHz
- WIFI_BW_HT40：带宽40MHz
- WIFI_BW80：带宽80MHz
- WIFI_BW160：带宽160MHz

### wifi_scan_type_t

WiFi扫描类型枚举：

- WIFI_SCAN_TYPE_ACTIVE：主动扫描
- WIFI_SCAN_TYPE_PASSIVE：被动扫描

### wifi_storage_t

WiFi存储类型枚举：

- WIFI_STORAGE_FLASH：配置存储在Flash中
- WIFI_STORAGE_RAM：配置存储在RAM中

## 1. WiFi初始化和配置函数

### esp_wifi_init
**函数原型：**
```c
esp_err_t esp_wifi_init(const wifi_init_config_t *config);
```
**参数：**
config：指向WiFi初始化配置结构体的指针，建议使用WIFI_INIT_CONFIG_DEFAULT()宏
**返回值：**
ESP_OK：操作成功
ESP_ERR_NO_MEM：内存不足
其他：参考esp_err.h中的错误代码
**作用：**初始化WiFi驱动，分配WiFi控制结构、RX/TX缓冲区、WiFi NVS结构等资源，并启动WiFi任务

### esp_wifi_deinit
**函数原型：**
```c
esp_err_t esp_wifi_deinit(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未通过esp_wifi_init初始化
**作用：**释放esp_wifi_init分配的所有资源并停止WiFi任务

### esp_wifi_set_mode
**函数原型：**
```c
esp_err_t esp_wifi_set_mode(wifi_mode_t mode);
```
**参数：**
mode：WiFi工作模式（WIFI_MODE_STA/WIFI_MODE_AP/WIFI_MODE_APSTA/WIFI_MODE_NAN）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**设置WiFi工作模式为Station、SoftAP、Station+SoftAP或NAN模式

### esp_wifi_get_mode
**函数原型：**
```c
esp_err_t esp_wifi_get_mode(wifi_mode_t *mode);
```
**参数：**
mode：存储当前WiFi模式的指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取当前WiFi工作模式

### esp_wifi_set_config
**函数原型：**
```c
esp_err_t esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf);
```
**参数：**
interface：接口类型（WIFI_IF_STA/WIFI_IF_AP）
conf：Station或SoftAP配置参数
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_WIFI_MODE：模式无效
ESP_ERR_WIFI_PASSWORD：密码无效
ESP_ERR_WIFI_NVS：WiFi内部NVS错误
**作用：**设置STA、AP或NAN的配置参数，配置会存储在NVS中

### esp_wifi_get_config
**函数原型：**
```c
esp_err_t esp_wifi_get_config(wifi_interface_t interface, wifi_config_t *conf);
```
**参数：**
interface：接口类型
conf：存储配置参数的指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_IF：接口无效
**作用：**获取指定接口的配置参数

## 2. WiFi启动和停止函数

### esp_wifi_start
**函数原型：**
```c
esp_err_t esp_wifi_start(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NO_MEM：内存不足
ESP_ERR_WIFI_CONN：WiFi内部错误
ESP_FAIL：其他WiFi内部错误
**作用：**根据当前配置启动WiFi（创建station/soft-AP/NAN控制块并启动相应功能）

### esp_wifi_stop
**函数原型：**
```c
esp_err_t esp_wifi_stop(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**停止WiFi并释放相应的控制块

### esp_wifi_restore
**函数原型：**
```c
esp_err_t esp_wifi_restore(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**将WiFi持久化设置恢复为默认值（包括带宽、协议、配置、模式等）

## 3. WiFi连接和断开函数

### esp_wifi_connect
**函数原型：**
```c
esp_err_t esp_wifi_connect(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_WIFI_MODE：WiFi模式错误
ESP_ERR_WIFI_CONN：WiFi内部错误
ESP_ERR_WIFI_SSID：AP的SSID无效
**作用：**WiFi Station连接到AP（仅影响WIFI_MODE_STA或WIFI_MODE_APSTA模式）

### esp_wifi_disconnect
**函数原型：**
```c
esp_err_t esp_wifi_disconnect(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_FAIL：其他WiFi内部错误
**作用：**WiFi Station从AP断开连接

### esp_wifi_deauth_sta
**函数原型：**
```c
esp_err_t esp_wifi_deauth_sta(uint16_t aid);
```
**参数：**
aid：关联ID，0表示断开所有Station，否则断开指定ID的Station
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_MODE：WiFi模式错误
**作用：**断开所有或指定关联ID的Station

## 4. WiFi扫描函数

### esp_wifi_scan_start
**函数原型：**
```c
esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, bool block);
```
**参数：**
config：扫描配置参数，NULL表示使用默认配置
block：true表示阻塞调用直到扫描完成，false表示立即返回
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_WIFI_TIMEOUT：阻塞扫描超时
ESP_ERR_WIFI_STATE：WiFi仍在连接时调用扫描
**作用：**扫描所有可用的AP

### esp_wifi_scan_stop
**函数原型：**
```c
esp_err_t esp_wifi_scan_stop(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_WIFI_STATE：WiFi仍在连接时调用
**作用：**停止正在进行的扫描

### esp_wifi_scan_get_ap_num
**函数原型：**
```c
esp_err_t esp_wifi_scan_get_ap_num(uint16_t *number);
```
**参数：**
number：存储扫描到的AP数量
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取上次扫描中发现的AP数量

### esp_wifi_scan_get_ap_records
**函数原型：**
```c
esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);
```
**参数：**
number：输入时表示ap_records可容纳的最大AP数量，输出时表示实际返回的AP数量
ap_records：wifi_ap_record_t数组，用于存储找到的AP
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NO_MEM：内存不足
**作用：**获取扫描到的AP列表（按RSSI降序排列），此API会释放扫描AP列表占用的所有内存

### esp_wifi_scan_get_ap_record
**函数原型：**
```c
esp_err_t esp_wifi_scan_get_ap_record(wifi_ap_record_t *ap_record);
```
**参数：**
ap_record：指向单个AP记录的指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
ESP_FAIL：扫描AP为空
**作用：**每次从扫描AP列表中获取一条AP记录

### esp_wifi_clear_ap_list
**函数原型：**
```c
esp_err_t esp_wifi_clear_ap_list(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_WIFI_MODE：WiFi模式错误
ESP_ERR_INVALID_ARG：参数无效
**作用：**清除上次扫描中找到的AP列表，释放所有占用的内存

### esp_wifi_set_scan_parameters
**函数原型：**
```c
esp_err_t esp_wifi_set_scan_parameters(const wifi_scan_default_params_t *config);
```
**参数：**
config：默认扫描配置参数，NULL表示重置为默认值
**返回值：**
ESP_OK：操作成功
ESP_FAIL：station模式尚未启动
ESP_ERR_INVALID_ARG：参数值不满足要求
ESP_ERR_NOT_SUPPORTED：AP模式尚不支持此API
ESP_ERR_INVALID_STATE：扫描/连接正在进行中
**作用：**设置Station扫描使用的默认参数

### esp_wifi_get_scan_parameters
**函数原型：**
```c
esp_err_t esp_wifi_get_scan_parameters(wifi_scan_default_params_t *config);
```
**参数：**
config：存储扫描默认参数的结构体
**返回值：**
ESP_OK：操作成功
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取Station使用的默认扫描参数

## 5. WiFi状态查询函数

### esp_wifi_sta_get_ap_info
**函数原型：**
```c
esp_err_t esp_wifi_sta_get_ap_info(wifi_ap_record_t *ap_info);
```
**参数：**
ap_info：存储AP信息的wifi_ap_record_t结构体
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_CONN：station接口未初始化
ESP_ERR_WIFI_NOT_CONNECT：station处于断开状态
**作用：**获取设备当前连接的AP的信息

### esp_wifi_ap_get_sta_list
**函数原型：**
```c
esp_err_t esp_wifi_ap_get_sta_list(wifi_sta_list_t *sta);
```
**参数：**
sta：存储连接到SoftAP的station列表
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_MODE：WiFi模式错误
ESP_ERR_WIFI_CONN：WiFi内部错误
**作用：**获取与SoftAP关联的Station列表

### esp_wifi_ap_get_sta_aid
**函数原型：**
```c
esp_err_t esp_wifi_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid);
```
**参数：**
mac：STA的MAC地址
aid：存储与STA对应的AID
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NOT_FOUND：未找到请求的资源
ESP_ERR_WIFI_MODE：WiFi模式错误
ESP_ERR_WIFI_CONN：WiFi内部错误
**作用：**获取连接到SoftAP的STA的AID

## 6. WiFi电源管理函数

### esp_wifi_set_ps
**函数原型：**
```c
esp_err_t esp_wifi_set_ps(wifi_ps_type_t type);
```
**参数：**
type：省电模式类型（WIFI_PS_NONE/WIFI_PS_MIN_MODEM/WIFI_PS_MAX_MODEM）
**返回值：**
ESP_OK：操作成功
**作用：**设置当前WiFi省电类型，默认为WIFI_PS_MIN_MODEM

### esp_wifi_get_ps
**函数原型：**
```c
esp_err_t esp_wifi_get_ps(wifi_ps_type_t *type);
```
**参数：**
type：存储当前省电类型
**返回值：**
ESP_OK：操作成功
**作用：**获取当前WiFi省电类型

### esp_wifi_set_inactive_time
**函数原型：**
```c
esp_err_t esp_wifi_set_inactive_time(wifi_interface_t ifx, uint16_t sec);
```
**参数：**
ifx：要配置的接口
sec：不活动时间（单位：秒）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
**作用：**设置STA或AP的不活动时间（STA默认6秒，AP默认300秒）

### esp_wifi_get_inactive_time
**函数原型：**
```c
esp_err_t esp_wifi_get_inactive_time(wifi_interface_t ifx, uint16_t *sec);
```
**参数：**
ifx：要配置的接口
sec：存储不活动时间
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取指定接口的不活动时间

### esp_wifi_force_wakeup_acquire
**函数原型：**
```c
esp_err_t esp_wifi_force_wakeup_acquire(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
**作用：**请求额外的WiFi无线电引用，保持WiFi活动状态（RF打开）以接收数据包

### esp_wifi_force_wakeup_release
**函数原型：**
```c
esp_err_t esp_wifi_force_wakeup_release(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
**作用：**释放额外的WiFi无线电引用，如果不再使用无线电，WiFi进入睡眠状态（RF关闭）

## 7. WiFi协议和带宽配置函数

### esp_wifi_set_protocol
**函数原型：**
```c
esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap);
```
**参数：**
ifx：接口类型
protocol_bitmap：WiFi协议位图（WIFI_PROTOCOL_11B/11G/11N/11A/11AC/11AX等）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NOT_SUPPORTED：当前频段模式不支持此API
**作用：**设置指定接口的协议类型

### esp_wifi_get_protocol
**函数原型：**
```c
esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap);
```
**参数：**
ifx：接口类型
protocol_bitmap：存储当前WiFi协议位图
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NOT_SUPPORTED：当前频段模式不支持此API
**作用：**获取指定接口的当前协议位图

### esp_wifi_set_bandwidth
**函数原型：**
```c
esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw);
```
**参数：**
ifx：要配置的接口
bw：带宽（WIFI_BW_HT20/WIFI_BW_HT40等）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NOT_SUPPORTED：当前频段模式不支持此API
**作用：**设置指定接口的带宽

### esp_wifi_get_bandwidth
**函数原型：**
```c
esp_err_t esp_wifi_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw);
```
**参数：**
ifx：要配置的接口
bw：存储带宽值
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NOT_SUPPORTED：当前频段模式不支持此API
**作用：**获取指定接口的带宽

### esp_wifi_set_channel
**函数原型：**
```c
esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
```
**参数：**
primary：主信道号（HT20时为信道号，HT40时为主信道）
second：副信道（HT20时忽略，HT40时为副信道）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
**作用：**设置设备的主/副信道

### esp_wifi_get_channel
**函数原型：**
```c
esp_err_t esp_wifi_get_channel(uint8_t *primary, wifi_second_chan_t *second);
```
**参数：**
primary：存储当前主信道
second：存储当前副信道
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取设备的主/副信道

## 8. WiFi MAC地址和国家代码函数

### esp_wifi_set_mac
**函数原型：**
```c
esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, const uint8_t mac[6]);
```
**参数：**
ifx：接口类型
mac：MAC地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_WIFI_MAC：MAC地址无效
ESP_ERR_WIFI_MODE：WiFi模式错误
**作用：**设置WiFi station、soft-AP或NAN接口的MAC地址

### esp_wifi_get_mac
**函数原型：**
```c
esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t mac[6]);
```
**参数：**
ifx：接口类型
mac：存储MAC地址
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_IF：接口无效
**作用：**获取指定接口的MAC地址

### esp_wifi_set_country
**函数原型：**
```c
esp_err_t esp_wifi_set_country(const wifi_country_t *country);
```
**参数：**
country：配置的国家信息
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**配置国家信息（不推荐使用，建议使用esp_wifi_set_country_code）

### esp_wifi_get_country
**函数原型：**
```c
esp_err_t esp_wifi_get_country(wifi_country_t *country);
```
**参数：**
country：存储当前国家信息
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取当前国家信息

### esp_wifi_set_country_code
**函数原型：**
```c
esp_err_t esp_wifi_set_country_code(const char *country, bool ieee80211d_enabled);
```
**参数：**
country：配置的国家ISO代码
ieee80211d_enabled：是否启用802.11d
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**配置国家代码

### esp_wifi_get_country_code
**函数原型：**
```c
esp_err_t esp_wifi_get_country_code(char *country);
```
**参数：**
country：存储国家代码
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取当前国家代码

## 9. WiFi混杂模式函数

### esp_wifi_set_promiscuous
**函数原型：**
```c
esp_err_t esp_wifi_set_promiscuous(bool en);
```
**参数：**
en：false-禁用，true-启用
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**启用或禁用混杂模式

### esp_wifi_get_promiscuous
**函数原型：**
```c
esp_err_t esp_wifi_get_promiscuous(bool *en);
```
**参数：**
en：存储混杂模式的当前状态
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取混杂模式状态

### esp_wifi_set_promiscuous_rx_cb
**函数原型：**
```c
esp_err_t esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);
```
**参数：**
cb：回调函数指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**注册混杂模式下的RX回调函数

### esp_wifi_set_promiscuous_filter
**函数原型：**
```c
esp_err_t esp_wifi_set_promiscuous_filter(const wifi_promiscuous_filter_t *filter);
```
**参数：**
filter：混杂模式下要过滤的数据包类型
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**启用混杂模式数据包类型过滤器

### esp_wifi_get_promiscuous_filter
**函数原型：**
```c
esp_err_t esp_wifi_get_promiscuous_filter(wifi_promiscuous_filter_t *filter);
```
**参数：**
filter：存储混杂过滤器的当前状态
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取混杂过滤器配置

### esp_wifi_set_promiscuous_ctrl_filter
**函数原型：**
```c
esp_err_t esp_wifi_set_promiscuous_ctrl_filter(const wifi_promiscuous_filter_t *filter);
```
**参数：**
filter：控制数据包的子类型过滤器
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**启用混杂模式下控制数据包的子类型过滤器

### esp_wifi_get_promiscuous_ctrl_filter
**函数原型：**
```c
esp_err_t esp_wifi_get_promiscuous_ctrl_filter(wifi_promiscuous_filter_t *filter);
```
**参数：**
filter：存储控制数据包子类型过滤器的当前状态
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取混杂模式下控制数据包的子类型过滤器

## 10. WiFi高级功能函数

### esp_wifi_set_max_tx_power
**函数原型：**
```c
esp_err_t esp_wifi_set_max_tx_power(int8_t power);
```
**参数：**
power：最大WiFi发射功率（单位：0.25dBm，范围：8~84，对应2dBm~20dBm）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
**作用：**WiFi启动后设置最大发射功率

### esp_wifi_get_max_tx_power
**函数原型：**
```c
esp_err_t esp_wifi_get_max_tx_power(int8_t *power);
```
**参数：**
power：存储最大WiFi发射功率（单位：0.25dBm）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动
ESP_ERR_INVALID_ARG：参数无效
**作用：**WiFi启动后获取最大发射功率

### esp_wifi_set_storage
**函数原型：**
```c
esp_err_t esp_wifi_set_storage(wifi_storage_t storage);
```
**参数：**
storage：存储类型（WIFI_STORAGE_FLASH/WIFI_STORAGE_RAM）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**设置WiFi API配置存储类型，默认为WIFI_STORAGE_FLASH

### esp_wifi_set_vendor_ie
**函数原型：**
```c
esp_err_t esp_wifi_set_vendor_ie(bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, const void *vnd_ie);
```
**参数：**
enable：true-启用指定IE，false-移除指定IE
type：信息元素类型
idx：索引（0或1）
vnd_ie：指向供应商特定元素数据的指针
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_NO_MEM：内存不足
**作用：**设置802.11供应商特定信息元素

### esp_wifi_set_vendor_ie_cb
**函数原型：**
```c
esp_err_t esp_wifi_set_vendor_ie_cb(esp_vendor_ie_cb_t cb, void *ctx);
```
**参数：**
cb：回调函数
ctx：传递给回调函数的上下文参数
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**注册供应商特定信息元素监控回调

### esp_wifi_80211_tx
**函数原型：**
```c
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
```
**参数：**
ifx：接口类型
buffer：原始ieee80211缓冲区
len：原始缓冲区长度（必须>=24字节且<=1500字节）
en_sys_seq：是否使用内部序列号
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_IF：接口无效
ESP_ERR_INVALID_ARG：参数无效
ESP_ERR_WIFI_NO_MEM：内存不足
**作用：**发送原始ieee80211数据

### esp_wifi_register_80211_tx_cb
**函数原型：**
```c
esp_err_t esp_wifi_register_80211_tx_cb(esp_wifi_80211_tx_done_cb_t cb);
```
**参数：**
cb：回调函数，NULL表示注销
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**注册80211 tx数据的TX回调函数

### esp_wifi_set_rssi_threshold
**函数原型：**
```c
esp_err_t esp_wifi_set_rssi_threshold(int32_t rssi);
```
**参数：**
rssi：RSSI阈值（单位：dBm，范围：-100~10）
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**设置RSSI阈值，当平均RSSI低于阈值时，WiFi任务将发布WIFI_EVENT_STA_BSS_RSSI_LOW事件

### esp_wifi_get_tsf_time
**函数原型：**
```c
int64_t esp_wifi_get_tsf_time(wifi_interface_t interface);
```
**参数：**
interface：要获取tsf_time的接口
**返回值：**
0或TSF时间
**作用：**获取TSF时间

### esp_wifi_set_event_mask
**函数原型：**
```c
esp_err_t esp_wifi_set_event_mask(uint32_t mask);
```
**参数：**
mask：WiFi事件掩码
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**设置掩码以启用或禁用某些WiFi事件

### esp_wifi_get_event_mask
**函数原型：**
```c
esp_err_t esp_wifi_get_event_mask(uint32_t *mask);
```
**参数：**
mask：存储WiFi事件掩码
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取WiFi事件掩码

## 11. WiFi CSI（信道状态信息）函数

### esp_wifi_set_csi_rx_cb
**函数原型：**
```c
esp_err_t esp_wifi_set_csi_rx_cb(wifi_csi_cb_t cb, void *ctx);
```
**参数：**
cb：回调函数
ctx：传递给回调函数的上下文参数
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
**作用：**注册CSI数据的RX回调函数

### esp_wifi_set_csi_config
**函数原型：**
```c
esp_err_t esp_wifi_set_csi_config(const wifi_csi_config_t *config);
```
**参数：**
config：CSI配置
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动或混杂模式未启用
ESP_ERR_INVALID_ARG：参数无效
**作用：**设置CSI数据配置

### esp_wifi_get_csi_config
**函数原型：**
```c
esp_err_t esp_wifi_get_csi_config(wifi_csi_config_t *config);
```
**参数：**
config：存储CSI配置
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动或混杂模式未启用
ESP_ERR_INVALID_ARG：参数无效
**作用：**获取CSI数据配置

### esp_wifi_set_csi
**函数原型：**
```c
esp_err_t esp_wifi_set_csi(bool en);
```
**参数：**
en：true-启用，false-禁用
**返回值：**
ESP_OK：操作成功
ESP_ERR_WIFI_NOT_INIT：WiFi未初始化
ESP_ERR_WIFI_NOT_STARTED：WiFi未启动或混杂模式未启用
ESP_ERR_INVALID_ARG：参数无效
**作用：**启用或禁用CSI

## 12. WiFi FTM（精细时间测量）函数

### esp_wifi_ftm_initiate_session
**函数原型：**
```c
esp_err_t esp_wifi_ftm_initiate_session(wifi_ftm_initiator_cfg_t *cfg);
```
**参数：**
cfg：FTM发起者会话配置
**返回值：**
ESP_OK：操作成功
其他：失败
**作用：**通过发送FTM请求启动FTM发起者会话

### esp_wifi_ftm_end_session
**函数原型：**
```c
esp_err_t esp_wifi_ftm_end_session(void);
```
**参数：**
无
**返回值：**
ESP_OK：操作成功
其他：失败
**作用：**结束正在进行的FTM发起者会话

### esp_wifi_ftm_resp_set_offset
**函数原型：**
```c
esp_err_t esp_wifi_ftm_resp_set_offset(int16_t offset_cm);
```
**参数：**
offset_cm：以厘米为单位添加的T1偏移
**返回值：**
ESP_OK：操作成功
其他：失败
**作用：**设置FTM响应者的偏移量（厘米）

### esp_wifi_ftm_get_report
**函数原型：**
```c
esp_err_t esp_wifi_ftm_get_report(wifi_ftm_report_entry_t *report, uint8_t num_entries);
```
**参数：**
report：接收FTM报告的缓冲区指针
num_entries：要填充在报告中的FTM报告条目数
**返回值：**
ESP_OK：操作成功
其他：失败
**作用：**获取复制到用户提供缓冲区的FTM测量报告

## 13. WiFi其他配置函数

### esp_wifi_config_11b_rate
**函数原型：**
```c
esp_err_t esp_wifi_config_11b_rate(wifi_interface_t ifx, bool disable);
```
**参数：**
ifx：要配置的接口
disable：true表示禁用11b速率，false表示启用11b速率
**返回值：**
ESP_OK：操作成功
其他：失败
**作用：**启用或禁用指定接口的11b速率

### esp_wifi_connectionless_module_set_wake_interval
**函数原型：**
```c
esp_err_t esp_wifi_connectionless_module_set_wake_interval(uint16_t wake_interval);
```
**参数：**
wake_interval：芯片唤醒后的毫秒数（范围：1~65535）
**返回值：**
ESP_OK：操作成功
**作用：**设置无连接模块的唤醒间隔以定期唤醒

### esp_wifi_statis_dump
**函数原型：**
```c
esp_err_t esp_wifi_statis_dump(uint32_t modules);
```
**参数：**
modules：要转储的统计模块
**返回值：**
ESP_OK：操作成功
其他：失败
**作用：**转储WiFi统计信息

- 
