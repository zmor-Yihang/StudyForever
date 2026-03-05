#include "./BSP/I2C/i2c.h"

static void SCL_High(void)
{
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);
    delay_us(10);
}

static void SCL_Low(void)
{
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET);
    delay_us(10);
}
static void SDA_High(void)
{
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);
    delay_us(10);
}

static void SDA_Low(void)
{
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET);
    delay_us(10);
}

static uint8_t SDA_Read(void)
{
    return HAL_GPIO_ReadPin(SDA_PORT, SDA_PIN);
}

void I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pin = SCL_PIN | SDA_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, SCL_PIN | SDA_PIN, GPIO_PIN_SET);
}

void I2C_Start()
{
    SCL_Low();  /* 兼容指定地址读，此时起始时序时SDA可能为0，*/
    SDA_High(); /* 因此需要再驱动时钟一次，将SDA置位 */
    SCL_High();
    SDA_Low();
}

void I2C_Stop()
{
    SCL_Low();
    SDA_Low();  /* 与start时序类似0 */
    SCL_High();
    SDA_High();
}

void I2C_SendByte(uint8_t byte)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        SCL_Low();
        HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, (GPIO_PinState) (byte & (0x80 >> i)));
        SCL_High();
    }
}

uint8_t I2C_ReceiveByte()
{
    uint8_t byte = 0;
    SCL_Low();
    SDA_High();     /* 主机放开SDA控制权,从机放数据 */
    for(uint8_t i = 0; i < 8; i++)
    {
        SCL_High();
        if(SDA_Read() == GPIO_PIN_SET)
        {
            byte |= (0x80 >> i);
        }
        SCL_Low();   
    } 
    return byte;
}

void I2C_SendAck(uint8_t ack)
{
    SCL_Low();
    if(ack == 1){
        SDA_High();
    }
    else{
        SDA_Low();
    }
    SCL_High();
}

uint8_t I2C_ReceiverAck(void)
{ 
    SCL_Low();
    SDA_High();  /* 主机放手 */
    SCL_High();
    return SDA_Read();  /* 主机读从机应答 */
}