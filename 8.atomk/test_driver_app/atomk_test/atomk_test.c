#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/wait.h>

#define DEVICE_PATH "/dev/cdev_led0"
#define BUFFER_SIZE 64
#define NUM_THREADS 20  // 增加线程数以提高并发压力
#define ITERATIONS_PER_THREAD 10  // 增加迭代次数

// 全局变量，用于同步线程启动
static volatile int start_flag = 0;
static pthread_barrier_t start_barrier;

// 线程参数结构体
typedef struct {
    int thread_id;
    int success_count;
    int fail_count;
    double total_time;
    int max_concurrent_opens;  // 记录最大同时打开数
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
    int current_concurrent_opens = 0;
    
    // 等待所有线程准备就绪
    pthread_barrier_wait(&start_barrier);
    
    printf("Thread %d starting\n", data->thread_id);
    
    for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
        start_time = get_time_ms();
        
        // 尝试打开设备
        fd = open(DEVICE_PATH, O_WRONLY);
        if (fd < 0) {
            data->fail_count++;
            printf("Thread %d, iteration %d: Failed to open device: %s\n", 
                   data->thread_id, i, strerror(errno));
            // 短暂延迟后重试
            usleep(10000); // 减少等待时间以增加并发压力
            continue;
        }
        
        // 成功打开设备，增加计数
        current_concurrent_opens++;
        if (current_concurrent_opens > data->max_concurrent_opens) {
            data->max_concurrent_opens = current_concurrent_opens;
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
        current_concurrent_opens--;
        
        end_time = get_time_ms();
        data->total_time += (end_time - start_time);
        
        // 极短延迟以增加并发竞争
        usleep(5000); // 减少等待时间到5ms
    }
    
    printf("Thread %d completed: Success=%d, Fail=%d, Max concurrent opens=%d, Avg time=%.2fms\n", 
           data->thread_id, data->success_count, data->fail_count,
           data->max_concurrent_opens,
           data->success_count > 0 ? data->total_time / data->success_count : 0);
    
    return NULL;
}

// 测试原子性 - 验证只有一个进程能打开设备
void test_atomicity() {
    printf("\n=== 测试原子性 - 验证只有一个进程能打开设备 ===\n");
    printf("使用 %d 个线程同时尝试访问设备，每个线程尝试 %d 次\n", NUM_THREADS, ITERATIONS_PER_THREAD);
    
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    
    // 初始化屏障，确保所有线程同时开始
    pthread_barrier_init(&start_barrier, NULL, NUM_THREADS);
    
    // 创建线程
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].success_count = 0;
        thread_data[i].fail_count = 0;
        thread_data[i].total_time = 0.0;
        thread_data[i].max_concurrent_opens = 0;
        
        if (pthread_create(&threads[i], NULL, concurrent_access_test, &thread_data[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }
    
    // 等待所有线程完成
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 销毁屏障
    pthread_barrier_destroy(&start_barrier);
    
    // 统计结果
    int total_success = 0, total_fail = 0;
    double total_time = 0.0;
    int max_concurrent_opens = 0;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        total_success += thread_data[i].success_count;
        total_fail += thread_data[i].fail_count;
        total_time += thread_data[i].total_time;
        if (thread_data[i].max_concurrent_opens > max_concurrent_opens) {
            max_concurrent_opens = thread_data[i].max_concurrent_opens;
        }
    }
    
    printf("\n=== 原子性测试结果 ===\n");
    printf("总成功次数: %d\n", total_success);
    printf("总失败次数: %d\n", total_fail);
    printf("总尝试次数: %d\n", total_success + total_fail);
    printf("成功率: %.2f%%\n", 
           (total_success + total_fail) > 0 ? (100.0 * total_success / (total_success + total_fail)) : 0);
    printf("平均操作时间: %.2fms\n", 
           total_success > 0 ? total_time / total_success : 0);
    printf("单个线程最大同时打开数: %d\n", max_concurrent_opens);
    
    // 验证原子性
    printf("\n=== 原子性分析 ===\n");
    if (total_fail == 0) {
        printf("❌ 原子性测试失败: 所有 %d 次访问都成功，原子性保护未生效！\n", total_success);
        printf("   这表明多个线程可以同时打开设备，违反了独占访问原则。\n");
    } else if (total_fail > total_success) {
        printf("✅ 原子性测试通过: 有 %d 次访问被正确拒绝，%d 次成功\n", total_fail, total_success);
        printf("   失败率 %.2f%% 表明原子性保护有效工作\n", 
               (100.0 * total_fail / (total_success + total_fail)));
    } else {
        printf("⚠️  原子性测试部分通过: 有 %d 次访问被拒绝，%d 次成功\n", total_fail, total_success);
        printf("   失败率 %.2f%% 可能表明原子性保护工作但不够严格\n", 
               (100.0 * total_fail / (total_success + total_fail)));
    }
    
    // 额外检查：如果成功次数远大于失败次数，可能原子性保护不够严格
    if (total_success > total_fail * 2) {
        printf("⚠️  警告: 成功次数 (%d) 远大于失败次数 (%d)，原子性保护可能不够严格\n", 
               total_success, total_fail);
    }
}

