#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/TIMER/gtim.h"

/* 添加外部变量声明 */
extern TIM_HandleTypeDef htim3;

int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 
    TIM_PWM_Init();

 
  uint16_t pwmVal = 0; // 调整PWM占空比
  uint8_t swap = 1; // 设置改变方向。1：占空比越来越大；0：占空比越来越小
 
  while (1)
  {
    delay_ms(1);
    
    if(swap)
        pwmVal++;
    else
        pwmVal--;
    
    if(pwmVal > 500)
        swap = 0;
    else if(pwmVal == 0)
        swap = 1;
    
    //修改比较值，修改占空比
      TIM_PWM_SetDuty(pwmVal);
  }
}
