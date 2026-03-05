# idf_component_register(SRCS "led/led.c" "key/key.c" "uart/uart.c" "systimer/systimer.c"
#                              "gentimer/gentimer.c" "pwm/pwm.c"
#                        INCLUDE_DIRS "led" "key" "uart" "systimer" "gentimer" "pwm"
#                        REQUIRES esp_driver_gpio esp_driver_uart esp_timer
#                        esp_driver_gptimer esp_driver_ledc user_queue
#                        )


# 收集所有子目录的源文件
file(GLOB_RECURSE BSP_SRCS "*/*.c")

# 收集所有子目录作为包含目录
file(GLOB BSP_INCLUDE_DIRS LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
list(FILTER BSP_INCLUDE_DIRS INCLUDE REGEX "^[^.]+$")

# 定义需要的依赖
set(BSP_REQUIRES 
      esp_driver_gpio 
      esp_driver_uart 
      esp_timer
      esp_driver_gptimer 
      esp_driver_ledc
      user_queue
)

idf_component_register(SRCS ${BSP_SRCS}
                       INCLUDE_DIRS ${BSP_INCLUDE_DIRS}
                       REQUIRES ${BSP_REQUIRES})
                       