# -----------------------------------------------
# Tests
# -----------------------------------------------

add_library(pico_test INTERFACE)

target_include_directories(pico_test INTERFACE ${PICO_SDK_PATH}/test/pico_test/include )
target_link_libraries(pico_test INTERFACE 
    FreeRTOS-Kernel
    pico_stdlib
    pico_stdio
    hardware_sync
    hardware_gpio
    hardware_uart
)


add_executable(test_dummy test_dummy.cpp)
target_include_directories(test_dummy PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_dummy PRIVATE pico_test)
add_test(NAME Dummy COMMAND test_dummy)
pico_add_extra_outputs(test_dummy)


add_executable(test_color test_color.cpp)
target_include_directories(test_color PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_color PRIVATE pico_test)
add_test(NAME Color COMMAND test_color)
pico_add_extra_outputs(test_color)
