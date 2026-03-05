#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./W25Q64/w25q64.h"

uint8_t MID;        /* 制造商ID存储变量 */
uint16_t DID;       /* 设备ID存储变量 */

int main(void)
{   
    HAL_Init(); 
    sys_stm32_clock_init(RCC_PLL_MUL9); 
    delay_init(72); 
    UART_Init(); 

    W25Q64_Init();
    
    /* 读取并显示芯片ID */
    W25Q64_Read_ID(&MID, &DID);
    printf("MID = %x\r\n",MID);  /* 显示制造商ID */
    printf("DID = %x\r\n",DID);  /* 显示设备ID */

    /* 测试数据缓冲区 */
    uint8_t buffer[4] = {0};  /* 读取缓冲区 */
    uint8_t data[4] = {0x10, 0x22, 0x3C, 0x18};  /* 测试数据 */
    
    while(1)
    {   
        /* 向地址0x000010写入4字节测试数据 */
        W25Q64_Write(0x000010, data, 4);
        
        /* 从地址0x000010读取4字节数据 */
        W25Q64_Read(0x000010, buffer, 4);
        
        /* 显示读取到的数据 */
        for(uint8_t i = 0; i < 4; i++)
        {
            printf("buffer[%d] = %x\r\n",i,buffer[i]);
        }
         
        delay_ms(1000);  /* 延时1秒 */
    }
}