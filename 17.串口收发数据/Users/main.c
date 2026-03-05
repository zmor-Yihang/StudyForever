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
    UART_Init();
    
    UART_SendString("UART Polling Mode Test\r\n");
    uint8_t receivedData;
    
    while(1)
    {
        /* 接收一个字节数据 */
        receivedData = UART_ReceiveByte();
        
        /* 发送回接收到的数据（回显） */
        UART_SendByte(receivedData);
        
    }
}