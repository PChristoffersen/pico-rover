#include "control.h"

#include <limits>

#include <led/colorlayer.h>
#include <led/animation/knightrider.h>
#include <led/animation/colorcycle.h>
#include <led/animation/blink.h>
#include <led/animation/solid.h>
#include <led/animation/chase.h>

namespace LED {

Control::Control(Robot &robot):
    m_task { nullptr },
    #ifdef RASPBERRYPI_PICO_W
    m_builtin { CYW43_WL_GPIO_LED_PIN },
    #else    
    m_builtin { PICO_DEFAULT_LED_PIN },
    #endif
    m_strip { LED_STRIP_PIO, LED_STRIP_PIN, LED_STRIP_IS_RGBW },
    m_robot { robot },
    m_animation_mode  { AnimationMode::_LAST },
    m_animation_mode_set { AnimationMode::BLACK },
    m_light { m_light_layer },
    m_light_mode { LightMode::OFF },
    m_light_mode_set { LightMode::OFF },
    m_indicator { m_indicator_layer },
    m_indicator_mode_set { IndicatorMode::NONE },
    m_brightness_set { m_strip.get_brightness() }
{
}



inline bool Control::update_state(TickType_t now)
{
    bool dirty = false;

    lock();
    if (m_animation_mode != m_animation_mode_set) {
        if (m_animation) {
            m_animation->stop();
            dirty = true;
        }

        printf("SetMode: %u -> %u\n", static_cast<uint>(m_animation_mode), static_cast<uint>(m_animation_mode_set));
        m_animation_mode = m_animation_mode_set;

        switch (m_animation_mode) {
            case AnimationMode::BLINK:
                m_animation = std::make_unique<Animation::Blink<LED_STRIP_PIXEL_COUNT>>(m_animations_layer, Color::RGB::MAGENTA*0.1, 100000);
                break;
            case AnimationMode::CHASE:
                m_animation = std::make_unique<Animation::Chase<LED_STRIP_PIXEL_COUNT>>(m_animations_layer);
                break;
            case AnimationMode::COLOR_CYCLE:
                m_animation = std::make_unique<Animation::ColorCycle<LED_STRIP_PIXEL_COUNT>>(m_animations_layer);
                break;
            case AnimationMode::KNIGHT_RIDER:
                m_animation = std::make_unique<Animation::KnightRider<LED_STRIP_PIXEL_COUNT>>(m_animations_layer, 6, LED_STRIP_PIXEL_COUNT/2-6);
                break;
            default:
                m_animation = std::make_unique<Animation::Solid<LED_STRIP_PIXEL_COUNT>>(m_animations_layer, Color::RGB::BLACK);
        }

        if (m_animation) {
            m_animation->start(now);
        }
    }
    if (m_light_mode != m_light_mode_set) {
        m_light_mode = m_light_mode_set;
        switch (m_light_mode) {
            case LightMode::OFF:
                m_light_layer.set_visible(false);
                m_light.stop();
                break;
            case LightMode::ON:
                m_light.start(now);
                m_light_layer.set_visible(true);
                break;
        }
    }

    if (std::abs(m_brightness_set-m_strip.get_brightness()) > 0.05f) {
        m_strip.set_brightness(m_brightness_set);
        dirty = true;
    }

    m_indicator.set_mode(m_indicator_mode_set);
    m_indicator_mode_set = m_indicator.get_mode();

    unlock();

    return dirty;
}


inline void Control::update_animation(TickType_t now)
{
    m_animation->update(now);
    m_light.update(now);
    m_indicator.update(now);
}


inline void Control::draw_buffer()
{
    m_buffer.fill(Color::RGB::BLACK);

    if (m_animations_layer.is_visible()) {
        m_buffer << m_animations_layer;
        m_animations_layer.clear_dirty();
    }
    if (m_light_layer.is_visible()) {
        m_buffer << m_light_layer;
        m_light_layer.clear_dirty();
    }
    if (m_indicator_layer.is_visible()) {
        m_buffer << m_indicator_layer;
        m_indicator_layer.clear_dirty();
    }
}


inline void Control::run()
{
    bool dirty = false;
    TickType_t last_update = xTaskGetTickCount();

    m_indicator.start(last_update);

    while (true) {
        xTaskDelayUntil(&last_update, pdMS_TO_TICKS(UPDATE_INTERVAL_MS));

        dirty = update_state(last_update);
        update_animation(last_update);

        // Check if we need to update
        dirty |= m_animations_layer.is_dirty();
        dirty |= m_light_layer.is_dirty();
        dirty |= m_indicator_layer.is_dirty();

        if (!dirty) {
            continue;
        }

        // Update display if needed
        draw_buffer();

        m_strip.show(m_buffer);
    }
}


void Control::init()
{
    m_builtin.init();
    m_strip.init();

    m_animations_layer.fill(Color::RGBA::TRANSPARENT);
    m_animations_layer.set_visible(true);
 
    m_indicator_layer.fill(Color::RGBA::TRANSPARENT);
    m_indicator_layer.set_visible(false);
    m_indicator_mode_set = m_indicator.get_mode();

    m_task = xTaskCreateStatic([](auto arg){ reinterpret_cast<Control*>(arg)->run(); }, "LEDStrip", TASK_STACK_SIZE, this, LED_TASK_PRIORITY, m_task_stack, &m_task_buf);
    assert(m_task);
    vTaskSuspend(m_task);
}


void Control::start() 
{
    m_builtin.blink();
    vTaskResume(m_task);
}


void Control::update_connected(bool connected)
{
    /*
    if (connected) {
        m_animation = std::make_unique<Animation::KnightRider>(m_strip);
        m_animation->begin();
    }
    else {
        set_mode(99);
        m_animation = std::make_unique<Animation::Blink>(m_strip, Color::MAGENTA*0.1, 100000);
        m_animation->begin();
    }
    */
}


void Control::set_animation_mode(AnimationMode mode) 
{
    lock();
    m_animation_mode_set = mode;
    unlock();
}


void Control::set_light_mode(LightMode mode) 
{
    lock();
    m_light_mode_set = mode;
    unlock();
}


void Control::set_indicator_mode(IndicatorMode mode)
{
    lock();
    m_indicator_mode_set = mode;
    unlock();
}


void Control::set_brightness(strip_type::brightness_type brightness)
{
    lock();
    m_brightness_set = brightness;
    unlock();
}


}