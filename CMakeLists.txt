# Cmake Pico project file
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_PATH "/opt/pico/pico-sdk")
#set(FREERTOS_KERNEL_PATH "/opt/pico/FreeRTOS-Kernel-SMP")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(cmake/pico_sdk_import.cmake)
if(NOT PICO_PLATFORM STREQUAL "host")
include(cmake/FreeRTOS_Kernel_import.cmake)
else()
include(cmake/FreeRTOS_Kernel_posix.cmake)
endif()
include(cmake/image_generate.cmake)
include(cmake/font_generate.cmake)


project(pico-rover LANGUAGES C CXX ASM)

set(PICO_BOARD pico)
#set(PICO_BOARD pico_w)


# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()


if(NOT PICO_PLATFORM STREQUAL "host")

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/boardconfig.cpp
    src/robot.cpp
    src/rtos.c
    src/usb_descriptors.c
    src/usb_device.c
    src/wifi.c
    src/led/color.cpp
    src/led/color_hsv.cpp
    src/led/control.cpp
    src/led/cyw43_led.cpp
    src/led/gpio_led.cpp
    src/led/strip.cpp
    src/motor/dcmotor.cpp
    src/motor/encoder.cpp
    src/motor/servo.cpp
    src/oled/control.cpp
    src/oled/ssd1306_display.cpp
    src/radio/radio.cpp
    src/sensor/bno055.cpp
    src/sensor/pico_adc.cpp
    src/sensor/ina219.cpp
    src/telemetry/provider.cpp
    src/util/debug.cpp
    src/util/i2c_bus.cpp
    src/watchdog/watchdog.cpp
    src/ros/client.cpp
    src/ros/hal.c
    src/ros/pico_cdc_transport.cpp
)


pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_description(${PROJECT_NAME} "Pico Rover controller")
pico_set_program_version(${PROJECT_NAME} "0.1")


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-unused-parameter  -Wno-unused-variable  -Wno-type-limits)
    pico_enable_stdio_usb(${PROJECT_NAME} 0)
    pico_enable_stdio_semihosting(${PROJECT_NAME} 0)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        BOARD_GPIO_STDIO=1
    )
else()
    #target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror -Wno-unused-parameter)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter)
    #pico_enable_stdio_usb(${PROJECT_NAME} 1)
endif()


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
target_compile_definitions(${PROJECT_NAME} PRIVATE PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
target_compile_definitions(${PROJECT_NAME} PRIVATE PICO_STDIO_DEFAULT_CRLF=0)
target_compile_definitions(${PROJECT_NAME} PRIVATE PICO_DEFAULT_UART_TX_PIN=16)
target_compile_definitions(${PROJECT_NAME} PRIVATE PICO_DEFAULT_UART_RX_PIN=17)
target_compile_definitions(${PROJECT_NAME} PRIVATE PICO_DEFAULT_I2C_SDA_PIN=21)
target_compile_definitions(${PROJECT_NAME} PRIVATE PICO_DEFAULT_I2C_SCL_PIN=20)

set_source_files_properties(
                ${FREERTOS_KERNEL_PATH}/tasks.c
                ${FREERTOS_KERNEL_PATH}/stream_buffer.c
                ${FREERTOS_KERNEL_PATH}/event_groups.c
                ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/port.c
                PROPERTIES
                COMPILE_FLAGS "-Wno-sign-compare -Wno-type-limits -Wno-error -Wno-unused")
    
set_source_files_properties(
                ${PICO_TINYUSB_PATH}/hw/bsp/rp2040/family.c
                ${PICO_TINYUSB_PATH}/src/tusb.c
                PROPERTIES
                COMPILE_FLAGS "-Wno-type-limits")

# Add PIO programs
pico_generate_pio_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/led/ws2812.pio OUTPUT_DIR ${PROJECT_BINARY_DIR}/generated)
pico_generate_pio_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/motor/quadrature_encoder.pio OUTPUT_DIR ${PROJECT_BINARY_DIR}/generated)
pico_generate_pio_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/util/uart_tx.pio OUTPUT_DIR ${PROJECT_BINARY_DIR}/generated)

image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/raspberry_logo.bmp Raspberry_Logo ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/battery.bmp Battery ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/warning.bmp Warning ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/no_con.bmp NoConnection ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/signal_1.bmp Signal1 ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/signal_2.bmp Signal2 ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/signal_3.bmp Signal3 ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/signal_4.bmp Signal4 ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/check.bmp Check ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/cancel.bmp Cancel ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/timer.bmp Timer ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/tool.bmp Tool ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/face.bmp Face ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/lock_closed.bmp LockClosed ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/lock_open.bmp LockOpen ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/power_button.bmp PowerButton ${PROJECT_BINARY_DIR}/generated/resources)
image_generate_header(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/resources/images/bullet.bmp Bullet ${PROJECT_BINARY_DIR}/generated/resources)
font_generate_header(${PROJECT_NAME} LiberationMono 16 LiberationMono_16 ${PROJECT_BINARY_DIR}/generated/resources)
font_generate_header(${PROJECT_NAME} LiberationMono 24 LiberationMono_24 ${PROJECT_BINARY_DIR}/generated/resources)
font_generate_header(${PROJECT_NAME} LiberationSans:Bold 16 LiberationSans_16 ${PROJECT_BINARY_DIR}/generated/resources)
font_generate_header(${PROJECT_NAME} LiberationSans:Bold 24 LiberationSans_24 ${PROJECT_BINARY_DIR}/generated/resources)


# Add the standard library to the build
target_link_libraries(${PROJECT_NAME} 
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    pico_stdlib
    pico_unique_id
    pico_multicore
    tinyusb_device 
    tinyusb_board
    fbus2
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


# Add wifi
if(PICO_BOARD STREQUAL "pico_w")
    target_link_libraries(${PROJECT_NAME}
        #pico_cyw43_arch_lwip_threadsafe_background
        pico_cyw43_arch_lwip_sys_freertos
    )
endif()


# Add Micro-ROS
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/libs/micro_ros_raspberrypi_pico_sdk/libmicroros/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/libs/micro_ros_raspberrypi_pico_sdk/libmicroros)
target_link_libraries(${PROJECT_NAME} microros)


add_subdirectory(libs)



pico_add_extra_outputs(${PROJECT_NAME})

else() # PICO_PLATFORM == HOST

enable_testing() 

add_subdirectory(test)

endif()


