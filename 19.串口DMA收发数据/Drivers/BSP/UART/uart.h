#ifndef __UART_H__
#define __UART_H__

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"

/* 定义句柄 */
extern UART_HandleTypeDef huart1;


/* 定义句柄 */
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

/* 定义缓冲区 */
extern uint8_t txBuffer[256];
extern uint8_t rxBuffer[256];
extern volatile uint8_t rxCompleteFlag;

void UART_DMA_Init(void);

void UART_DMA_Transmit(uint8_t *data, uint16_t size);
void UART_DMA_Receive(uint16_t size);

#endif
