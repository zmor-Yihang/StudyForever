
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
    UART_DMA_Init();
    
   /* 要发送的数据 */
    const char *msg = "Hello DMA UART!\r\n";
    uint16_t msgLen = strlen(msg);

    uint8_t data[16] = {0};
    for(int i = 0; i < 16; i++)
    {
        data[i] = i * 2;
    } 

    /* 启动一次DMA接收，接收16个字节 */
    UART_DMA_Receive(16);

    while(1)
    {
        /* 发送数据 */
        UART_DMA_Transmit((uint8_t *)msg, msgLen);

        /* 模拟主循环做点事情，延时2秒 */
        HAL_Delay(2000);

        /* 判断是否有接收完成 */
        if(rxCompleteFlag)
        {
            rxCompleteFlag = 0;
            printf("Receive Data:\r\n");
            for(int i = 0; i < 16; i++)
            {
                printf("%d;", rxBuffer[i]);
            }
            LED_Toggle(GPIOB, GPIO_PIN_14);
            UART_DMA_Receive(16);
        }
    }
}