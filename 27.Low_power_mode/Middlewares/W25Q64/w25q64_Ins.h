#ifndef __W25Q64_INS_H
#define __W25Q64_INS_H

/* 写操作相关命令 */
#define W25Q64_WRITE_ENABLE							0x06	/* 写使能命令，允许后续执行擦除和编程操作 */
#define W25Q64_WRITE_DISABLE						0x04	/* 写禁止命令，禁止擦除和编程操作 */
#define W25Q64_WRITE_STATUS_REGISTER				0x01	/* 写入状态寄存器 */

/* 编程操作 */
#define W25Q64_PAGE_PROGRAM							0x02	/* 页编程命令，将数据写入指定页 */
#define W25Q64_QUAD_PAGE_PROGRAM					0x32	/* 四线页编程命令，使用4线模式快速写入数据 */

/* 擦除操作 */
#define W25Q64_SECTOR_ERASE_4KB						0x20	/* 4KB扇区擦除 */
#define W25Q64_BLOCK_ERASE_32KB						0x52	/* 32KB块擦除 */
#define W25Q64_BLOCK_ERASE_64KB						0xD8	/* 64KB块擦除 */
#define W25Q64_CHIP_ERASE							0xC7	/* 整片擦除 */

/* 状态寄存器操作 */
#define W25Q64_READ_STATUS_REGISTER_1				0x05	/* 读取状态寄存器1 */
#define W25Q64_READ_STATUS_REGISTER_2				0x35	/* 读取状态寄存器2 */

/* 电源管理 */
#define W25Q64_POWER_DOWN							0xB9	/* 掉电模式 */
#define W25Q64_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB	/* 退出掉电模式并读取设备ID */
#define W25Q64_HIGH_PERFORMANCE_MODE				0xA3	/* 高性能模式 */

/* 读取操作 */
#define W25Q64_READ_DATA							0x03	/* 普通读取数据 */
#define W25Q64_FAST_READ							0x0B	/* 快速读取 */
#define W25Q64_FAST_READ_DUAL_OUTPUT				0x3B	/* 双线输出快速读取 */
#define W25Q64_FAST_READ_DUAL_IO					0xBB	/* 双线I/O快速读取 */
#define W25Q64_FAST_READ_QUAD_OUTPUT				0x6B	/* 四线输出快速读取 */
#define W25Q64_FAST_READ_QUAD_IO					0xEB	/* 四线I/O快速读取 */
#define W25Q64_OCTAL_WORD_READ_QUAD_IO				0xE3	/* 八字四线I/O读取 */

/* 设备信息读取 */
#define W25Q64_MANUFACTURER_DEVICE_ID				0x90	/* 读取制造商和设备ID */
#define W25Q64_JEDEC_ID								0x9F	/* 读取JEDEC ID */
#define W25Q64_READ_UNIQUE_ID						0x4B	/* 读取唯一ID */

/* 其他操作 */
#define W25Q64_ERASE_SUSPEND						0x75	/* 暂停擦除操作 */
#define W25Q64_ERASE_RESUME							0x7A	/* 恢复擦除操作 */
#define W25Q64_CONTINUOUS_READ_MODE_RESET			0xFF	/* 重置连续读取模式 */

/* 虚拟字节 */
#define W25Q64_DUMMY_BYTE							0xFF	/* 虚拟字节，用于占位或延时 */

#endif
