#ifndef __UART_H__
#define __UART_H__

#include "./SYSTEM/sys/sys.h"

extern UART_HandleTypeDef huart1;

void UART_SendByte(uint8_t data);
void UART_SendString(char *str);
uint8_t UART_ReceiveByte(void);

#endif
