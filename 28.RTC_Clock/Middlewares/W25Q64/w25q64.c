#include "w25q64.h"

/*
 * W25Q64写使能函数
 * 功能：发送写使能指令，允许后续写操作
 */
static void W25Q64_Write_Enable(void)
{
    SPI_Start();  /* 开始SPI通信 */
    SPI_Transmit(W25Q64_WRITE_ENABLE);
    SPI_Stop();  /* 结束SPI通信 */
}

/*
 * W25Q64等待忙状态结束
 * 功能：轮询状态寄存器，等待芯片操作完成
 */
static void W25Q64_Wait_Busy(void)
{
    SPI_Start();  /* 开始SPI通信 */
    SPI_Transmit(W25Q64_READ_STATUS_REGISTER_1);  /* 发送读状态寄存器指令 */
    /* 检查BUSY位(bit0)，为1表示忙，等待直到为0 */
    while ((SPI_Receive() & 0x01) == 0x01);
    SPI_Stop();  /* 结束SPI通信 */
}

/*
 * W25Q64初始化函数
 * 功能：初始化SPI接口
 */
void W25Q64_Init(void)
{
    SPI_Init();
}

/*
 * W25Q64读取ID函数
 * 功能：读取制造商ID和设备ID
 * 参数：MID - 制造商ID指针，DID - 设备ID指针
 */
void W25Q64_Read_ID(uint8_t *MID, uint16_t *DID)
{
    SPI_Start();                        /* 开始SPI通信 */
    SPI_Transmit(W25Q64_JEDEC_ID);      /* 发送读JEDEC ID指令0x9F */
    *MID = SPI_Receive();               /* 读取制造商ID */
    uint8_t DIDH = SPI_Receive();       /* 读取设备ID高字节 */
    uint8_t DIDL = SPI_Receive();       /* 读取设备ID低字节 */
    *DID = (DIDH << 8) | DIDL;          /* 组合成16位设备ID */
    SPI_Stop();                         /* 结束SPI通信 */
}

/*
 * W25Q64写数据函数
 * 功能：向指定地址写入数据
 * 参数：Adr - 起始地址，data - 数据缓冲区指针，len - 数据长度
 */
void W25Q64_Write(uint32_t Adr, uint8_t *data, uint16_t len)
{
    W25Q64_Sector_Erase(Adr);  /* 擦除目标扇区 */
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */
    W25Q64_Write_Enable();  /* 发送写使能 */

    SPI_Start();  /* 开始SPI通信 */
    SPI_Transmit(W25Q64_PAGE_PROGRAM);  /* 发送页编程指令0x02 */
    SPI_Transmit(Adr >> 16);  /* 发送地址高字节 */
    SPI_Transmit(Adr >> 8);   /* 发送地址中字节 */
    SPI_Transmit(Adr);        /* 发送地址低字节 */
    
    /* 写入数据 */
    for(uint16_t i = 0; i < len; i++)
    {
        SPI_Transmit(*(data + i));
    }
    SPI_Stop();  /* 结束SPI通信 */
}

/*
 * W25Q64读数据函数
 * 功能：从指定地址读取数据
 * 参数：Adr - 起始地址，buffer - 数据缓冲区指针，len - 数据长度
 */
void W25Q64_Read(uint32_t Adr, uint8_t *buffer, uint16_t len)
{
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */

    SPI_Start();                /* 开始SPI通信 */
    SPI_Transmit(W25Q64_READ_DATA);  /* 发送读数据指令0x03 */
    SPI_Transmit(Adr >> 16);  /* 发送地址高字节 */
    SPI_Transmit(Adr >> 8);   /* 发送地址中字节 */
    SPI_Transmit(Adr);        /* 发送地址低字节 */
    
    /* 读取数据 */
    for(uint16_t i = 0; i < len; i++)
    {
        *(buffer + i) = SPI_Receive();
    }
    SPI_Stop();  /* 结束SPI通信 */
}

/*
 * W25Q64扇区擦除函数
 * 功能：擦除指定地址所在的4KB扇区
 * 参数：Adr - 扇区内任意地址
 */
void W25Q64_Sector_Erase(uint32_t Adr)
{
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */

    SPI_Start();  /* 开始SPI通信 */
    W25Q64_Write_Enable();  /* 发送写使能 */  
    SPI_Transmit(W25Q64_SECTOR_ERASE_4KB);  /* 发送扇区擦除指令0x20 */
    SPI_Transmit(Adr >> 16);  /* 发送地址高字节 */
    SPI_Transmit(Adr >> 8);   /* 发送地址中字节 */
    SPI_Transmit(Adr);        /* 发送地址低字节 */
    SPI_Stop();  /* 结束SPI通信 */
}