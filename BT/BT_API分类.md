# ESP32蓝牙API分类指南

## 📌 核心概念

在BT/api文件夹中的所有头文件都提供给用户使用，分为以下几类：

### ✅ 当前活跃的主流API (推荐使用)

这些是Espressif官方推荐的，功能完整的API接口：

---

## 🔷 1. 蓝牙栈基础管理 (3个文件)

### 用途
初始化和管理Bluedroid蓝牙协议栈的生命周期

### 文件
- **`esp_bt_main.h`** - 主协议栈管理
  - `esp_bluedroid_init()` - 初始化Bluedroid栈
  - `esp_bluedroid_enable()` / `esp_bluedroid_disable()` - 使能/禁用
  - `esp_bluedroid_get_status()` - 获取栈状态
  - 配置SSP和安全连接选项

- **`esp_bt_device.h`** - 本地设备管理
  - 获取/设置蓝牙地址
  - 获取/设置设备名称
  - 设备信息查询

- **`esp_bt_defs.h`** - 基础数据定义
  - 蓝牙地址类型 (`esp_bd_addr_t`)
  - UUID定义
  - 设备类型常数

---

## 🔷 2. BLE(低功耗蓝牙)核心 (7个文件)

### 用途
完整的BLE蓝牙5.0/5.1/5.2功能支持

### 核心文件

#### **`esp_gap_ble_api.h`** - BLE GAP(通用访问配置文件)
**功能**: 广播、扫描、连接、配对等基础连接功能
**常用API**:
- `esp_ble_gap_set_device_name()` - 设置设备名
- `esp_ble_gap_config_adv_data()` - 配置广告数据
- `esp_ble_gap_start_advertising()` - 开始广播
- `esp_ble_gap_set_scan_params()` - 配置扫描参数
- `esp_ble_gap_start_scanning()` - 开始扫描
- `esp_ble_gap_connect_with_addrtype()` - 建立连接
- `esp_ble_gap_set_security_param()` - 设置安全参数

#### **`esp_gattc_api.h`** - BLE GATT客户端
**功能**: 作为GATT客户端连接BLE设备，读写特征值
**常用API**:
- `esp_ble_gattc_register_callback()` - 注册回调
- `esp_ble_gattc_app_register()` - 注册应用
- `esp_ble_gattc_open()` - 打开连接
- `esp_ble_gattc_search_service()` - 服务发现
- `esp_ble_gattc_read_char()` - 读取特征值
- `esp_ble_gattc_write_char()` - 写入特征值
- `esp_ble_gattc_register_for_notify()` - 注册通知

#### **`esp_gatts_api.h`** - BLE GATT服务器
**功能**: 创建GATT服务和特征，响应客户端请求
**常用API**:
- `esp_ble_gatts_register_callback()` - 注册回调
- `esp_ble_gatts_app_register()` - 注册应用
- `esp_ble_gatts_create_service()` - 创建服务
- `esp_ble_gatts_add_char()` - 添加特征
- `esp_ble_gatts_send_indicate()` - 发送通知
- `esp_ble_gatts_send_response()` - 发送响应

#### **`esp_gatt_common_api.h`** - GATT通用功能
**功能**: GATT客户端和服务器的公共功能

#### **`esp_gatt_defs.h`** - GATT数据定义
**功能**: GATT相关的数据结构、常量、枚举

#### **`esp_ble_cte_api.h`** - BLE恒定音调扩展(CTE)
**功能**: 蓝牙5.1方向查找功能，用于室内定位
**常用API**:
- `esp_ble_cte_enable_initiator()` - 启用发起端CTE
- `esp_ble_cte_disable_initiator()` - 禁用发起端CTE
- `esp_ble_cte_set_iq_samples_enable()` - IQ样本采集

#### **`esp_ble_iso_api.h`** - BLE同步通道(ISO)
**功能**: 蓝牙5.2等时同步通道，用于音频/数据流传输
**常用API**:
- `esp_ble_iso_create_big()` - 创建ISO广播组
- `esp_ble_iso_establish_cis()` - 建立连接的等时通道
- `esp_ble_iso_send_data()` - 发送ISO数据

---

## 🔷 3. 经典蓝牙GAP (1个文件)

### **`esp_gap_bt_api.h`** - 经典蓝牙GAP
**功能**: 设备发现、配对、连接、安全配置

**常用API**:
- `esp_bt_gap_set_scan_mode()` - 设置扫描模式（可被发现）
- `esp_bt_gap_start_discovery()` - 开始设备发现
- `esp_bt_gap_cancel_discovery()` - 取消发现
- `esp_bt_gap_remove_bond_device()` - 移除配对设备
- `esp_bt_gap_set_pin()` - 设置PIN码
- `esp_bt_gap_ssp_passkey_reply()` - SSP密钥回复
- `esp_bt_gap_set_device_name()` - 设置设备名
- `esp_bt_gap_get_device_name()` - 获取设备名

