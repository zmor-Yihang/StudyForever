#include "./BSP/TIMER/btim.h"


TIM_HandleTypeDef g_hbtim;
void BTIM_Init()
{
    g_hbtim.Instance = TIM2;
    g_hbtim.Init.Prescaler = 7199;
    g_hbtim.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_hbtim.Init.Period = 4999;
    g_hbtim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&g_hbtim);

    HAL_TIM_Base_Start_IT(&g_hbtim);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();

        HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void TIM2_IRQHandler()
{
    HAL_TIM_IRQHandler(&g_hbtim);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        LED_Toggle(GPIOB, GPIO_PIN_14);
    }
}