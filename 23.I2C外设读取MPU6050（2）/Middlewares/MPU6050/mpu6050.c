#include "./MPU6050/mpu6050.h" 

/* 	通过I2C发送数据，这里需要将寄存器地址和要写入的数据拼接在一起，  
	因为I2C协议是基于字节传输的。首先需要发送要写入的从设备的寄存器地址，再发送数据。
	在I2C通信中，设备地址是固定的（`MPU6050_ADDRESS`），而从设备寄存器地址和数据是连续
	发送的。因此，我们需要将寄存器地址和数据打包成一个缓冲区，这样才能按顺序发送。
	
	另外，`MPU6050_ADDRESS & 0xFE` 将设备地址的最低位（即读/写位）设置为0，  
	确保进行写操作。`0xFE` 是一个掩码，其作用是清除地址的最低位，将其设置为0。
	这确保了I2C通信中，传输的是设备的写操作地址（而不是读操作地址）。	*/

/* 指定地址向MPU6050写入寄存器数据 */
void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    I2C_SendByte(MPU6050_ADDRESS & 0xFE, reg, data);
}

uint8_t MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data;
    I2C_RecieveByte(MPU6050_ADDRESS & 0xFE, reg, &data);
    return data;
}


/* MPU6050初始化函数 */
void MPU6050_Init(void)
{
    I2C_Init();  /* 初始化I2C通信 */
    /* MPU6050寄存器初始化，配置部分重要寄存器 */
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);  /* 电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪 */
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);  /* 电源管理寄存器2，保持默认值0，所有轴均不待机 */
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);  /* 采样率分频寄存器，配置采样率 */
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);  	 /* 配置寄存器，配置DLPF（数字低通滤波器） */
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); /* 陀螺仪配置寄存器，选择满量程为±2000°/s */
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);/* 加速度计配置寄存器，选择满量程为±16g */
}

/* 获取加速度计和陀螺仪数据 */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
                     int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    uint8_t DataH, DataL;  /* 定义数据高8位和低8位的变量 */
    
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);  /* 读取加速度计X轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);  /* 读取加速度计X轴的低8位数据 */
    *AccX = (DataH << 8) | DataL;					/* 数据拼接，得到16位加速度计X轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);  /* 读取加速度计Y轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);  /* 读取加速度计Y轴的低8位数据 */
    *AccY = (DataH << 8) | DataL;					/* 数据拼接，得到16位加速度计Y轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);  /* 读取加速度计Z轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);  /* 读取加速度计Z轴的低8位数据 */
    *AccZ = (DataH << 8) | DataL;  					/* 数据拼接，得到16位加速度计Z轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);  /* 读取陀螺仪X轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);  /* 读取陀螺仪X轴的低8位数据 */
    *GyroX = (DataH << 8) | DataL;  			   /* 数据拼接，得到16位陀螺仪X轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);  /* 读取陀螺仪Y轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);  /* 读取陀螺仪Y轴的低8位数据 */
    *GyroY = (DataH << 8) | DataL;  			   /* 数据拼接，得到16位陀螺仪Y轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);  /* 读取陀螺仪Z轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);  /* 读取陀螺仪Z轴的低8位数据 */
    *GyroZ = (DataH << 8) | DataL;				   /* 数据拼接，得到16位陀螺仪Z轴的数据 */
}
