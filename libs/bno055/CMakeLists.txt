
add_library(bno055 STATIC
    src/bno055.cpp
)
target_include_directories(bno055 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)


target_link_libraries(bno055
    FreeRTOS-Kernel
    pico_stdlib
    hardware_i2c
    i2c_bus    
)
