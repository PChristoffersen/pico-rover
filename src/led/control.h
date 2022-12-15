#pragma once

#include <memory>
#include <array>
#include <pico/stdlib.h>
#include <rtos.h>

#include <boardconfig.h>
#include "strip.h"
#include "gpio_led.h"
#include "cyw43_led.h"
#include "animation.h"
#include "colorbuffer.h"
#include "colorlayer.h"

class Robot;

namespace LED {

    class Control {
        public:
            #ifdef RASPBERRYPI_PICO_W
            using led_type = CYW43Led;
            #else
            using led_type = GPIOLed;
            #endif
            using strip_type = Strip<LED_STRIP_PIXEL_COUNT>;

            enum class Mode : uint {
                BLACK,
                BLINK,
                CHASE,
                COLOR_CYCLE,
                KNIGHT_RIDER,
                _LAST
            };

            Control(Robot &robot);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor

            void init();

            void update_connected(bool connected);

            void set_mode(Mode mode);


            led_type &builtin() { return m_builtin; };
            strip_type &strip() { return m_strip; }

        private:
            static constexpr uint TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
            static constexpr uint UPDATE_INTERVAL_MS { 25u };

            static constexpr size_t NUM_ANIMATIONS { static_cast<size_t>(Mode::_LAST) };
            using animations_type = std::array<std::unique_ptr<Animation::Base>, NUM_ANIMATIONS>;

            using color_buffer_type = Color::Buffer<Color::RGBW, LED_STRIP_PIXEL_COUNT>;
            using animations_layer_type = Color::Layer<Color::RGBA, LED_STRIP_PIXEL_COUNT>;

            StaticSemaphore_t m_mutex_buf;
            SemaphoreHandle_t m_mutex;

            StaticTask_t m_task_buf;
            StackType_t  m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            led_type   m_builtin;
            strip_type m_strip;

            Robot &m_robot;

            Mode m_mode;

            animations_type m_animations;
            animations_layer_type m_animations_layer;

            inline void run();

            inline void update_animation(TickType_t now);
            inline void draw_buffer(color_buffer_type &buffer);
    };

}