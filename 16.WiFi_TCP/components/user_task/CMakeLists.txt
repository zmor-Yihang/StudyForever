# 收集所有 .c 源文件
file(GLOB USER_TASK_SRCS "*.c")

# 定义包含目录
set(USER_TASK_INCLUDE_DIRS
    "."             # 当前目录（包含所有任务头文件）
)

# 定义需要的依赖
set(USER_TASK_REQUIRES
    freertos        # FreeRTOS（任务管理）
    BSP             # BSP层（硬件驱动接口）
    user_queue      # 用户队列和信号量
    esp_timer       # ESP定时器
    utils           # 工具函数
)

# 注册组件
idf_component_register(
    SRCS ${USER_TASK_SRCS}
    INCLUDE_DIRS ${USER_TASK_INCLUDE_DIRS}
    REQUIRES ${USER_TASK_REQUIRES}
)