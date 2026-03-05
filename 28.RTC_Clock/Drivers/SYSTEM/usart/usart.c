#include "./SYSTEM/usart/usart.h"

/****************************************开始*******************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if (__ARMCC_VERSION >= 6010050)            /* 判断是否使用AC6编译器 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下声明main无参数 */

#else
/* 如果使用AC5编译器，定义__FILE结构体与半主机相关设置 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;  /* 文件句柄 */
    /* 可根据需要增加成员，通常只用printf无需其它操作 */
};

#endif

/* 不使用半主机，重定义_ttywrch函数 */
int _ttywrch(int ch)
{
    ch = ch;  /* 避免编译器警告，空实现 */
    return ch; /* 返回ch */
}

/* 定义_sys_exit函数，防止使用半主机 */
void _sys_exit(int x)
{
    x = x;  /* 避免编译器警告，空实现 */
}

/* 定义_sys_command_string函数，兼容性需要，返回NULL */
char *_sys_command_string(char *cmd, int len)
{
    return NULL; /* 不处理命令，返回NULL */
}

/* 定义标准输出文件结构体 */
FILE __stdout;

/* 重定义fputc函数，printf最终通过fputc输出到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
    USART1->DR = (uint8_t)ch;             /* 将要发送的字符ch写入DR寄存器 */
    return ch;                            /* 返回发送的字符 */
}
/*********************************************结束*************************************/


/* 定义句柄 */
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* 接收缓冲区 */
uint8_t rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxSize = 0;
volatile uint8_t rxCompleteFlag = 0;

void UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    /* 配置GPIO引脚 */
    /* USART1 TX - PA9 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USART1 RX - PA10 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置UART参数 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    /* 初始化UART */
    HAL_UART_Init(&huart1);
    
    /* 配置RX DMA */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_usart1_rx);
    
    /* 关联DMA和UART */
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);
    
    /* 配置NVIC */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    
    /* 启动DMA接收 */
    HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_BUFFER_SIZE);
    
    /* 使能UART空闲中断 */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    /*
        使用 HAL_UART_Receive_IT 或 HAL_UART_Receive_DMA 等
        HAL库函数时，库函数内部会自动使能对应的接收/发送相关中断，
        因此用户无需再手动调用 __HAL_UART_ENABLE_IT 宏来开启这些中断；
        只有像空闲中断（IDLE）等特殊中断，需要用户显式用宏函数单独使能。
	*/
}

/* 发送数据函数 */
void UART_SendData(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&huart1, data, size, HAL_MAX_DELAY);
}

/* UART中断服务函数 */
void USART1_IRQHandler(void)
{
    /* 判断是否是IDLE中断 */
    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
        /* 清除IDLE标志位 */
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);

        /* 设置接收完成标志 */
        rxCompleteFlag = 1;
        
        /* 停止DMA传输 */
        HAL_UART_DMAStop(&huart1);
        
        /* 计算接收到的数据长度 */
        rxSize = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
          
        /* 重新启动DMA接收 */
        HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_BUFFER_SIZE);
    }
    
    /* 其他中断处理 */
    HAL_UART_IRQHandler(&huart1);
}

/* DMA中断服务函数 */
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* 提示发送数据超出缓冲区大小 */
    printf("over size!!!\r\n");
}
