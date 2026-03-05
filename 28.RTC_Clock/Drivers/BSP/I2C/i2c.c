#include "./BSP/I2C/i2c.h"

I2C_HandleTypeDef hi2c2;
void I2C_Init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_I2C2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    hi2c2.Instance = I2C2;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;  /* 7位地址模式 */
    hi2c2.Init.ClockSpeed = 100000;                       /* 通信速率，最大400k */
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; /* 单地址模式 */
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;               /* SCL高低电平持续时间之比  */
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; /* 禁用广播模式 */
    /* I2C主设备发送通用呼叫地址（0x00）时，I2C 总线上的所有从设备都会响应这个地址，
        这个模式通常用于对总线上的所有从设备进行广播消息或重置操作。*/
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;     /* 允许时钟拉伸 */
    /* 允许I2C从设备在数据传输过程中暂停（“拉伸”）时钟信号。当从设备忙于处理
       数据或等待其他操作时，它可以通过拉低时钟线来延迟主设备的时钟信号，以便有更
       多的时间来处理请求。主设备会在时钟信号被恢复后继续通信。 */
    hi2c2.Init.OwnAddress1 = 0;     /* 自身地址1 */
    hi2c2.Init.OwnAddress2 = 0;     /* 自身地址2 */
    HAL_I2C_Init(&hi2c2);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* 向从设备发送一个字节数据 */
void I2C_SendByte(uint16_t adr, uint8_t data)
{
    HAL_I2C_Master_Transmit(&hi2c2, adr, &data, 1, 1000);
}

/* 向从设备发送多个字节数据 */
void I2C_SendBytes(uint16_t adr, uint8_t *sendBuffre, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c2, adr, sendBuffre, len, 1000);
}

/* 从设备接收一个字节数据 */
void I2C_RecieveByte(uint16_t adr, uint8_t *recieveBuffer)
{
    HAL_I2C_Master_Receive(&hi2c2, adr, recieveBuffer, 1, 1000);
}