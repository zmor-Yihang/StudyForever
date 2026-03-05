#include "./BSP/TIMER/gtim.h"

TIM_HandleTypeDef htim3;
extern uint32_t count;
void gtim_init(void)
{
    
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_IC_InitTypeDef TIM_IC_InitStruct;

    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    htim3.Instance = TIM3;
    htim3.Init.Period = 1000;
    htim3.Init.Prescaler = 72-1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&htim3);

    TIM_IC_InitStruct.ICFilter = 0x0;
    TIM_IC_InitStruct.ICPolarity = TIM_ICPOLARITY_RISING;
    TIM_IC_InitStruct.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_IC_InitStruct.ICSelection = TIM_ICSELECTION_DIRECTTI;
    HAL_TIM_IC_ConfigChannel(&htim3, &TIM_IC_InitStruct, TIM_CHANNEL_1);

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        // count++;
        // printf("count: %d\r\n",count);
    }
}


