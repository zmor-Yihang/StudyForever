# 收集所有 .c 源文件（不包含子目录）
file(GLOB UTILS_SRCS "*.c")

# 定义包含目录
set(UTILS_INCLUDE_DIRS
    "."             # 当前目录（包含 utils_delay.h, fifofast.h）
    "macros"        # 宏定义目录
    "macros/com"    # 通用宏
    "macros/mpl"    # 元编程宏
)

# 定义需要的依赖
set(UTILS_REQUIRES
    freertos        # FreeRTOS 用于延时函数
)

# 注册组件
idf_component_register(
    SRCS ${UTILS_SRCS}
    INCLUDE_DIRS ${UTILS_INCLUDE_DIRS}
    REQUIRES ${UTILS_REQUIRES}
)
