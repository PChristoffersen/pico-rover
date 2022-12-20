#include "control.h"

#include <limits>

#include "colorlayer.h"
#include "animation/knightrider.h"
#include "animation/colorcycle.h"
#include "animation/blink.h"
#include "animation/solid.h"
#include "animation/chase.h"

namespace LED {

Control::Control(Robot &robot):
    m_mutex { nullptr },
    m_task { nullptr },
    #ifdef RASPBERRYPI_PICO_W
    m_builtin { CYW43_WL_GPIO_LED_PIN },
    #else    
    m_builtin { PICO_DEFAULT_LED_PIN },
    #endif
    m_strip { LED_STRIP_PIO, LED_STRIP_PIN, LED_STRIP_IS_RGBW },
    m_robot { robot },
    m_animation_mode  { AnimationMode::_LAST },
    m_animation_mode_set { AnimationMode::KNIGHT_RIDER },
    m_indicator { m_indicator_layer },
    m_indicator_mode_set { IndicatorMode::NONE }
{
    m_mutex = xSemaphoreCreateMutexStatic(&m_mutex_buf);
    configASSERT(m_mutex);
    xSemaphoreGive(m_mutex);
}



inline void Control::update_modes(TickType_t now)
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    if (m_animation_mode != m_animation_mode_set) {
        if (m_animation) {
            m_animation->stop();
        }

        printf("SetMode: %u -> %u\n", static_cast<uint>(m_animation_mode_set), static_cast<uint>(m_animation_mode));
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
    m_indicator.set_mode(m_indicator_mode_set);
    m_indicator_mode_set = m_indicator.get_mode();
    xSemaphoreGive(m_mutex);
}


inline void Control::update_animation(TickType_t now)
{
    m_animation->update(now);
    m_indicator.update(now);
}


inline void Control::draw_buffer()
{
    m_buffer.fill(Color::RGB::BLACK);

    if (m_animations_layer.is_visible()) {
        m_buffer << m_animations_layer;
        m_animations_layer.clear_dirty();
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

        update_modes(last_update);
        update_animation(last_update);

        // Check if we need to update
        dirty  = m_animations_layer.is_dirty() && m_animations_layer.is_visible();
        dirty |= m_indicator_layer.is_dirty() && m_indicator_layer.is_visible();

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
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_animation_mode_set = mode;
    xSemaphoreGive(m_mutex);
}


void Control::set_indicator_mode(IndicatorMode mode)
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_indicator_mode_set = mode;
    xSemaphoreGive(m_mutex);
}


}