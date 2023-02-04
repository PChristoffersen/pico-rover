
add_library(ina219 STATIC
    src/ina219.cpp
)
target_include_directories(ina219 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(ina219 PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


target_link_libraries(ina219
    FreeRTOS-Kernel
    pico_stdlib
    hardware_i2c
    i2c_bus    
)