// 测试严格原子性 - 使用更严格的方法检测原子性保护
void test_strict_atomicity() {
    printf("\n=== 严格原子性测试 - 检测原子性保护是否生效 ===\n");
    printf("此测试将尝试同时打开设备多次，如果原子性保护生效，应该只有一次成功\n");
    
    int num_processes = 10;
    int pipe_fds[2];
    pid_t pids[num_processes];
    int success_count = 0;
    int fail_count = 0;
    
    // 创建管道用于进程间通信
    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        return;
    }
    
    // 创建多个子进程同时尝试打开设备
    for (int i = 0; i < num_processes; i++) {
        pids[i] = fork();
        
        if (pids[i] == 0) {
            // 子进程
            close(pipe_fds[0]); // 关闭读端
            
            int fd = open(DEVICE_PATH, O_WRONLY);
            if (fd < 0) {
                // 打开失败，向父进程发送失败信号
                int result = 0; // 0表示失败
                write(pipe_fds[1], &result, sizeof(result));
                close(pipe_fds[1]);
                exit(EXIT_SUCCESS);
            } else {
                // 打开成功，向父进程发送成功信号
                int result = 1; // 1表示成功
                write(pipe_fds[1], &result, sizeof(result));
                
                // 保持设备打开一段时间
                sleep(1);
                close(fd);
                close(pipe_fds[1]);
                exit(EXIT_SUCCESS);
            }
        } else if (pids[i] < 0) {
            perror("fork");
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            return;
        }
    }
    
    // 父进程
    close(pipe_fds[1]); // 关闭写端
    
    // 读取所有子进程的结果
    for (int i = 0; i < num_processes; i++) {
        int result;
        if (read(pipe_fds[0], &result, sizeof(result)) > 0) {
            if (result == 1) {
                success_count++;
            } else {
                fail_count++;
            }
        }
    }
    
    close(pipe_fds[0]);
    
    // 等待所有子进程结束
    for (int i = 0; i < num_processes; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    printf("\n=== 严格原子性测试结果 ===\n");
    printf("成功打开设备的进程数: %d\n", success_count);
    printf("失败打开设备的进程数: %d\n", fail_count);
    printf("总进程数: %d\n", num_processes);
    
    // 分析结果
    printf("\n=== 原子性保护分析 ===\n");
    if (success_count == 1) {
        printf("✅ 原子性保护完全生效: 只有1个进程成功打开设备，%d个进程被正确拒绝\n", fail_count);
        printf("   这表明设备实现了严格的独占访问，符合原子性要求。\n");
    } else if (success_count == 0) {
        printf("❌ 异常情况: 没有任何进程能够打开设备，可能设备存在问题\n");
    } else {
        printf("❌ 原子性保护失效: 有 %d 个进程同时成功打开设备！\n", success_count);
        printf("   这表明原子性保护未生效，多个进程可以同时访问设备。\n");
        printf("   预期结果应该只有1个进程能成功打开设备。\n");
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
    
    // 严格原子性测试
    test_strict_atomicity();
    
    printf("\n测试完成\n");
    return EXIT_SUCCESS;
}