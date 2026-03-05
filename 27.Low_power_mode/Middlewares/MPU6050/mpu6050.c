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
    uint8_t buffer[2] = {reg, data};  /* 创建一个包含寄存器地址和数据的缓冲区 */
    I2C_SendBytes(MPU6050_ADDRESS & 0xFE, buffer, 2);  /* 通过I2C发送数据，传输寄存器地址和数据 */
}

uint8_t MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data;  /* 存储读取到的寄存器数据 */   
    /* 
        在I2C通信中，读取操作与写操作有些不同。
        在读操作中，设备的第一个字节数据并不是目标寄存器地址，而是从设备的当前“地址指针”读取数据。
        因此，如果我们要读取特定寄存器的数据，首先必须通过写操作指定寄存器地址。 
        这意味着，写操作的第一个字节（即`reg`）用于设置设备内部的“地址指针”指向目标寄存器。
        然后，通过执行读操作时，设备会从该地址指针位置读取数据并返回。
    */
    I2C_SendByte(MPU6050_ADDRESS, reg);  /* 通过I2C发送寄存器地址（写操作），指定要读取的寄存器 */  
    /* 
        通过I2C读取数据时，设备已经知道目标寄存器的地址指针，并会返回该寄存器的数据。
        由于之前已通过写操作指定了寄存器地址，现在设备会直接从该地址读取数据。
        这里使用 `MPU6050_ADDRESS | 0x01` 发送读取请求，并接收从该寄存器读取的数据。
    */
    I2C_RecieveByte(MPU6050_ADDRESS | 0x01, &data);  /* 发送读取请求并通过I2C接收数据 */ 
    return data;  /* 返回读取到的数据 */
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
