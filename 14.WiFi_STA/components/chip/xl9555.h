#ifndef __XL9555_H__
#define __XL9555_H__

#include "i2c.h"
#include "esp_log.h"

/**
 * @brief XL9555 设备地址
 * @details 用于I2C通信的设备地址, 7位地址, 0x20
 *          扩展GPIO分两组, 每组8个引脚, 分别对应P0.0-P0.7和P1.0-P1.7
 */
#define XL9555_ADDRESS 0x20        /* XL9555 设备地址 */

#define XL9555_REG_INPUT_P0 0x00   /* 读取P0口电平状态, 8个位表示P0.0-P0.7 */
#define XL9555_REG_INPUT_P1 0x01   /* 读取P1口电平状态, 8个位表示P1.0-P1.7 */

#define XL9555_REG_OUTPUT_P0 0x02   /* 设置P0口电平状态, 8个位表示P0.0-P0.7 */
#define XL9555_REG_OUTPUT_P1 0x03   /* 设置P1口电平状态, 8个位表示P1.0-P1.7 */

#define XL9555_REG_INV_P0 0x04   /* 反转P0口电平状态, 8个位表示P0.0-P0.7 */
#define XL9555_REG_INV_P1 0x05   /* 反转P1口电平状态, 8个位表示P1.0-P1.7 */

#define XL9555_REG_CONFIG_P0 0x06   /* 配置P0端口方向寄存器, 1表示输入, 0表示输出 */
#define XL9555_REG_CONFIG_P1 0x07   /* 配置P1端口方向寄存器, 1表示输入, 0表示输出 */

extern i2c_master_dev_handle_t xl9555_handle;

void xl9555_init(void);
void xl9555_set_pin_mode(uint8_t port, uint8_t pin, uint8_t mode);
void xl9555_set_pin(uint8_t port, uint8_t pin, uint8_t value);
void xl9555_toggle_pin(uint8_t port, uint8_t pin);
void xl9555_read_pin(uint8_t port, uint8_t pin, uint8_t *value);

#endif // __XL9555_H__
