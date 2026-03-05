#include "./BSP/DMA/dma.h"

DMA_HandleTypeDef hdma;

uint32_t src[DMA_BUFFER_SIZE] = {0};
uint32_t dst[DMA_BUFFER_SIZE] = {0};
void DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma.Instance = DMA1_Channel1;
    hdma.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma.Init.MemInc = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma.Init.Mode = DMA_NORMAL;
    hdma.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma);

    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    HAL_DMA_Start_IT(&hdma, (uint32_t)src, (uint32_t)dst, 10);
}

void DMA1_Channel1_IRQHandler(void)
{ 
    HAL_DMA_IRQHandler(&hdma); 
    printf("over transform!\r\n");
}

