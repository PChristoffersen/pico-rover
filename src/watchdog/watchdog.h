#pragma once

#include <pico/stdlib.h>

namespace Watchdog {

    class Watchdog {
        public:
            Watchdog();

            bool caused_reboot() const;

            void init();
            void term();

            void begin();

            absolute_time_t ping_core0();
            absolute_time_t ping_core1();


        private:
            static constexpr uint32_t INTERVAL      { 1000u }; // Watchdog reset interval in ms
            static constexpr uint32_t PING_INTERVAL { INTERVAL/4u }; // Ping watchdog 4 per reset interval

            absolute_time_t m_last_core1_ping;
    };

}