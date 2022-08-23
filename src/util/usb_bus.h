#pragma once

#include <pico/stdlib.h>

static constexpr uint8_t ROS_CDC { 0 };

void usb_bus_init();

absolute_time_t usb_bus_update();

