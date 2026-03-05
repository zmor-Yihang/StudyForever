#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

#define DEVICE_PATH "/dev/cdev_led0"
#define BUFFER_SIZE 64
#define NUM_THREADS 10
#define ITERATIONS_PER_THREAD 5

// 线程参数结构体
typedef struct {
    int thread_id;
    int success_count;
    int fail_count;
    double total_time;
} thread_data_t;

// 获取当前时间（毫秒）
double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

// 线程函数 - 尝试并发访问设备
void* concurrent_access_test(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int fd;
    char command[BUFFER_SIZE];
    ssize_t bytes_written;
    double start_time, end_time;
    
    printf("Thread %d starting\n", data->thread_id);
    
    for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
        start_time = get_time_ms();
        
        // 尝试打开设备
        fd = open(DEVICE_PATH, O_WRONLY);
        if (fd < 0) {
            data->fail_count++;
            printf("Thread %d, iteration %d: Failed to open device: %s\n", 
                   data->thread_id, i, strerror(errno));
            usleep(100000); // 等待100ms后重试
            continue;
        }
        
        // 准备命令
        snprintf(command, BUFFER_SIZE, "toggle");
        
        // 向设备写入命令
        bytes_written = write(fd, command, strlen(command));
        if (bytes_written < 0) {
            printf("Thread %d, iteration %d: Failed to write to device: %s\n", 
                   data->thread_id, i, strerror(errno));
            data->fail_count++;
        } else {
            data->success_count++;
        }
        
        // 关闭设备文件
        close(fd);
        
        end_time = get_time_ms();
        data->total_time += (end_time - start_time);
        
        // 短暂延迟
        usleep(50000); // 等待50ms
    }
    
    printf("Thread %d completed: Success=%d, Fail=%d, Avg time=%.2fms\n", 
           data->thread_id, data->success_count, data->fail_count,
           data->success_count > 0 ? data->total_time / data->success_count : 0);
    
    return NULL;
}

// 测试原子性 - 验证只有一个进程能打开设备
void test_atomicity() {
    printf("\n=== 测试原子性 - 验证只有一个进程能打开设备 ===\n");
    
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    
    // 创建线程
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].success_count = 0;
        thread_data[i].fail_count = 0;
        thread_data[i].total_time = 0.0;
        
        if (pthread_create(&threads[i], NULL, concurrent_access_test, &thread_data[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }
    
    // 等待所有线程完成
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 统计结果
    int total_success = 0, total_fail = 0;
    double total_time = 0.0;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        total_success += thread_data[i].success_count;
        total_fail += thread_data[i].fail_count;
        total_time += thread_data[i].total_time;
    }
    
    printf("\n=== 原子性测试结果 ===\n");
    printf("总成功次数: %d\n", total_success);
    printf("总失败次数: %d\n", total_fail);
    printf("成功率: %.2f%%\n", 
           (total_success + total_fail) > 0 ? (100.0 * total_success / (total_success + total_fail)) : 0);
    printf("平均操作时间: %.2fms\n", 
           total_success > 0 ? total_time / total_success : 0);
    
    // 验证原子性
    if (total_fail > 0) {
        printf("✓ 原子性测试通过: 有 %d 次访问被正确拒绝，说明原子性保护有效\n", total_fail);
    } else {
        printf("⚠ 原子性测试可能未通过: 所有访问都成功，可能原子性保护未生效\n");
    }
}

// 测试独占访问 - 一个进程打开设备后，其他进程无法打开
void test_exclusive_access() {
    printf("\n=== 测试独占访问 - 一个进程打开设备后，其他进程无法打开 ===\n");
    
    int fd1, fd2;
    
    // 第一个进程打开设备
    fd1 = open(DEVICE_PATH, O_WRONLY);
    if (fd1 < 0) {
        printf("第一个进程无法打开设备: %s\n", strerror(errno));
        return;
    }
    
    printf("第一个进程成功打开设备\n");
    
    // 第二个进程尝试打开同一个设备
    fd2 = open(DEVICE_PATH, O_WRONLY);
    if (fd2 < 0) {
        printf("✓ 独占访问测试通过: 第二个进程无法打开设备 (%s)\n", strerror(errno));
    } else {
        printf("⚠ 独占访问测试可能未通过: 第二个进程也能打开设备\n");
        close(fd2);
    }
    
    // 关闭第一个进程的设备
    close(fd1);
    printf("第一个进程已关闭设备\n");
    
    // 再次尝试打开设备
    fd2 = open(DEVICE_PATH, O_WRONLY);
    if (fd2 < 0) {
        printf("关闭后仍无法打开设备: %s\n", strerror(errno));
    } else {
        printf("✓ 关闭后可以重新打开设备\n");
        close(fd2);
    }
}

int main() {
    printf("原子性防止并发访问测试程序\n");
    printf("设备路径: %s\n", DEVICE_PATH);
    
    // 测试独占访问
    test_exclusive_access();
    
    // 测试原子性
    test_atomicity();
    
    printf("\n测试完成\n");
    return EXIT_SUCCESS;
}