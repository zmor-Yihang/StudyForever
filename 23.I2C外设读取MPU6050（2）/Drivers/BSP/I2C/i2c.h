#ifndef __I2C_H__
#define __I2C_H__

#include "./SYSTEM/sys/sys.h"
#include "./MPU6050/MPU6050_Reg.h"

extern I2C_HandleTypeDef hi2c2;

void I2C_Init(void);
void I2C_SendByte(uint16_t adr, uint16_t memAdr, uint8_t data);
void I2C_SendBytes(uint16_t adr, uint8_t *sendBuffer, uint8_t len);
void I2C_RecieveByte(uint16_t adr, uint16_t memAdr, uint8_t *data);

#endif
