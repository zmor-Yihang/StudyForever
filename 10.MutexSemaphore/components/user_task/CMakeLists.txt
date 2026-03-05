idf_component_register(
    SRCS "task1.c" "task2.c" "task3.c" "user_task.c"
    INCLUDE_DIRS "."
    REQUIRES freertos BSP user_queue esp_timer utils
)