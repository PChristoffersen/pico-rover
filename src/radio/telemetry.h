/**
 * @author Peter Christoffersen
 * @brief Radio receiver telemetry  
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>

#include "frsky.h"

typedef struct {
    uint16_t app_id;
    uint32_t data;
} radio_telemetry_t;


void radio_telemetry_null(radio_telemetry_t *event);
void radio_telemetry_temperature1(radio_telemetry_t *event, uint8_t offset, uint32_t tempC); 
void radio_telemetry_temperature2(radio_telemetry_t *event, uint8_t offset, uint32_t tempC);
void radio_telemetry_cells(radio_telemetry_t *event, uint8_t battery_id, uint8_t offset, uint8_t n_cells, float voltage0, float voltage1);
void radio_telemetry_a3(radio_telemetry_t *event, uint8_t offset, float voltage);
void radio_telemetry_a4(radio_telemetry_t *event, uint8_t offset, float voltage);


