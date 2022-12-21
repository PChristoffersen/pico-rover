
if(NOT PICO_PLATFORM STREQUAL "host")
add_subdirectory(i2c_bus)
add_subdirectory(bno055)
add_subdirectory(ina219)
add_subdirectory(fbus2)
endif()
add_subdirectory(ssd1306)
add_subdirectory(led_strip)