---

## 🔷 4. 音频配置文件 (8个文件)

### 用途
支持蓝牙音频传输和远程控制

#### **A2DP (高级音频分发配置文件)**
- **`esp_a2dp_api.h`** - A2DP主API
  - 音频源(Source)：播放音乐
  - 音频汇(Sink)：接收音乐
  - `esp_a2d_source_init()` - 初始化音频源
  - `esp_a2d_sink_init()` - 初始化音频汇
  - `esp_a2d_source_connect()` - 连接音频源
  - `esp_a2d_source_disconnect()` - 断开连接

- **`esp_a2dp_legacy_api.h`** - A2DP旧版API ⚠️
  - 向后兼容接口
  - 新项目不建议使用

#### **AVRCP (音视频远程控制配置文件)**
- **`esp_avrc_api.h`** - 远程控制
  - 播放/暂停/停止控制
  - 音量调节
  - 获取播放状态
  - `esp_avrc_ct_init()` - 初始化控制端
  - `esp_avrc_tg_init()` - 初始化目标端
  - `esp_avrc_ct_send_passthrough_cmd()` - 发送控制命令

#### **免提配置文件(HFP)**
- **`esp_hf_client_api.h`** - 免提客户端(HF Client) ✅ 推荐
  - 作为免提设备连接手机等音频网关
  - `esp_hf_client_init()` - 初始化
  - `esp_hf_client_connect()` - 连接
  - `esp_hf_client_dial()` - 拨号
  - `esp_hf_client_answer_call()` - 接听
  - `esp_hf_client_volume_update()` - 音量调节
  - 音频数据回调处理

- **`esp_hf_ag_api.h`** - 音频网关(HF AG) ✅ 推荐
  - 作为音频网关接受免提设备连接（如手机功能）
  - `esp_hf_ag_init()` - 初始化
  - `esp_hf_ag_slc_connect()` - 建立连接
  - `esp_hf_ag_devices_status_indchange()` - 设备状态指示
  - `esp_hf_ag_cind_response()` - 能力响应
  - 音频数据回调处理

- **`esp_hf_client_legacy_api.h`** - 免提客户端旧版API ⚠️
  - 向后兼容
  - 包含: `esp_hf_client_register_data_callback()`

- **`esp_hf_ag_legacy_api.h`** - 音频网关旧版API ⚠️
  - 向后兼容
  - 包含: `esp_hf_ag_register_data_callback()`

- **`esp_hf_defs.h`** - HFP数据定义
  - 通话状态枚举
  - 操作类型定义
  - 错误代码

---

## 🔷 5. 数据传输配置文件 (2个文件)

### 用途
提供面向连接的数据通道

#### **`esp_spp_api.h`** - SPP(串口配置文件) ✅ 推荐
**功能**: 虚拟串口通信，透明数据传输
**常用API**:
- `esp_spp_enhanced_init()` - 初始化(推荐用这个，`esp_spp_init()`已弃用)
- `esp_spp_start_srv()` - 启动服务器
- `esp_spp_connect()` - 建立连接
- `esp_spp_disconnect()` - 断开连接
- `esp_spp_write()` - 发送数据
- `esp_spp_vfs_register()` - 注册VFS接口（虚拟文件系统）
- `esp_spp_get_profile_status()` - 获取状态

#### **`esp_l2cap_bt_api.h`** - L2CAP(逻辑链路控制和适配协议)
**功能**: 经典蓝牙面向连接的通道
**常用API**:
- `esp_bt_l2cap_init()` - 初始化
- `esp_bt_l2cap_start_srv()` - 启动服务
- `esp_bt_l2cap_connect()` - 建立连接
- `esp_bt_l2cap_vfs_register()` - VFS支持

---

## 🔷 6. 人机界面设备(HID) (2个文件)

### 用途
支持键盘、鼠标、游戏手柄等HID设备

#### **`esp_hidd_api.h`** - HID设备(HIDD)
**功能**: 让ESP32作为蓝牙键盘/鼠标/遥控器
**常用API**:
- `esp_bt_hid_device_init()` - 初始化
- `esp_bt_hid_device_register_app()` - 注册应用
- `esp_bt_hid_device_connect()` - 连接到主机
- `esp_bt_hid_device_send_report()` - 发送HID报告（按键、鼠标事件）
- `esp_bt_hid_device_disconnect()` - 断开连接

#### **`esp_hidh_api.h`** - HID主机(HIDH)
**功能**: 让ESP32连接和控制蓝牙HID设备
**常用API**:
- `esp_bt_hid_host_init()` - 初始化
- `esp_bt_hid_host_connect()` - 连接HID设备
- `esp_bt_hid_host_disconnect()` - 断开连接
- `esp_bt_hid_host_get_protocol()` - 获取协议模式
- `esp_bt_hid_host_set_protocol()` - 设置协议模式
- `esp_bt_hid_host_get_report()` - 获取报告
- `esp_bt_hid_host_set_report()` - 发送报告

