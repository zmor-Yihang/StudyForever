#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"


/* 添加外部变量声明 */
extern TIM_HandleTypeDef htim3;
uint16_t count = 0; 
int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 

    Tim_SlaveMode_Init();  /* PA6为时钟源，LED在PA12 */
    LED_Init();

    while (1)
    {
        LED_Toggle(GPIOA, GPIO_PIN_7);/* 翻转PA7模拟输出脉冲 */
        printf("count = %d\r\n", count);
        delay_ms(500);
    }

    
}
