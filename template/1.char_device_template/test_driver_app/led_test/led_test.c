#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/led0"
#define BUFFER_SIZE 64

void print_usage(const char *program_name) {
    printf("Usage: %s <on|off|toggle>\n", program_name);
    printf("Example: %s on\n", program_name);
}

int main(int argc, char *argv[]) {
    int fd;
    ssize_t bytes_written;
    char command[BUFFER_SIZE];

    // 检查命令行参数
    if (argc != 2) {
        printf("Error: Invalid number of arguments.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // 验证输入命令
    if (strcmp(argv[1], "on") != 0 && 
        strcmp(argv[1], "off") != 0 && 
        strcmp(argv[1], "toggle") != 0) {
        printf("Error: Invalid command '%s'.\n", argv[1]);
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // 打开设备文件
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Error opening device");
        printf("Make sure the driver is loaded and you have permission to access %s\n", DEVICE_PATH);
        return EXIT_FAILURE;
    }

    // 准备命令
    strncpy(command, argv[1], BUFFER_SIZE - 1);
    command[BUFFER_SIZE - 1] = '\0';  // 确保字符串结束

    // 向设备写入命令
    bytes_written = write(fd, command, strlen(command));
    if (bytes_written < 0) {
        perror("Error writing to device");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Successfully sent command '%s' to LED driver\n", command);

    // 关闭设备文件
    close(fd);

    return EXIT_SUCCESS;
}