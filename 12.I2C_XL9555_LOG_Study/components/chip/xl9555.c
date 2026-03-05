#include "xl9555.h"
#include "esp_log.h"

static const char *TAG = "XL9555";

/* XL9555设备的I2C句柄 */
i2c_master_dev_handle_t xl9555_handle = NULL;

/**
 * @brief 写XL9555寄存器
 * @details 向XL9555的指定寄存器写入数据
 *
 * @param[in] reg 寄存器地址
 * @param[in] value 要写入的值
 * @return ESP_OK: 成功; 其他: 错误码
 */
static esp_err_t xl9555_write_register(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    esp_err_t ret = i2c_transmit_data(xl9555_handle, data, sizeof(data));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "写寄存器0x%02X失败: %s", reg, esp_err_to_name(ret));
    }
    return ret;
}

/**
 * @brief 读XL9555寄存器
 * @details 从XL9555的指定寄存器读取数据
 *
 * @param[in] reg 寄存器地址
 * @param[out] value 读取到的值的存储指针
 * @return ESP_OK: 成功; 其他: 错误码
 */
static esp_err_t xl9555_read_register(uint8_t reg, uint8_t *value)
{
    esp_err_t ret = i2c_transmit_receive_data(xl9555_handle, &reg, 1, value, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "读寄存器0x%02X失败: %s", reg, esp_err_to_name(ret));
    }
    return ret;
}

/**
 * @brief 初始化XL9555设备
 * @details 初始化I2C总线并添加XL9555设备到总线上
 * @note 调用此函数前请确保已正确配置I2C引脚
 * @return ESP_OK: 成功; 其他: 错误码
 */
esp_err_t xl9555_init(void)
{
    ESP_LOGI(TAG, "初始化XL9555设备...");
    
    // 初始化I2C总线
    i2c_bus_init();
    
    // 首先扫描I2C总线，查看有哪些设备
    ESP_LOGI(TAG, "扫描I2C总线以查找设备...");
    i2c_scan_bus();
    
    // 添加XL9555设备
    i2c_add_device(XL9555_ADDRESS, &xl9555_handle);
    
    // 检测设备是否在线
    esp_err_t ret = i2c_detect_device(XL9555_ADDRESS);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "XL9555设备检测成功，地址: 0x%02X", XL9555_ADDRESS);
    } else {
        ESP_LOGE(TAG, "XL9555设备检测失败，地址: 0x%02X, 错误: %s", XL9555_ADDRESS, esp_err_to_name(ret));
        return ret;
    }
    
    return ESP_OK;
}

/**
 * @brief 设置GPIO端口模式
 * @details 配置XL9555的GPIO端口为输入或输出模式
 *
 * @param[in] gpio GPIO端口号 (0: Port0, 1: Port1)
 * @param[in] mode 端口模式配置值
 *                 - 0x00: 所有引脚配置为输出
 *                 - 0xFF: 所有引脚配置为输入
 *                 - 其他值: 按位配置，1为输入，0为输出
 *
 * @note 仅支持端口0和端口1，其他值会导致未知行为
 * @return ESP_OK: 成功; 其他: 错误码
 */
esp_err_t xl9555_set_mode(uint8_t gpio, uint8_t mode)
{
    esp_err_t ret = ESP_FAIL;
    
    if (gpio == 0)
    {
        ret = xl9555_write_register(XL9555_REG_CONFIG_P0, mode);
        ESP_LOGI(TAG, "设置P0端口模式: 0x%02X", mode);
    }
    else if (gpio == 1)
    {
        ret = xl9555_write_register(XL9555_REG_CONFIG_P1, mode);
        ESP_LOGI(TAG, "设置P1端口模式: 0x%02X", mode);
    }
    else
    {
        ESP_LOGE(TAG, "无效的GPIO端口号: %d", gpio);
        return ESP_ERR_INVALID_ARG;
    }
    
    return ret;
}

/**
 * @brief 设置指定引脚的输出状态
 * @details 设置XL9555指定端口的指定引脚为高电平或低电平
 *
 * @param[in] port 端口号 (0: Port0, 1: Port1)
 * @param[in] pin 引脚号 (0-7)
 * @param[in] value 输出值 (0: 低电平, 非0: 高电平)
 *
 * @note 仅对配置为输出模式的引脚有效
 * @warning 引脚号超出范围(0-7)会导致未定义行为
 */
void xl9555_set_pin(uint8_t port, uint8_t pin, uint8_t value)
{
    uint8_t reg = 0;
    uint8_t current_value;
    if (port == 0)
    {
        reg = XL9555_REG_SET_P0;
    }
    else if (port == 1)
    {
        reg = XL9555_REG_SET_P1;
    }
    else
    {
        return; // 无效端口
    }

    // 读取当前寄存器值
    xl9555_read_register(reg, &current_value);

    // 修改指定引脚的值
    if (value)
        current_value |= (1 << pin); // 设置引脚为高
    else
        current_value &= ~(1 << pin); // 设置引脚为低

    // 写回修改后的值
    xl9555_write_register(reg, current_value);
}

/**
 * @brief 翻转指定引脚的输出状态
 * @details 读取当前输出状态，然后翻转指定引脚的电平
 *
 * @param[in] port 端口号 (0: Port0, 1: Port1)
 * @param[in] pin 引脚号 (0-7)
 *
 * @note 仅对配置为输出模式的引脚有效
 * @warning 引脚号超出范围(0-7)会导致未定义行为
 */
void xl9555_toggle_pin(uint8_t port, uint8_t pin)
{
    uint8_t reg = 0;
    uint8_t current_value;
    if (port == 0)
    {
        reg = XL9555_REG_SET_P0;  // 使用输出寄存器而不是反转寄存器
    }
    else if (port == 1)
    {
        reg = XL9555_REG_SET_P1;  // 使用输出寄存器而不是反转寄存器
    }
    else
    {
        return; // 无效端口
    }

    // 读取当前寄存器值
    xl9555_read_register(reg, &current_value);

    // 反转指定引脚的值
    current_value ^= (1 << pin);

    // 写回修改后的值
    xl9555_write_register(reg, current_value);
}

/**
 * @brief 读取指定引脚的输入状态
 * @details 读取XL9555指定端口的指定引脚的电平状态
 *
 * @param[in] port 端口号 (0: Port0, 1: Port1)
 * @param[in] pin 引脚号 (0-7)
 * @param[out] value 读取到的引脚状态 (0: 低电平, 1: 高电平)
 *
 * @note 可读取输入或输出模式引脚的状态
 * @warning 引脚号超出范围(0-7)会导致未定义行为
 * @warning value指针不能为NULL
 */
void xl9555_read_pin(uint8_t port, uint8_t pin, uint8_t *value)
{
    uint8_t reg = 0;
    uint8_t current_value;
    
    if (port == 0)
    {
        reg = XL9555_REG_READ_P0;
    }
    else if (port == 1)
    {
        reg = XL9555_REG_READ_P1;
    }
    else
    {
        return; // 无效端口
    }

    // 读取当前寄存器值
    xl9555_read_register(reg, &current_value);

    // 获取指定引脚的值
    *value = (current_value >> pin) & 0x01;
}