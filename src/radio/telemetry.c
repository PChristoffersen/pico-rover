#include "telemetry.h"

#include <pico/stdlib.h>

#include "frsky.h"


void radio_telemetry_null(radio_telemetry_t *event) 
{
    event->app_id = 0x0000;
    event->data = 0x00000000;
}


static inline void __radio_telemetry_temperature(radio_telemetry_t *event, uint16_t base, uint8_t offset, uint32_t tempC)
{
    event->app_id = base+offset;
    event->data = tempC;
}

void radio_telemetry_temperature1(radio_telemetry_t *event, uint8_t offset, uint32_t tempC) 
{ 
    __radio_telemetry_temperature(event, FRDID_T1_FIRST_ID, offset, tempC); 
}
void radio_telemetry_temperature2(radio_telemetry_t *event, uint8_t offset, uint32_t tempC) 
{ 
    __radio_telemetry_temperature(event, FRDID_T2_FIRST_ID, offset, tempC); 
}


void radio_telemetry_cells(radio_telemetry_t *event, uint8_t battery_id, uint8_t offset, uint8_t n_cells, float voltage0, float voltage1)
{
    event->app_id = FRDID_CELLS_FIRST_ID+offset;

    uint32_t cv1 = voltage0 * 500;
    uint32_t cv2 = voltage1 * 500;

    event->data = (cv1 & 0x0fff) << 20 | (cv2 & 0x0fff) << 8 | n_cells << 4 | battery_id;
}


void radio_telemetry_a3(radio_telemetry_t *event, uint8_t offset, float voltage)
{
    event->app_id = FRDID_A3_FIRST_ID+offset;
    event->data = voltage*100;
}

void radio_telemetry_a4(radio_telemetry_t *event, uint8_t offset, float voltage)
{
    event->app_id = FRDID_A4_FIRST_ID+offset;
    event->data = voltage*100;
}
