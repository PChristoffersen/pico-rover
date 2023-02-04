
add_library(fbus2 STATIC
    src/receiver.cpp
    src/receiver_uart.cpp
    src/receiver_pio.cpp
    src/mapping.cpp
)
target_include_directories(fbus2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_include_directories(fbus2 PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

pico_generate_pio_header(fbus2 ${CMAKE_CURRENT_SOURCE_DIR}/src/uart_tx.pio OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
pico_generate_pio_header(fbus2 ${CMAKE_CURRENT_SOURCE_DIR}/src/uart_rx.pio OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)


target_link_libraries(fbus2
    FreeRTOS-Kernel
    pico_stdlib
    hardware_irq 
    hardware_pio
)
