#ifndef __IOCTL_CMDS_H__
#define __IOCTL_CMDS_H__

#include <linux/ioctl.h>

/* ioctl命令魔数 */
#define IOCTL_MAGIC 'Z'

/* ioctl命令定义 */
#define IOCTL_RESET_BUFFER   _IO(IOCTL_MAGIC, 0)    // 重置缓冲区
#define IOCTL_GET_BUFFER_SIZE _IOR(IOCTL_MAGIC, 1, int)  // 获取缓冲区大小
#define IOCTL_CLEAR_BUFFER   _IO(IOCTL_MAGIC, 2)    // 清空缓冲区
#define IOCTL_SET_POSITION   _IOW(IOCTL_MAGIC, 3, int)  // 设置文件位置
#define IOCTL_GET_POSITION   _IOR(IOCTL_MAGIC, 4, int)  // 获取当前文件位置
#define IOCTL_FILL_BUFFER    _IOW(IOCTL_MAGIC, 5, char*) // 用指定字符填充缓冲区

#endif /* __IOCTL_CMDS_H__ */