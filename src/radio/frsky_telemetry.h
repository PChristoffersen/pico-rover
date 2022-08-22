/**
 * @author Peter Christoffersen
 * @brief Radio receiver telemetry  
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <string.h>
#include <stdio.h>
#include <pico/stdlib.h>

#include "frsky_protocol.h"


namespace Radio::FrSky {

    struct Telemetry {
        using app_id_type = uint16_t;
        using data_type = uint32_t;

        app_id_type app_id;
        data_type data;

        static Telemetry null()
        {
            return { 
                .app_id = 0x0000, 
                .data = 0x00000000, 
            };
        }

        static Telemetry temperature1(uint8_t offset, uint32_t tempC) 
        { 
            return { 
                .app_id = static_cast<app_id_type>(FRDID_T1_FIRST_ID+offset),
                .data   = tempC,
            };
        }

        static Telemetry temperature2(uint8_t offset, uint32_t tempC) 
        { 
            return { 
                .app_id = static_cast<app_id_type>(FRDID_T2_FIRST_ID+offset),
                .data   = tempC,
            };
        }


        static Telemetry cells(uint8_t battery_id, uint8_t offset, uint8_t n_cells, float voltage0, float voltage1)
        {
            uint16_t app_id = FRDID_CELLS_FIRST_ID+offset;
            uint32_t cv1 = voltage0 * 500;
            uint32_t cv2 = voltage1 * 500;
            uint32_t data = (cv1 & 0x0fff) << 20 | (cv2 & 0x0fff) << 8 | n_cells << 4 | battery_id;
            return {
                .app_id = app_id,
                .data   = data,
            };
        }


        static Telemetry a3(uint8_t offset, float voltage)
        {
            return {
                .app_id = static_cast<app_id_type>(FRDID_A3_FIRST_ID+offset),
                .data   = static_cast<data_type>((voltage*100.0f)+0.5f),
            };
        }

        static Telemetry a4(uint8_t offset, float voltage)
        {
            return {
                .app_id = static_cast<app_id_type>(FRDID_A4_FIRST_ID+offset),
                .data   = static_cast<data_type>((voltage*100.0f)+0.5f),
            };
        }


        static Telemetry current(uint8_t offset, float current_ma)
        {
            return {
                .app_id = static_cast<app_id_type>(FRDID_CURR_FIRST_ID+offset),
                .data   = static_cast<data_type>((current_ma*10.0f)+0.5f),
            };
        }

        static Telemetry rpm(uint8_t offset, float rpm)
        {
            int32_t val = 10.0f*rpm + 0.5f;
            return {
                .app_id = static_cast<app_id_type>(FRDID_RPM_FIRST_ID+offset),
                .data   = (data_type)val,
            };
        }

        static Telemetry sbec(uint8_t offset, float voltage, float current) 
        {
            return {
                .app_id = static_cast<app_id_type>(FRDID_SBEC_POWER_FIRST_ID+offset),
                .data   = static_cast<data_type>(static_cast<uint16_t>(current)<<16 | static_cast<uint16_t>((voltage*1000.0f)+0.5f)),
            };
        }

        
        static Telemetry diy(uint16_t off , float data)
        {
            int32_t val = 10.0f*data+0.5f;
            return {
                .app_id = static_cast<app_id_type>(FRDID_DIY_FIRST_ID+off),
                .data   = (data_type)val,
            };
        }
        static Telemetry diy(uint16_t off , int32_t data)
        {
            return {
                .app_id = static_cast<app_id_type>(FRDID_DIY_FIRST_ID+off),
                .data   = (data_type)data,
            };
        }
        static Telemetry diy(uint16_t off , uint32_t data)
        {
            return {
                .app_id = static_cast<app_id_type>(FRDID_DIY_FIRST_ID+off),
                .data   = (data_type)data,
            };
        }
    };


    class TelemetryProvider {
        protected:
            friend class Receiver;
            virtual Telemetry get_next_telemetry() = 0;
    };

}