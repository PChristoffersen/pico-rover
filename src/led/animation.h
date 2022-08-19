#pragma once

#include <pico/stdlib.h>

#include "strip.h"

namespace LED::Animation {

    class Base {
        public:
            Base(StripBase &strip) : m_strip { strip } {}

            virtual void begin() = 0;

            virtual absolute_time_t update() = 0;
        protected:
            StripBase &m_strip;
    };

    class Periodic : public Base {
        public:
            using interval_type = int64_t;

            Periodic(StripBase &strip, interval_type interval) :
                Base { strip },
                m_last_update { get_absolute_time() }
            {
            }

            void begin() override
            {
                m_last_update = get_absolute_time();
                do_update();
            }

            virtual absolute_time_t update() override 
            {
                if (absolute_time_diff_us(m_last_update, get_absolute_time())>m_interval) {

                    m_last_update = delayed_by_us(m_last_update, m_interval);
                }
                return delayed_by_us(m_last_update, m_interval);
            }

        protected:
            absolute_time_t m_last_update;
            interval_type m_interval;

            virtual void do_update() = 0;
    };

};
