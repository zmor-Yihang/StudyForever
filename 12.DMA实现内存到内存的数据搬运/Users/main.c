#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/DMA/dma.h"
#include "./BSP/LED/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);


    for(int i = 0; i < 10; i++)
    {
        src[i] = i;
    }
    for(int i = 0; i < 10; i++)
    {
        printf("src[%d] = %d\r\n",i,src[i]);
    }
    LED_Init();
    printf("DMA Start!\r\n");
    DMA_Init();
    delay_ms(1000);
    for(int i = 0; i < 10; i++)
    {
        printf("dst[%d] = %d\r\n",i,dst[i]);
    }
    while(1)
    {
        LED_Toggle(GPIOB, GPIO_PIN_14); 
        delay_ms(500);
    }
        
}
