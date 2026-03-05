idf_component_register(
    SRCS "user_MutexSemaphore.c" "user_CountingSemaphore.c" "user_queue.c"
    INCLUDE_DIRS "."
    REQUIRES freertos 
)