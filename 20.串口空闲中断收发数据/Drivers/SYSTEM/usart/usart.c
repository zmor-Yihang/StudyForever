// #include "./SYSTEM/usart/usart.h"

// /****************************************开始*******************************************/
// /* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

// #if 1

// #if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
// __asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
// __asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

// #else
// /* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
// #pragma import(__use_no_semihosting)

// struct __FILE
// {
//     int handle;
//     /* Whatever you require here. If the only file you are using is */
//     /* standard output using printf() for debugging, no file handling */
//     /* is required. */
// };

// #endif

// /* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
// int _ttywrch(int ch)
// {
//     ch = ch;
//     return ch;
// }

// /* 定义_sys_exit()以避免使用半主机模式 */
// void _sys_exit(int x)
// {
//     x = x;
// }

// char *_sys_command_string(char *cmd, int len)
// {
//     return NULL;
// }

// /* FILE 在 stdio.h里面定义. */
// FILE __stdout;

// /* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
// int fputc(int ch, FILE *f)
// {
//     while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */

//     USART1->DR = (uint8_t)ch;             /* 将要发送的字符 ch 写入到DR寄存器 */
//     return ch;
// }
// #endif
// /*********************************************结束*************************************/


// /* 定义句柄 */
// UART_HandleTypeDef huart1;

// /* 定义缓冲区 */
// uint8_t rxBuffer[64];   /* 接受缓冲区64字节 */
// uint8_t rxData;
// volatile uint8_t rxComplete = 0;
// void UART_Init(void)
// {
//     /* 定义结构体 */
//     GPIO_InitTypeDef GPIO_InitStruct = {0};
    
//     /* 使能时钟 */
//     __HAL_RCC_USART1_CLK_ENABLE();
//     __HAL_RCC_GPIOA_CLK_ENABLE();
    
//     /* 配置GPIO引脚 */
//     /* USART1 TX - PA9 */
//     GPIO_InitStruct.Pin = GPIO_PIN_9;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		/* 推挽输出模式 */
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
//     /* USART1 RX - PA10 */
//     GPIO_InitStruct.Pin = GPIO_PIN_10;
//     GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 	/* 输入模式 */
//     GPIO_InitStruct.Pull = GPIO_NOPULL;			/* 无上下拉 */
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
//     /* 配置UART参数 */
//     huart1.Instance = USART1;
//     huart1.Init.BaudRate = 115200;					 /* 设置波特率 */
//     huart1.Init.WordLength = UART_WORDLENGTH_8B;	 /* 设置传输数据为长度 */
//     huart1.Init.StopBits = UART_STOPBITS_1;		  	 /* 设置停止位长度 */
//     huart1.Init.Parity = UART_PARITY_NONE;			 /* 设置奇偶校验 */
//     huart1.Init.Mode = UART_MODE_TX_RX;				 /* 设置传输模式 */	
//     huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;	 /* 硬件流控制 */
//     huart1.Init.OverSampling = UART_OVERSAMPLING_16; /* 设置过采样 */
    
//     /* 初始化UART */
//     HAL_UART_Init(&huart1);

//     /*开启中断*/
//     HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
//     HAL_NVIC_EnableIRQ(USART1_IRQn);

//     /* 启动UART */
//     HAL_UART_Receive_IT(&huart1, &rxData, 1);
// }

// /* 发送字符串数据 */
// void UART_SendString(char *str)
// {
//     HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
// }

// /* UART中断服务函数 */
// void USART1_IRQHandler(void)
// {
//     HAL_UART_IRQHandler(&huart1);
// }

// /* 接收完成回调函数，以 \r\n 作为一条数据的结束标志 */
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     /* 判断是否为 USART1 产生的中断 */
//     if (huart->Instance == USART1)
//     {
//         static uint8_t rxIndex = 0;   /* 静态变量，记录接收数据的下标 */
//         static uint8_t state = 0;     /* 状态变量：0=普通状态，1=刚收到'\r' */

//         switch(state)
//         {
//             case 0:
//                 /* 普通状态，尚未遇到回车符 '\r' */
//                 if (rxData == '\r')
//                 {
//                     state = 1;  /* 如果收到 '\r'，状态切换为1，等待下一个字符判断是否为 '\n' */
//                 }
//                 else
//                 {
//                     /* 普通字符，存入缓冲区 */
//                     if (rxIndex < sizeof(rxBuffer) - 1)
//                         rxBuffer[rxIndex++] = rxData;
//                 }
//                 break;

//             case 1:
//                 /* 状态1：上一次收到的是 '\r'，现在判断是否为 '\n' */
//                 if (rxData == '\n')
//                 {
//                     /* 收到 '\n'，说明遇到了完整的 "\r\n"，数据接收完成 */
//                     rxBuffer[rxIndex] = '\0';   /* 添加字符串结束符 */
//                     rxComplete = 1;             /* 设置接收完成标志 */
//                     rxIndex = 0;                /* 重置下标，准备接收下一条数据 */
//                 }
//                 else
//                 {
//                     /* 如果不是 '\n'，则之前收到的 '\r' 视为普通数据，和当前字符一起存入缓冲区 */
//                     if (rxIndex < sizeof(rxBuffer) - 2)
//                     {
//                         rxBuffer[rxIndex++] = '\r';    /* 存入上一个 '\r' */
//                         rxBuffer[rxIndex++] = rxData;  /* 存入当前收到的字符 */
//                     }
//                 }
//                 state = 0;   /* 状态回到普通状态，准备下一轮判断 */
//                 break;
//         }

//         /* 启动下一字节的中断接收，保证持续接收数据 */
//         HAL_UART_Receive_IT(&huart1, &rxData, 1);
//     }
// }