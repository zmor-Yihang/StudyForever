#include "xl9555.h"

static const char *TAG = "XL9555";

/* XL9555设备的I2C句柄 */
i2c_master_dev_handle_t xl9555_handle = NULL;

/**
 * @brief 写XL9555寄存器
 * @details 向XL9555的指定寄存器写入数据
 * @param[in] reg 寄存器地址
 * @param[in] value 要写入的值
 */
static void xl9555_write_register(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    i2c_transmit_data(xl9555_handle, data, sizeof(data));
}

/**
 * @brief 读XL9555寄存器
 * @details 从XL9555的指定寄存器读取数据
 * @param[in] reg 寄存器地址
 * @param[out] value 读取到的值的存储指针
 */
static void xl9555_read_register(uint8_t reg, uint8_t *value)
{
    i2c_transmit_receive_data(xl9555_handle, &reg, 1, value, 1);
}

/**
 * @brief 初始化XL9555设备
 * @details 初始化I2C总线并添加XL9555设备到总线上
 * @note 调用此函数前确保已正确配置I2C引脚
 */
void xl9555_init(void)
{
    i2c_bus_init();
    i2c_add_device(XL9555_ADDRESS, &xl9555_handle);
}

/**
 * @brief 设置GPIO引脚模式
 * @details 配置XL9555的指定引脚为输入或输出模式
 * @param[in] port GPIO端口号 (0: Port0, 1: Port1)
 * @param[in] pin 引脚号 (0-7)
 * @param[in] mode 引脚模式 (0: 输出模式, 1: 输入模式)
 * @note 仅支持端口0和端口1，其他值会导致未知行为
 */
void xl9555_set_pin_mode(uint8_t port, uint8_t pin, uint8_t mode)
{
    uint8_t reg = 0;
    uint8_t current_config;
    
    if (pin > 7)
    {
        ESP_LOGE(TAG, "Invalid pin number %d", pin); // 无效引脚
        return;
    }
    
    if (port == 0)
    {
        reg = XL9555_REG_CONFIG_P0;
    }
    else if (port == 1)
    {
        reg = XL9555_REG_CONFIG_P1;
    }
    else
    {
        ESP_LOGE(TAG, "Invalid port number %d", port); // 无效端口
        return;
    }
    
    // 读取当前配置寄存器值
    xl9555_read_register(reg, &current_config);
    
    // 修改指定引脚的配置
    if (mode)
    {
        current_config |= (1 << pin); // 设置为输入模式 (1)
    }
    else
    {
        current_config &= ~(1 << pin); // 设置为输出模式 (0)
    }
    
    // 写回修改后的配置
    xl9555_write_register(reg, current_config);
}

/**
 * @brief 设置指定引脚的输出状态
 * @details 设置XL9555指定端口的指定引脚为高电平或低电平
 *
 * @param[in] port 端口号 (0: Port0, 1: Port1)
 * @param[in] pin 引脚号 (0-7)
 * @param[in] value 输出值 (0: 低电平, 1: 高电平)
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
        reg = XL9555_REG_OUTPUT_P0;
    }
    else if (port == 1)
    {
        reg = XL9555_REG_OUTPUT_P1;
    }
    else
    {
        ESP_LOGE(TAG, "Invalid port number %d", port); // 无效端口
        return;
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
        reg = XL9555_REG_OUTPUT_P0; // 使用输出寄存器而不是反转寄存器
    }
    else if (port == 1)
    {
        reg = XL9555_REG_OUTPUT_P1; // 使用输出寄存器而不是反转寄存器
    }
    else
    {
        ESP_LOGE(TAG, "Invalid port number %d", port); // 无效端口
        return;
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
        reg = XL9555_REG_INPUT_P0;
    }
    else if (port == 1)
    {
        reg = XL9555_REG_INPUT_P1;
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