#ifndef __DMA_H
#define __DMA_H

#include "./SYSTEM/sys/sys.h"

#define DMA_BUFFER_SIZE 10
extern DMA_HandleTypeDef hdma;

extern uint32_t src[DMA_BUFFER_SIZE];
extern uint32_t dst[DMA_BUFFER_SIZE];

void DMA_Init(void);

#endif


