#ifndef __USART_H__
#define __USART_H__

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"

/* 定义句柄 */
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

/* 接收缓冲区 */
#define RX_BUFFER_SIZE 128

extern uint8_t rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxSize;
extern volatile uint8_t rxCompleteFlag;

void UART_Init(void);
void UART_SendData(uint8_t *data, uint16_t size);

#endif
