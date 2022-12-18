#pragma once

#include <pico/stdlib.h>
#include <rtos.h>
#include <string_view>

#include <boardconfig.h>
#include "colorlayer.h"

namespace LED::Animation {

    class Base {
        public:
            using color_type = Color::RGBA;
            using layer_type = Color::Layer<color_type, LED_STRIP_PIXEL_COUNT>;

            Base(layer_type &layer) : m_layer { layer } {}
            virtual ~Base() {}

            virtual void start(TickType_t now) = 0;
            virtual void stop() {};
            virtual void update(TickType_t now) {}

        protected:
            layer_type &m_layer;

    };


    class Periodic : public Base {
        public:
            using interval_type = TickType_t;

            Periodic(layer_type &layer, interval_type interval) :
                Base { layer },
                m_interval { interval }
            {
            }

            virtual void start(TickType_t now)
            {
                m_last_update = now;
                do_update();
            }

            virtual void update(TickType_t now) override 
            {
                if (now-m_last_update >= m_interval) {
                    do_update();
                    m_last_update += m_interval;
                }
            }

        protected:
            interval_type m_interval;

            virtual void do_update() = 0;

        private:
            TickType_t m_last_update;
    };

};
