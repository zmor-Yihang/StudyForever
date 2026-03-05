#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    LED_Init();
    UART_Init();
    
    UART_SendString("UART Interrupt Mode Test\r\n");
    UART_SendString("Please input command and press Enter:\r\n");
    
    while(1)
    {
        if(rxComplete)
        {
            /* 处理接收到的指令 */
            if(strcmp((char*)rxBuffer, "LED_ON") == 0)
            {
                LED_On(GPIOB, GPIO_PIN_14);
                UART_SendString("LED turned ON\r\n");
            }
            else if(strcmp((char*)rxBuffer, "LED_OFF") == 0)
            {
                LED_Off(GPIOB, GPIO_PIN_14);
                UART_SendString("LED turned OFF\r\n");
            }
            else
            {
                UART_SendString("Unknown command: ");
                UART_SendString((char*)rxBuffer);
                UART_SendString("\r\n");
            }
            
            rxComplete = 0;  /* 重置接收完成标志 */
        }
        
        /* 其他任务处理 */
        delay_ms(10);
    }
}