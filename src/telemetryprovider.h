#pragma once

#include <pico/stdlib.h>
#include <radio/frsky_telemetry.h>

class TelemetryProvider : public Radio::FrSky::TelemetryProvider {
    public:
        TelemetryProvider(class Robot &robot);

        void init();

        virtual Radio::FrSky::Telemetry get_next_telemetry();


        #ifndef NDEBUG
        void print_stats();
        #endif

    private:
        static constexpr uint BATTERY_CELL_FREQUENCY  { 32 };
        static constexpr uint BATTERY_CUR_FREQUENCY  { 32 };
        static constexpr uint CPU_TEMP_FREQUENCY { 32 }; 
        static constexpr uint VSYS_FREQUENCY     { 32 }; 

        class Robot &m_robot;

        uint m_count;

        uint m_last_battery_cell;
        uint m_last_battery_cur;
        uint m_last_cpu_temp;
        uint m_last_vsys;

        uint m_next_encoder;

        // Stats
        uint m_battery_count;
        uint m_cpu_temp_count;
        uint m_vsys_count;
        uint m_rpm_count;
};
