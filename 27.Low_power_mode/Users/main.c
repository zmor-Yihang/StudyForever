#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"

int main(void)
{   
    HAL_Init(); 
    sys_stm32_clock_init(RCC_PLL_MUL9); 
    delay_init(72); 
    UART_Init();
    LED_Init(); 
    PWR_Init();  /* 初始化PWR */
    printf("System is running...\r\n");
            
    while(1)
    {        
        LED_On(GPIOB, GPIO_PIN_14);
        delay_ms(1000); 

        printf("Entering Standby Mode...\r\n");
        HAL_PWR_EnterSTANDBYMode();

        printf("EXIT Standby Mode...\r\n");  /* 这句执行不到 */
    }
}