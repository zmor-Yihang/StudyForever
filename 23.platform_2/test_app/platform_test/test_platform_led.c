#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../../lib/libled/libled.h"

#define DEVICE_PATH "/dev/platform_device"

void print_usage(const char *prog_name)
{
    printf("用法: %s [命令]\n", prog_name);
    printf("可用命令:\n");
    printf("  on       - 点亮LED\n");
    printf("  off      - 熄灭LED\n");
    printf("  toggle   - 切换LED状态\n");
    printf("  get      - 获取LED当前状态\n");
    printf("  set 0|1  - 设置LED状态 (0=熄灭, 1=点亮)\n");
    printf("  test     - 运行完整测试序列\n");
    printf("如果不提供命令，将运行交互式模式\n");
}

void test_led_operations(int fd)
{
    int state;
    int ret;
    
    printf("\n=== LED功能测试 ===\n");
    
    // 测试LED关闭
    printf("1. 测试LED关闭...\n");
    ret = led_off(fd);
    if (ret < 0) {
        printf("LED关闭失败: %s\n", strerror(errno));
    } else {
        printf("LED关闭成功\n");
    }
    
    sleep(1);
    
    // 测试LED状态获取
    printf("2. 获取LED当前状态...\n");
    ret = led_get(fd, &state);
    if (ret < 0) {
        printf("获取LED状态失败: %s\n", strerror(errno));
    } else {
        printf("LED当前状态: %s\n", state ? "点亮" : "熄灭");
    }
    
    sleep(1);
    
    // 测试LED点亮
    printf("3. 测试LED点亮...\n");
    ret = led_on(fd);
    if (ret < 0) {
        printf("LED点亮失败: %s\n", strerror(errno));
    } else {
        printf("LED点亮成功\n");
    }
    
    sleep(1);
    
    // 再次测试LED状态获取
    printf("4. 再次获取LED状态...\n");
    ret = led_get(fd, &state);
    if (ret < 0) {
        printf("获取LED状态失败: %s\n", strerror(errno));
    } else {
        printf("LED当前状态: %s\n", state ? "点亮" : "熄灭");
    }
    
    sleep(1);
    
    // 测试LED设置
    printf("5. 测试LED设置为熄灭状态...\n");
    ret = led_set(fd, 0);
    if (ret < 0) {
        printf("LED设置失败: %s\n", strerror(errno));
    } else {
        printf("LED设置为熄灭状态成功\n");
    }
    
    sleep(1);
    
    // 再次获取状态验证
    printf("6. 验证LED状态...\n");
    ret = led_get(fd, &state);
    if (ret < 0) {
        printf("获取LED状态失败: %s\n", strerror(errno));
    } else {
        printf("LED当前状态: %s\n", state ? "点亮" : "熄灭");
    }
    
    sleep(1);
    
    // 测试LED切换
    printf("7. 测试LED切换...\n");
    ret = led_toggle(fd);
    if (ret < 0) {
        printf("LED切换失败: %s\n", strerror(errno));
    } else {
        printf("LED切换成功\n");
    }
    
    sleep(1);
    
    // 最终状态检查
    printf("8. 最终LED状态检查...\n");
    ret = led_get(fd, &state);
    if (ret < 0) {
        printf("获取LED状态失败: %s\n", strerror(errno));
    } else {
        printf("LED最终状态: %s\n", state ? "点亮" : "熄灭");
    }
    
    printf("\n=== LED功能测试完成 ===\n");
}

