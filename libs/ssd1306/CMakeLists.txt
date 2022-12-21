
add_library(ssd1306 STATIC
    src/display.cpp
)
target_include_directories(ssd1306 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)


target_link_libraries(ssd1306 PRIVATE
    FreeRTOS-Kernel
    pico_stdlib
)

if(NOT PICO_PLATFORM STREQUAL "host")
target_link_libraries(ssd1306 PRIVATE
    hardware_i2c
    i2c_bus    
)
endif()