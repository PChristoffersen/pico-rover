
add_library(fbus2 STATIC
    src/receiver.cpp
    src/receiver_uart.cpp
    src/mapping.cpp
)
target_include_directories(fbus2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)


target_link_libraries(fbus2
    FreeRTOS-Kernel
    pico_stdlib
    hardware_irq 
    hardware_pio
)
