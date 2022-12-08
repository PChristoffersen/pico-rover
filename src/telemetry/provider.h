#pragma once

#include <pico/stdlib.h>
#include <radio/frsky_telemetry.h>

class Robot;

namespace Telemetry {

    class Provider : public Radio::FrSky::TelemetryProvider {
        public:
            Provider(Robot &robot);

            void init();

            virtual  Radio::FrSky::Telemetry get_next_telemetry();

            #ifndef NDEBUG
            absolute_time_t m_last_print;
            uint m_last_count;
            void print_stats();
            #endif

        private:
            class Robot &m_robot;

            uint m_count;
            uint m_primary;
            uint m_secondary;

            inline Radio::FrSky::Telemetry get_secondary();
    };

}
