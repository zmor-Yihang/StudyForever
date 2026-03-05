#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/adc/adc.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);
    
    ADC_Init();
    while(1)
    {
        uint16_t value = ADC_GetValue();
        float voltage = (float)value *(3.3 / 4096) ;
        printf("ADC_Value: %d\r\n", value);
        printf("Votelge_Value: %.2f\r\n", voltage);
        delay_ms(1000);  
    }

        
}
