#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/WDG/iwdg.h"
int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 

    IWDG_Init();
    printf("no\r\n");
    while(1)
    {
        delay_ms(900);
        IWDG_Feed();
        printf("yes\r\n");
    }

}
