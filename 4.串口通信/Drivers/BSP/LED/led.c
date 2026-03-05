#include "./BSP/LED/led.h"

void LED_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x)
{
    if (GPIOx == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }else if (GPIOx == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }else if (GPIOx == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }else if (GPIOx == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }else if (GPIOx == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
    GPIO_InitTypeDef gpioInitStruct;
    gpioInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInitStruct.Pin = GPIO_PIN_x;
    gpioInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOx, &gpioInitStruct);
    HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x, GPIO_PIN_RESET);
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