void interactive_mode(int fd)
{
    char input[256];
    int state;
    int ret;
    
    printf("\n=== LED交互式控制模式 ===\n");
    printf("输入命令 (on, off, toggle, get, set 0|1, quit): ");
    
    while (fgets(input, sizeof(input), stdin)) {
        // 去除换行符
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
            break;
        } else if (strcmp(input, "on") == 0) {
            ret = led_on(fd);
            if (ret < 0) {
                printf("LED点亮失败: %s\n", strerror(errno));
            } else {
                printf("LED点亮成功\n");
            }
        } else if (strcmp(input, "off") == 0) {
            ret = led_off(fd);
            if (ret < 0) {
                printf("LED关闭失败: %s\n", strerror(errno));
            } else {
                printf("LED关闭成功\n");
            }
        } else if (strcmp(input, "toggle") == 0) {
            ret = led_toggle(fd);
            if (ret < 0) {
                printf("LED切换失败: %s\n", strerror(errno));
            } else {
                printf("LED切换成功\n");
            }
        } else if (strcmp(input, "get") == 0) {
            ret = led_get(fd, &state);
            if (ret < 0) {
                printf("获取LED状态失败: %s\n", strerror(errno));
            } else {
                printf("LED当前状态: %s\n", state ? "点亮" : "熄灭");
            }
        } else if (strncmp(input, "set ", 4) == 0) {
            int value = atoi(input + 4);
            if (value != 0 && value != 1) {
                printf("无效的状态值，请使用0(熄灭)或1(点亮)\n");
            } else {
                ret = led_set(fd, value);
                if (ret < 0) {
                    printf("LED设置失败: %s\n", strerror(errno));
                } else {
                    printf("LED设置为%s成功\n", value ? "点亮" : "熄灭");
                }
            }
        } else {
            printf("未知命令: %s\n", input);
            printf("可用命令: on, off, toggle, get, set 0|1, quit\n");
        }
        
        printf("输入命令 (on, off, toggle, get, set 0|1, quit): ");
    }
    
    printf("退出交互模式\n");
}

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    int state;
    
    printf("=== LED平台设备测试程序 ===\n");
    
    // 打开设备
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        printf("无法打开设备 %s: %s\n", DEVICE_PATH, strerror(errno));
        printf("请确保:\n");
        printf("1. 平台设备模块已加载 (sudo insmod LED_PLATFORM_DEVICE.ko)\n");
        printf("2. 平台驱动模块已加载 (sudo insmod PLATFORM_DRIVER.ko)\n");
        printf("3. 设备节点已创建 (ls -l /dev/platform_device)\n");
        printf("4. 有权限访问设备 (sudo chmod 666 /dev/platform_device)\n");
        return EXIT_FAILURE;
    }
    
    printf("成功打开设备 %s\n", DEVICE_PATH);
    
    // 根据命令行参数执行相应操作
    if (argc > 1) {
        if (strcmp(argv[1], "on") == 0) {
            ret = led_on(fd);
            if (ret < 0) {
                printf("LED点亮失败: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
            printf("LED点亮成功\n");
        } else if (strcmp(argv[1], "off") == 0) {
            ret = led_off(fd);
            if (ret < 0) {
                printf("LED关闭失败: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
            printf("LED关闭成功\n");
        } else if (strcmp(argv[1], "toggle") == 0) {
            ret = led_toggle(fd);
            if (ret < 0) {
                printf("LED切换失败: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
            printf("LED切换成功\n");
        } else if (strcmp(argv[1], "get") == 0) {
            ret = led_get(fd, &state);
            if (ret < 0) {
                printf("获取LED状态失败: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
            printf("LED当前状态: %s\n", state ? "点亮" : "熄灭");
        } else if (strcmp(argv[1], "set") == 0 && argc > 2) {
            int value = atoi(argv[2]);
            if (value != 0 && value != 1) {
                printf("无效的状态值，请使用0(熄灭)或1(点亮)\n");
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
            ret = led_set(fd, value);
            if (ret < 0) {
                printf("LED设置失败: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
            printf("LED设置为%s成功\n", value ? "点亮" : "熄灭");
        } else if (strcmp(argv[1], "test") == 0) {
            test_led_operations(fd);
        } else {
            printf("未知命令: %s\n", argv[1]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    } else {
        // 没有提供命令行参数，进入交互模式
        interactive_mode(fd);
    }
    
    // 关闭设备
    close(fd);
    printf("\n测试完成\n");
    
    return EXIT_SUCCESS;
}