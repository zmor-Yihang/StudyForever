
/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./W25Q64/w25q64.h"

uint8_t MID;    /* 制造商ID存储变量 */
uint16_t DID;    /* 设备ID存储变量 */

/*
 * 主函数
 * 功能：测试W25Q64闪存芯片的读写功能
 * 1. 初始化系统时钟和外设
 * 2. 读取并显示W25Q64的ID信息
 * 3. 循环写入测试数据并读取验证
 */
int main(void)
{   
    /* 系统初始化 */
    HAL_Init();  /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);  /* 设置系统时钟为72MHz */
    delay_init(72);  /* 初始化延时函数 */
    UART_Init();  /* 初始化串口 */
    
    /* W25Q64初始化 */
    W25Q64_Init();
    
    /* 读取并显示芯片ID */
    W25Q64_Read_ID(&MID, &DID);
    printf("MID = %x\r\n",MID);  /* 显示制造商ID */
    printf("DID = %x\r\n",DID);  /* 显示设备ID */

    /* 测试数据缓冲区 */
    uint8_t buffer[4] = {0};  /* 读取缓冲区 */
    uint8_t data[4] = {0x10, 0x22, 0x3C, 0x99};  /* 测试数据 */
    
    /* 主循环：持续测试读写功能 */
    while(1)
    {   
        /* 向地址0x000000写入4字节测试数据 */
        W25Q64_Write(0x000000, data, 4);
        
        /* 从地址0x000000读取4字节数据 */
        W25Q64_Read(0x000000, buffer, 4);
        
        /* 显示读取到的数据 */
        for(uint8_t i = 0; i < 4; i++)
        {
            printf("buffer[%d] = %x\r\n", i, buffer[i]);
        }
        
        delay_ms(1000);  /* 延时1秒 */
    }
}