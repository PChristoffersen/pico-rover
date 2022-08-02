# Cmake Pico project file
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
 
# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_PATH "/opt/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
#include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake) 

project(pico-rover LANGUAGES C CXX ASM)
 
# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(${PROJECT_NAME} 
    src/main.c
    src/debug/debug.c
    src/board.c
    src/led/led.c
    src/led/ledstrip.c
    src/motor/motor.c
    src/oled/oled.c
    src/radio/receiver.c
    src/radio/telemetry.c
    src/servo/servo.c
    src/sensors/board_adc.c
    src/sensors/ina219.c
)

pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_description(${PROJECT_NAME} "Pico Rover controller")
pico_set_program_version(${PROJECT_NAME} "0.1")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    pico_enable_stdio_usb(${PROJECT_NAME} 0)
    pico_enable_stdio_semihosting(${PROJECT_NAME} 0 v)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PICO_DEFAULT_UART_TX_PIN=16
        PICO_DEFAULT_UART_RX_PIN=17
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GENERIC_INTR_STDIO=1
    )

else()
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE
    PICO_DEFAULT_I2C_SDA_PIN=21
    PICO_DEFAULT_I2C_SCL_PIN=20
)



# Add PIO programs
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/led/ws2812.pio OUTPUT_DIR ${PROJECT_BINARY_DIR}/generated)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/motor/quadrature_encoder.pio OUTPUT_DIR ${PROJECT_BINARY_DIR}/generated)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/debug/uart_tx.pio OUTPUT_DIR ${PROJECT_BINARY_DIR}/generated)
 
# Add the standard library to the build
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    pico_unique_id
    pico_multicore
)
 
# Add any user requested libraries
target_link_libraries(${PROJECT_NAME} 
    hardware_irq 
    hardware_i2c
    hardware_pwm 
    hardware_pio 
    hardware_dma 
    hardware_adc
)


pico_add_extra_outputs(${PROJECT_NAME})
