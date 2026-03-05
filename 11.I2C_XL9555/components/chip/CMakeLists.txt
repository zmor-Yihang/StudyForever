# 收集所有 .c 源文件
file(GLOB USER_QUEUE_SRCS "xl9555.c" "*.c")

# 定义包含目录
set(USER_QUEUE_INCLUDE_DIRS
    "."             # 当前目录（包含所有头文件）
)

# 定义需要的依赖
set(USER_QUEUE_REQUIRES
    BSP         # BSP（板级支持包）
)

# 注册组件
idf_component_register(
    SRCS ${USER_QUEUE_SRCS}
    INCLUDE_DIRS ${USER_QUEUE_INCLUDE_DIRS}
    REQUIRES ${USER_QUEUE_REQUIRES}
)