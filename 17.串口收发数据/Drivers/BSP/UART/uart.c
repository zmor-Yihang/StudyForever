#include "./BSP/UART/uart.h"

/* 定义句柄 */
UART_HandleTypeDef huart1;

void UART_Init(void)
{
    /* 定义结构体 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置GPIO引脚 */
    /* USART1 TX - PA9 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		/* 推挽输出模式 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USART1 RX - PA10 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 	/* 输入模式 */
    GPIO_InitStruct.Pull = GPIO_NOPULL;			/* 无上下拉 */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置UART参数 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;					 /* 设置波特率 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B;	 /* 设置传输数据为长度 */
    huart1.Init.StopBits = UART_STOPBITS_1;		  	 /* 设置停止位长度 */
    huart1.Init.Parity = UART_PARITY_NONE;			 /* 设置奇偶校验 */
    huart1.Init.Mode = UART_MODE_TX_RX;				 /* 设置传输模式 */	
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;	 /* 硬件流控制 */
    huart1.Init.OverSampling = UART_OVERSAMPLING_16; /* 设置过采样 */
    
    /* 初始化UART */
    HAL_UART_Init(&huart1);
}

/* 发送一个字节数据 */
void UART_SendByte(uint8_t data)
{
    /* 阻塞等待直到发送完成，等待最大时间为HAL_MAX_DELAY */
    HAL_UART_Transmit(&huart1, &data, 1, HAL_MAX_DELAY);
}

/* 发送字符串数据 */
void UART_SendString(char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/* 接收一个字节数据 */
uint8_t UART_ReceiveByte(void)
{
    uint8_t data;
    /* 阻塞等待直到接收完成 */
    HAL_UART_Receive(&huart1, &data, 1, HAL_MAX_DELAY);
    return data;
}