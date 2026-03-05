#include "spi.h"

spi_device_handle_t spi2_handle;

void spi2_init(void)
{
    // 初始化SPI总线
    spi_bus_config_t spi_bus_config = {0};
    spi_bus_config.sclk_io_num = GPIO_NUM_12;
    spi_bus_config.miso_io_num = GPIO_NUM_13;
    spi_bus_config.mosi_io_num = GPIO_NUM_11;
    spi_bus_config.quadwp_io_num = -1;
    spi_bus_config.quadhd_io_num = -1;
    spi_bus_config.data4_io_num = -1;
    spi_bus_config.data5_io_num = -1;
    spi_bus_config.data6_io_num = -1;
    spi_bus_config.data7_io_num = -1;
    spi_bus_config.data_io_default_level = false;
    spi_bus_config.max_transfer_sz = 320 * 480;
    spi_bus_config.flags = SPICOMMON_BUSFLAG_MASTER;
    spi_bus_config.intr_flags = 0;
    spi_bus_config.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;

    spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_CH_AUTO);
}

uint8_t spi2_transfer_byte(uint8_t data)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));

    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    spi_device_polling_transmit(spi2_handle, &t);

    return t.rx_data[0];
}

void spi2_write_data(uint8_t *data, int len)
{
    spi_transaction_t t = {0};

    t.length = len * 8;                            
    t.tx_buffer = data;                            
    spi_device_polling_transmit(spi2_handle, &t);  
}