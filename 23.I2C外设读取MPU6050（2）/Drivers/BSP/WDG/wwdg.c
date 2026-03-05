#include "./BSP/WDG/wwdg.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

WWDG_HandleTypeDef hwwdg;
void WWDG_Init(void)
{
    __HAL_RCC_WWDG_CLK_ENABLE();
    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
    hwwdg.Init.Window = 0x5F;
    hwwdg.Init.Counter = 0x7F;
    hwwdg.Init.EWIMode = WWDG_EWI_ENABLE; 
    HAL_WWDG_Init(&hwwdg);

    HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&hwwdg);
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    printf("No feed dog!/r/n");
}

void WWDG_Feed()
{
    HAL_WWDG_Refresh(&hwwdg);
}
