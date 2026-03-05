#ifndef __UART_H__
#define __UART_H__

#include "./SYSTEM/sys/sys.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;

/* 定义缓冲区 */
extern uint8_t rxBuffer[64];   /* 接受缓冲区64字节 */
extern uint8_t rxData;
extern volatile uint8_t rxComplete;

void UART_Init(void);
void UART_SendString(char *str);

#endif
