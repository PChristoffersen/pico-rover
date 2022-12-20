# -----------------------------------------------
# Tests
# -----------------------------------------------

# Add GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    DOWNLOAD_EXTRACT_TIMESTAMP
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

function(rover_add_test TARGET)
    set(flags)
    set(args)
    set(listArgs SOURCES)
    cmake_parse_arguments(arg "${flags}" "${args}" "${listArgs}" ${ARGN})
    add_executable(${TARGET} ${arg_SOURCES})
    target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${TARGET} PRIVATE pico_test)
    pico_add_extra_outputs(${TARGET})
    gtest_discover_tests(${TARGET})
endfunction()



add_library(pico_test INTERFACE)
target_include_directories(pico_test INTERFACE 
    ${CMAKE_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/generated
)
target_link_libraries(pico_test INTERFACE 
    FreeRTOS-Kernel
    pico_stdlib
    pico_stdio
    hardware_sync
    hardware_gpio
    hardware_uart
    GTest::gtest_main
)


rover_add_test(test_color SOURCES 
    test_color.cpp
    ${CMAKE_SOURCE_DIR}/src/led/color.cpp
    ${CMAKE_SOURCE_DIR}/src/led/color_hsv.cpp
)

rover_add_test(test_animation SOURCES 
    test_animation.cpp
    ${CMAKE_SOURCE_DIR}/src/led/color.cpp
    ${CMAKE_SOURCE_DIR}/src/led/color_hsv.cpp
)

rover_add_test(test_oled SOURCES 
    test_oled.cpp
)
