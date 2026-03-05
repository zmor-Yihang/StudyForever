#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../lib/libled0/libled0.h"

int main()
{
    int fd;
    int led_state;
    int ret;

    printf("LED Platform Device Test Program\n");
    printf("===============================\n");

    // 打开设备文件
    fd = open("/dev/led0", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    printf("Device opened successfully\n");

    // 测试LED开启功能
    printf("\n1. Testing LED ON...\n");
    ret = led0_on(fd);
    if (ret < 0) {
        perror("led0_on failed");
    } else {
        printf("LED turned ON successfully\n");
    }

    sleep(1);

    // 测试LED关闭功能
    printf("\n2. Testing LED OFF...\n");
    ret = led0_off(fd);
    if (ret < 0) {
        perror("led0_off failed");
    } else {
        printf("LED turned OFF successfully\n");
    }

    sleep(1);

    // 测试LED切换功能
    printf("\n3. Testing LED TOGGLE (ON)...\n");
    ret = led0_toggle(fd);
    if (ret < 0) {
        perror("led0_toggle failed");
    } else {
        printf("LED toggled successfully\n");
    }

    sleep(1);

    printf("\n4. Testing LED TOGGLE (OFF)...\n");
    ret = led0_toggle(fd);
    if (ret < 0) {
        perror("led0_toggle failed");
    } else {
        printf("LED toggled successfully\n");
    }

    sleep(1);

    // 测试LED设置功能
    printf("\n5. Testing LED SET (ON)...\n");
    ret = led0_set(fd, 1);
    if (ret < 0) {
        perror("led0_set failed");
    } else {
        printf("LED set to ON successfully\n");
    }

    sleep(1);

    printf("\n6. Testing LED SET (OFF)...\n");
    ret = led0_set(fd, 0);
    if (ret < 0) {
        perror("led0_set failed");
    } else {
        printf("LED set to OFF successfully\n");
    }

    sleep(1);

    // 测试LED获取状态功能
    printf("\n7. Testing LED GET...\n");
    ret = led0_get(fd, &led_state);
    if (ret < 0) {
        perror("led0_get failed");
    } else {
        printf("Current LED state: %d\n", led_state);
    }

    // 关闭设备文件
    close(fd);
    printf("\nDevice closed successfully\n");

    printf("\nAll tests completed!\n");
    return 0;
}