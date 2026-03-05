#ifndef __SPI_H__
#define __SPI_H__

#include "./SYSTEM/sys/sys.h"

void SPI_Init();
void SPI_Start();
void SPI_Stop();
void SPI_Transmit(uint8_t transmitData);
uint8_t SPI_Receive();
uint8_t SPI_Swap(uint8_t transmitData);

#endif
