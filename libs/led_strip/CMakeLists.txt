
add_library(led_strip STATIC
    src/color_hsv.cpp
    src/color.cpp
    src/strip.cpp
)
target_include_directories(led_strip PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(led_strip PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


# Add PIO programs
pico_generate_pio_header(led_strip ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio OUTPUT_DIR ${PROJECT_BINARY_DIR}/generated)

target_link_libraries(led_strip
    FreeRTOS-Kernel
    pico_stdlib
)

if(NOT PICO_PLATFORM STREQUAL "host")
target_link_libraries(led_strip
    hardware_pio
    hardware_irq
    hardware_dma 
)
endif()