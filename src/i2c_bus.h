/**
 * @author Peter Christoffersen
 * @brief Helper function for using the i2c bus
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>

void i2c_bus_init();

void i2c_bus_acquire_blocking();
bool i2c_bus_acquire_timeout_us(uint32_t timeout_us);
bool i2c_bus_try_acquire();
void i2c_bus_release();

void i2c_bus_write_dma(uint8_t addr);

void i2c_dma_buffer_reset();
void i2c_dma_buffer_append(const uint8_t *src, size_t len);

