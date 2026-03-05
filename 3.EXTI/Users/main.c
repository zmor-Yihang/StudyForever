#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/LED.h"
#include "./BSP/EXTI/EXTI.h"
int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 
    EXTI_Init();
    LED_Init();
    while(1)
    {
        printf("hello world\r\n");
        delay_ms(1000);
    }

}
