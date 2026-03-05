#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./mpu6050/mpu6050.h"

// 读取16位数据的函数

int16_t AX, AY, AZ, GX, GY, GZ;			//定义用于存放各个数据的变量
int main(void)
{   
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    UART_Init();
    MPU6050_Init();

    printf("MPU6050 ID: 0x%x\r\n", MPU6050_ReadReg(MPU6050_WHO_AM_I));
    
    while(1)
    {   
        MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);	//获取MPU6050的数据
        printf("AccX: %d\tAccY: %d\tAccZ: %d\r\n", AX, AY, AZ);
        printf("GyroX: %d\tGyroY: %d\tGyroZ: %d\r\n", GX, GY, GZ);
        printf("\r\n");
        delay_ms(1000); 
    }
}