/* 主函数 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/UART/uart.h"
#include "./BSP/LED/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    LED_Init();
    UART_IDLE_Init();
    
    UART_SendData((uint8_t*)"UART IDLE Interrupt Test\r\n", 26);
    UART_SendData((uint8_t*)"Send any data...\r\n", 18);
    
    while(1)
    {
        if(rxCompleteFlag)
        {
            /* 在缓冲区末尾添加终止符 */
            rxBuffer[rxSize] = '\0';
            
            /* 处理接收到的数据 */
            UART_SendData((uint8_t*)"Received: ", 10);
            UART_SendData(rxBuffer, rxSize);
            UART_SendData((uint8_t*)"\r\n", 2);
            
            /* 复位标志 */
            rxCompleteFlag = 0;
            
            /* LED指示接收到数据 */
            LED_Toggle(GPIOB, GPIO_PIN_14);
        }
        
        delay_ms(10);
    }
}