---

## 🔷 7. 电话簿访问(PBAP) (2个文件)

### 用途
访问远程蓝牙设备的联系人、电话簿等

#### **`esp_pbac_api.h`** - PBAP客户端
**功能**: 从手机等设备获取电话簿信息
**常用API**:
- `esp_pbac_init()` - 初始化
- `esp_pbac_connect()` - 连接到PBAP服务器
- `esp_pbac_pull_phone_book()` - 获取电话簿
- `esp_pbac_pull_vcard_listing()` - 获取vCard列表
- `esp_pbac_pull_vcard_entry()` - 获取单个vCard

#### **`esp_pba_defs.h`** - PBAP数据定义
**功能**: PBAP相关的数据结构和常量

---

## 🔷 8. 服务发现(SDP) (1个文件)

### **`esp_sdp_api.h`** - SDP(服务发现协议)
**功能**: 查询蓝牙设备可用的服务，发布本地服务
**常用API**:
- `esp_sdp_init()` - 初始化
- `esp_sdp_search_record()` - 搜索远程服务记录
- `esp_sdp_create_record()` - 创建本地服务记录
- `esp_sdp_remove_record()` - 移除服务记录

---

## 🔷 9. 其他支持文件 (2个文件)

#### **`esp_bluedroid_hci.h`** - HCI接口
**功能**: 主机控制器接口（通常内部使用）

#### **`esp_bt_vs.h`** - 厂商特定命令 ⚠️
**功能**: ESP32特定的蓝牙控制器命令
**注意**: 这是内部使用的厂商扩展，一般用户不需要使用

---

## 📊 API使用场景速查表

| 应用场景 | 推荐文件 | 关键API |
|---------|---------|--------|
| **BLE信标(广播)** | esp_gap_ble_api.h | `esp_ble_gap_config_adv_data()` |
| **BLE扫描/发现** | esp_gap_ble_api.h | `esp_ble_gap_set_scan_params()` |
| **BLE传感器(服务器)** | esp_gatts_api.h | `esp_ble_gatts_create_service()` |
| **BLE客户端** | esp_gattc_api.h | `esp_ble_gattc_read_char()` |
| **经典蓝牙配对** | esp_gap_bt_api.h | `esp_bt_gap_start_discovery()` |
| **虚拟串口(SPP)** | esp_spp_api.h | `esp_spp_enhanced_init()` |
| **音频流传输** | esp_a2dp_api.h | `esp_a2d_source_init()` |
| **蓝牙键盘** | esp_hidd_api.h | `esp_bt_hid_device_send_report()` |
| **免提通话** | esp_hf_client_api.h | `esp_hf_client_dial()` |
| **远程控制** | esp_avrc_api.h | `esp_avrc_ct_send_passthrough_cmd()` |

---

## ⚠️ 已弃用的API

这些API仍然可用但不推荐新项目使用：

1. **`esp_spp_init()`** (在esp_spp_api.h)
   - 替代: `esp_spp_enhanced_init()`

2. **`esp_hf_client_legacy_api.h`** 中的函数
   - 替代: esp_hf_client_api.h中的新API

3. **`esp_hf_ag_legacy_api.h`** 中的函数
   - 替代: esp_hf_ag_api.h中的新API

4. **`esp_a2dp_legacy_api.h`** 中的函数
   - 替代: esp_a2dp_api.h中的新API

---

## 🎯 快速开发指南

### 最小化BLE设备
1. 包含: `esp_bt_main.h`, `esp_gap_ble_api.h`, `esp_gatt_common_api.h`
2. 初始化: Bluedroid → GAP → GATT

### 完整蓝牙应用
- 经典蓝牙: 添加 `esp_gap_bt_api.h`
- GATT服务: 添加 `esp_gatts_api.h`
- GATT客户端: 添加 `esp_gattc_api.h`
- SPP数据传输: 添加 `esp_spp_api.h`

### 音频应用
- 蓝牙音箱: A2DP + SPP
- 免提通话: HF AG或HF Client
- 远程控制: AVRCP + A2DP

---

## 📝 总结

所有27个文件中的API都是提供给用户使用的，分为：

- **核心必需** (6文件): 栈管理、BLE GAP/GATT
- **应用层** (15文件): A2DP、HFP、SPP、HID等具体配置文件  
- **工具/支持** (6文件): 数据定义、SDP、PBAP等

**关键原则**: 
- 优先使用非legacy版本的API
- 根据应用场景选择相应的Profile（配置文件）
- 注意`esp_err_t`返回值，处理可能的错误
