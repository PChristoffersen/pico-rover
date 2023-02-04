#pragma once

#include <memory>
#include <array>
#include <pico/stdlib.h>
#include <rtos.h>

#include <led/strip.h>
#include <led/colorbuffer.h>
#include <led/colorlayer.h>
#include <led/animation.h>
#include <led/animation/indicators.h>
#include <led/animation/headlights.h>

#include <boardconfig.h>
#include <util/lockable.h>
#include "gpio_led.h"
#include "cyw43_led.h"

class Robot;

namespace LED {

    class Control : public Lockable {
        public:
            #ifdef RASPBERRYPI_PICO_W
            using led_type = CYW43Led;
            #else
            using led_type = GPIOLed;
            #endif
            using strip_type = Strip<LED_STRIP_PIXEL_COUNT>;

            enum class AnimationMode : uint {
                BLACK,
                BLINK,
                CHASE,
                COLOR_CYCLE,
                KNIGHT_RIDER,
                _LAST
            };

            enum class LightMode : uint {
                OFF,
                ON,
            };

            using IndicatorMode = Animation::Indicators<LED_STRIP_PIXEL_COUNT>::Mode;

            Control(Robot &robot);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor

            void init();
            void start();

            void update_connected(bool connected);

            void set_animation_mode(AnimationMode mode);
            void set_light_mode(LightMode mode);
            void set_indicator_mode(IndicatorMode mode);
            void set_brightness(strip_type::brightness_type brightness);


            led_type &builtin() { return m_builtin; };
            strip_type &strip() { return m_strip; }

        private:
            static constexpr uint TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
            static constexpr uint UPDATE_INTERVAL_MS { 5u };

            static constexpr size_t NUM_ANIMATIONS { static_cast<size_t>(AnimationMode::_LAST) };
            using color_buffer_type = Color::Buffer<Color::RGBW, LED_STRIP_PIXEL_COUNT>;
            using animation_type = std::unique_ptr<Animation::Base>;
            using animation_layer_type = Color::Layer<Color::RGBA, LED_STRIP_PIXEL_COUNT>;
            using light_layer_type = Color::Layer<Color::RGBA, LED_STRIP_PIXEL_COUNT>;
            using light_type = Animation::Headlights<LED_STRIP_PIXEL_COUNT>;
            using indicator_type = Animation::Indicators<LED_STRIP_PIXEL_COUNT>;
            using indicator_layer_type = Color::Layer<Color::RGBA, LED_STRIP_PIXEL_COUNT>;

            StaticTask_t m_task_buf;
            StackType_t  m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            led_type   m_builtin;
            strip_type m_strip;
            color_buffer_type m_buffer;

            Robot &m_robot;

            animation_type m_animation;
            animation_layer_type m_animations_layer;
            AnimationMode m_animation_mode;
            AnimationMode m_animation_mode_set;

            light_type m_light;
            light_layer_type m_light_layer;
            LightMode m_light_mode;
            LightMode m_light_mode_set;

            indicator_type m_indicator;
            indicator_layer_type m_indicator_layer;
            IndicatorMode m_indicator_mode_set;

            strip_type::brightness_type m_brightness_set;

            inline void run();

            inline bool update_state(TickType_t now);
            inline void update_animation(TickType_t now);
            inline void draw_buffer();

    };

}