/**
 * @details ESP-IDF I2C配置步骤：
 *          1. 配置总线参数 i2c_master_bus_config_t
 *          2. 配置从设备参数 i2c_device_config_t 
 * @note - BSP层负责I2C总线管理
 *       - chip层负责具体芯片驱动
 */

#include "i2c.h"
#include "esp_log.h"

/* I2C主机总线句柄 */
i2c_master_bus_handle_t i2c_bus_handle = NULL;

/**
 * @brief 初始化I2C总线
 * @details 配置I2C主机总线参数并创建总线句柄。
 *          本函数只初始化总线，不添加设备。
 */
void i2c_bus_init(void)
{   
    i2c_master_bus_config_t bus_config = {0};
    bus_config.i2c_port = I2C_NUM_0;                /* I2C端口号 */
    bus_config.scl_io_num = GPIO_NUM_42;            /* SCL引脚 */
    bus_config.sda_io_num = GPIO_NUM_41;            /* SDA引脚 */
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;    /* 时钟源 */
    bus_config.flags.enable_internal_pullup = true; /* 内部上拉 */
    bus_config.glitch_ignore_cnt = 7;               /* 滤除时间小于7个时钟周期的毛刺, */

    esp_err_t ret = i2c_new_master_bus(&bus_config, &i2c_bus_handle);
    if (ret == ESP_OK) {
        ESP_LOGI("I2C", "I2C总线初始化成功");
    } else {
        ESP_LOGE("I2C", "I2C总线初始化失败: %s", esp_err_to_name(ret));
    }
}

/**
 * @brief 添加I2C从设备到总线
 * @details 配置从设备参数并将其添加到I2C总线上, 每个从设备需要单独添加。
 * @warning 调用前必须先调用 i2c_bus_init()
 */
void i2c_add_device(uint8_t device_address, i2c_master_dev_handle_t *dev_handle)
{
    i2c_device_config_t device_config = {0};
    device_config.dev_addr_length = I2C_ADDR_BIT_LEN_7; /* 设备地址长度 */
    device_config.device_address = device_address;      /* 纯设备地址, 不包含读写位 */
    device_config.scl_speed_hz = 100000;                /* I2C时钟频率 */
    device_config.scl_wait_us = 0;                      /* 每个SCL时钟周期后, 额外等待指定的微秒数 */
    device_config.flags.disable_ack_check = false;      /* 启用ACK检查 */

    i2c_master_bus_add_device(i2c_bus_handle, &device_config, dev_handle);
}

/**
 * @brief 探测I2C设备是否在线
 * @details 通过发送设备地址检测设备是否响应ACK。
 * @return - ESP_OK: 设备在线，成功响应
 *         - ESP_ERR_NOT_FOUND: 设备未响应
 *         - 其他错误码: 通信错误
 */
esp_err_t i2c_detect_device(uint16_t device_address)
{
    return i2c_master_probe(i2c_bus_handle, device_address, 5000);
}

/**
 * @brief 向I2C设备发送数据
 * @details 通过I2C总线向指定设备发送数据, 使用阻塞模式，超时时间为5000ms。 
 * @param[in] dev_handle I2C设备句柄
 * @param[in] data 待发送的数据缓冲区指针
 * @param[in] data_len 待发送的数据长度
 * @return ESP_OK: 成功; 其他: 错误码
 */
esp_err_t i2c_transmit_data(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t data_len)
{
    return i2c_master_transmit(dev_handle, data, data_len, 5000);
}

/**
 * @brief 从I2C设备接收数据
 * @details 通过I2C总线从指定设备接收数据, 使用阻塞模式，超时时间为5000ms。
 * @param[in] dev_handle I2C设备句柄
 * @param[out] data 接收数据的缓冲区指针
 * @param[in] data_len 期望接收的数据长度
 * @return ESP_OK: 成功; 其他: 错误码
 */
esp_err_t i2c_receive_data(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t data_len)
{
    return i2c_master_receive(dev_handle, data, data_len, 5000);
}

/**
 * @brief I2C发送并接收数据
 * @details 执行I2C组合写-读操作，先向设备发送数据，然后立即从同一设备接收数据，
 *          在操作之间不释放总线。常用于寄存器读取等操作，先把要读的寄存器地址写入。
 * @param[in] dev_handle I2C设备句柄
 * @param[in] tx_data 待发送的数据缓冲区指针
 * @param[in] tx_data_len 待发送的数据长度
 * @param[out] rx_data 接收数据的缓冲区指针
 * @param[in] rx_data_len 期望接收的数据长度
 * @return ESP_OK: 成功; 其他: 错误码
 */
esp_err_t i2c_transmit_receive_data(i2c_master_dev_handle_t dev_handle, 
                           uint8_t *tx_data, size_t tx_data_len, 
                           uint8_t *rx_data, size_t rx_data_len)
{
    return i2c_master_transmit_receive(dev_handle, tx_data, tx_data_len, rx_data, rx_data_len, 5000);
}

/**
 * @brief I2C总线扫描功能
 * @details 扫描I2C总线上的所有设备，打印找到的设备地址
 */
void i2c_scan_bus(void)
{
    ESP_LOGI("I2C_SCAN", "开始扫描I2C总线...");
    ESP_LOGI("I2C_SCAN", "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
    
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            fflush(stdout);
            uint8_t address = i + j;
            if (address < 0x08 || address > 0x77) {
                printf("   ");
            } else {
                esp_err_t ret = i2c_master_probe(i2c_bus_handle, address, 1000);
                if (ret == ESP_OK) {
                    printf("%02x ", address);
                } else {
                    printf("-- ");
                }
            }
        }
        printf("\r\n");
    }
}