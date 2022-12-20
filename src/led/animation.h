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

            virtual ~Base() {}

            virtual void start(TickType_t now) = 0;
            virtual void stop() {};
            virtual void update(TickType_t now) {}

    };

    template<size_t NLEDS>
    class BaseTemplate : public Base {
        public:
            using layer_type = Color::Layer<color_type, NLEDS>;
            
            BaseTemplate(layer_type &layer) : m_layer { layer } {}

        protected:
            layer_type &m_layer;

    };

    template<size_t NLEDS>
    class Periodic : public BaseTemplate<NLEDS> {
        public:
            using base_type = BaseTemplate<NLEDS>;
            using interval_type = TickType_t;
            using layer_type = typename base_type::layer_type;

            Periodic(layer_type &layer, interval_type interval) :
                BaseTemplate<NLEDS> { layer },
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
