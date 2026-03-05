#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/adc/adc.h"
#include "./BSP/led/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);
    LED_Init();
    ADC_Init();
    while(1)
    {
        /* LED指示程序运行 */
        LED_Toggle(GPIOB, GPIO_PIN_14);
        delay_ms(5000);
        
        /* 查看ADC转换结果 */
        for(int i = 0; i < 5; i++)
        {
            printf("buffer[%d] = %d\r\n", i, buffer[i]);
            printf("Voltge[%d] = %.2fV\r\n", i, buffer[i] * (3.3 / 4096.0));
        }
    }
}
