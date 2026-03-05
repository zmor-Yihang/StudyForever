#include "./BSP/LED/led.h"

void LED_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpioInitStruct;
    gpioInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInitStruct.Pin = GPIO_PIN_14;
    gpioInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpioInitStruct);
    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}

void LED_On(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x)
{
    HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x, GPIO_PIN_SET);
}

void LED_Off(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x)
{
    HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x, GPIO_PIN_RESET);
}
void LED_Toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x)
{
    HAL_GPIO_TogglePin(GPIOx, GPIO_PIN_x);
}
