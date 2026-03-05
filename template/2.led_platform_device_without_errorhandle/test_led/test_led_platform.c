#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../lib/libled0/libled0.h"

void show_menu() {
    printf("\n=== LED Platform Device Manual Test Program ===\n");
    printf("1. Turn LED ON\n");
    printf("2. Turn LED OFF\n");
    printf("3. Toggle LED\n");
    printf("4. Set LED state (0=OFF, 1=ON)\n");
    printf("5. Get LED state\n");
    printf("6. Exit\n");
    printf("Please enter your choice (1-6): ");
}

int main()
{
    int fd;
    int led_state;
    int ret;
    int choice;
    int set_value;

    printf("LED Platform Device Manual Test Program\n");
    printf("=======================================\n");

    // 打开设备文件
    fd = open("/dev/led0", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    printf("Device opened successfully\n");

    // 主循环，等待用户输入
    while (1) {
        show_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            // 清除输入缓冲区
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:
                printf("Turning LED ON...\n");
                ret = led0_on(fd);
                if (ret < 0) {
                    perror("led0_on failed");
                } else {
                    printf("LED turned ON successfully\n");
                }
                break;
                
            case 2:
                printf("Turning LED OFF...\n");
                ret = led0_off(fd);
                if (ret < 0) {
                    perror("led0_off failed");
                } else {
                    printf("LED turned OFF successfully\n");
                }
                break;
                
            case 3:
                printf("Toggling LED...\n");
                ret = led0_toggle(fd);
                if (ret < 0) {
                    perror("led0_toggle failed");
                } else {
                    printf("LED toggled successfully\n");
                }
                break;
                
            case 4:
                printf("Enter LED state (0=OFF, 1=ON): ");
                if (scanf("%d", &set_value) == 1) {
                    ret = led0_set(fd, set_value);
                    if (ret < 0) {
                        perror("led0_set failed");
                    } else {
                        printf("LED set successfully\n");
                    }
                } else {
                    printf("Invalid input for LED state!\n");
                    // 清除输入缓冲区
                    while (getchar() != '\n');
                }
                break;
                
            case 5:
                printf("Getting LED state...\n");
                ret = led0_get(fd, &led_state);
                if (ret < 0) {
                    perror("led0_get failed");
                } else {
                    printf("Current LED state: %d\n", led_state);
                }
                break;
                
            case 6:
                printf("Exiting program...\n");
                goto cleanup;
                
            default:
                printf("Invalid choice! Please enter a number between 1-6.\n");
                break;
        }
    }

cleanup:
    // 关闭设备文件
    close(fd);
    printf("Device closed successfully\n");
    printf("Program exited.\n");
    return 0;
}