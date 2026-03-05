idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES BSP user_task user_queue chip utils 
    )
