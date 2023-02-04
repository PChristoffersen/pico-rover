
add_library(i2c_bus STATIC
    src/i2c_bus.cpp
)
target_include_directories(i2c_bus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(i2c_bus PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


target_link_libraries(i2c_bus PRIVATE
    FreeRTOS-Kernel
    pico_stdlib
    hardware_i2c
    hardware_irq
    hardware_dma 
)
