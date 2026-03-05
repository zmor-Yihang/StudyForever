# 收集所有子目录的源文件
file(GLOB_RECURSE USER_TASK_SRCS "*/*.c")

# 收集所有子目录作为包含目录
file(GLOB USER_TASK_INCLUDE_DIRS LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
list(FILTER USER_TASK_INCLUDE_DIRS INCLUDE REGEX "^[^.]+$")

# 定义需要的依赖
set(USER_TASK_REQUIRES 
      freertos 
      BSP 
      user_queue 
      esp_timer
      utils
)

idf_component_register(SRCS ${USER_TASK_SRCS}
                       INCLUDE_DIRS ${USER_TASK_INCLUDE_DIRS}
                       REQUIRES ${USER_TASK_REQUIRES})


# idf_component_register(SRCS "task1/task1.c" "task2/task2.c" "task3/task3.c" "taskInit/user_task.c"
#                        INCLUDE_DIRS "task1" "task2" "task3" "taskInit"
#                        REQUIRES freertos BSP user_queue esp_timer)
                       