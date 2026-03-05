#ifndef __SPI_H__
#define __SPI_H__

#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

extern spi_device_handle_t spi2_handle;

void spi2_init(void);
uint8_t spi2_transfer_byte(uint8_t data);
void spi2_write_data(uint8_t *data, int len);

#endif  /* __SPI_H__ */