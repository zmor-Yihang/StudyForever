#ifndef __I2C_H__
#define __I2C_H__

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"

#define SCL_PORT GPIOB
#define SCL_PIN  GPIO_PIN_10
#define SDA_PORT GPIOB
#define SDA_PIN  GPIO_PIN_11

void I2C_Init(void);

void I2C_Start();
void I2C_Stop();
void I2C_SendByte(uint8_t byte);
uint8_t I2C_ReceiveByte();
void I2C_SendAck(uint8_t ack);
uint8_t I2C_ReceiverAck(void);

#endif
