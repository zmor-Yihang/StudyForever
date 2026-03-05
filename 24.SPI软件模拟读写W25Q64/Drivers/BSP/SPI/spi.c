/* spi.c文件 */
#include "spi.h"

/* SPI引脚控制函数 */
static void cs_low() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); }   /* 片选信号拉低 */
static void cs_high() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); }   /* 片选信号拉高 */
static void scl_low() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); }  /* 时钟信号拉低 */
static void scl_high() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); }   /* 时钟信号拉高 */
static void sda_low() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); }  /* MOSI信号拉低 */
static void sda_high() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET); }   /* MOSI信号拉高 */
static uint8_t sda_read() { return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6); }      /* 读取MISO信号 */

void SPI_Init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* PA4片选，PA5时钟，PA7输出(MOSI)，PA6输入(MISO) */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 设置初始空闲状态 */
    cs_low();
    scl_low();
}

/*
 * SPI通信开始函数
 * 功能：拉低片选信号，开始SPI通信
 */
void SPI_Start()
{
    cs_low();  /* 拉低片选信号，选中从设备 */
}

/*
 * SPI通信结束函数
 * 功能：拉高片选信号，结束SPI通信
 */
void SPI_Stop()
{
    cs_high();  /* 拉高片选信号，释放从设备 */
} 

/*
 * SPI数据交换函数
 * 功能：通过SPI接口发送和接收一个字节的数据
 * 参数：transmitData - 要发送的字节数据
 * 返回值：接收到的字节数据
 * 说明：使用软件模拟SPI时序，MSB先行
 */
uint8_t SPI_Swap(uint8_t transmitData)
{
    uint8_t receiveData = 0;
    
    /* 逐位传输8位数据 */
    for (uint8_t i = 0; i < 8; i++) 
    {
        /* 时钟低电平时准备数据 */
        scl_low();
        /* 根据发送数据的当前位设置MOSI电平 */
        (transmitData & (0x80 >> i)) ? sda_high() : sda_low();
        
        /* 时钟高电平时读取数据 */
        scl_high();
        /* 读取MISO线上的数据并保存 */
        receiveData = (receiveData << 1) | sda_read();
    }
    return receiveData;  /* 返回接收到的数据 */
}