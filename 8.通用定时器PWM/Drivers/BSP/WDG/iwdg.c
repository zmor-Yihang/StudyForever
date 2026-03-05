#include "./BSP/WDG/iwdg.h"



IWDG_HandleTypeDef g_hiwdg;
void IWDG_Init(void)
{
    g_hiwdg.Instance = IWDG;
    g_hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    g_hiwdg.Init.Reload = 1249;   
    HAL_IWDG_Init(&g_hiwdg);
}

void IWDG_Feed(void)
{
    HAL_IWDG_Refresh(&g_hiwdg);
}
