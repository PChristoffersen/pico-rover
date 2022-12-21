
add_library(ina219 STATIC
    src/ina219.cpp
)
target_include_directories(ina219 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)


target_link_libraries(ina219
    FreeRTOS-Kernel
    pico_stdlib
    hardware_i2c
    i2c_bus    
)
