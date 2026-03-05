
/* w25q64.h文件 */
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "./BSP/SPI/spi.h"
#include  "w25q64_Ins.h"

void W25Q64_Init(void);
void W25Q64_Read_ID(uint8_t *MID, uint16_t *DID);
void W25Q64_Write(uint32_t Adr, uint8_t *data, uint16_t len);
void W25Q64_Read(uint32_t Adr, uint8_t *data, uint16_t len);
void W25Q64_Sector_Erase(uint32_t Adr);

#endif
