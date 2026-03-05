#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "./MPU6050/MPU6050_Reg.h"
#include "./BSP/I2C/i2c.h"
#include "./SYSTEM/usart/usart.h"

void MPU6050_Init(void);
void MPU6050_WriteReg(uint8_t reg, uint8_t data);
uint8_t MPU6050_ReadReg(uint8_t reg);
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);

#endif
