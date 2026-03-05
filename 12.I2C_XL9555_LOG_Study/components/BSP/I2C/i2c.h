#ifndef __I2C_H__
#define __I2C_H__

#include "driver/i2c_master.h"

/* I2C总线句柄 */
extern i2c_master_bus_handle_t i2c_bus_handle;

void i2c_bus_init(void);

void i2c_add_device(uint8_t device_address, i2c_master_dev_handle_t *dev_handle);

esp_err_t i2c_detect_device(uint16_t device_address);

esp_err_t i2c_transmit_data(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t data_len);

esp_err_t i2c_receive_data(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t data_len);

esp_err_t i2c_transmit_receive_data(i2c_master_dev_handle_t dev_handle, 
                           uint8_t *tx_data, size_t tx_data_len, 
                           uint8_t *rx_data, size_t rx_data_len);

void i2c_scan_bus(void);

#endif // __I2C_H__
