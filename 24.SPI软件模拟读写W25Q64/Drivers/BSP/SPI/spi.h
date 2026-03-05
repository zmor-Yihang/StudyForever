
/* spi.h文件 */
#ifndef __SPI_H__
#define __SPI_H__

#include "./SYSTEM/sys/sys.h"

void SPI_Init();
void SPI_Start();
void SPI_Stop();
uint8_t SPI_Swap(uint8_t transmitData);

#endif