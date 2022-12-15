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
    m_mode  { Mode::BLACK }
{
    m_mutex = xSemaphoreCreateMutexStatic(&m_mutex_buf);
    configASSERT(m_mutex);
    xSemaphoreGive(m_mutex);

    m_animations[static_cast<uint>(Mode::BLACK)] = std::make_unique<Animation::Solid>(m_animations_layer, Color::RGB::BLACK);
    m_animations[static_cast<uint>(Mode::BLINK)] = std::make_unique<Animation::Blink>(m_animations_layer, Color::RGB::MAGENTA*0.1, 100000);
    m_animations[static_cast<uint>(Mode::CHASE)] = std::make_unique<Animation::Chase>(m_animations_layer);
    m_animations[static_cast<uint>(Mode::COLOR_CYCLE)] = std::make_unique<Animation::ColorCycle>(m_animations_layer);
    m_animations[static_cast<uint>(Mode::KNIGHT_RIDER)] = std::make_unique<Animation::KnightRider>(m_animations_layer, 4, 24-8);
}




inline void Control::update_animation(TickType_t now)
{
    if (m_animations[static_cast<uint>(m_mode)]) {
        m_animations[static_cast<uint>(m_mode)]->stop();
    }
}


inline void Control::draw_buffer(color_buffer_type &buffer)
{
    buffer.fill(Color::RGB::BLACK);

    if (m_animations_layer.visible()) {
        buffer.paint(m_animations_layer);
        m_animations_layer.setDirty(false);
    }
}


inline void Control::run()
{
    TickType_t last_time = xTaskGetTickCount();
    color_buffer_type buffer;
    bool dirty = false;

    while (true) {
        xTaskDelayUntil(&last_time, pdMS_TO_TICKS(UPDATE_INTERVAL_MS));

        xSemaphoreTake(m_mutex, portMAX_DELAY);

        update_animation(last_time);

        // Check if we need to update
        dirty = m_animations_layer.dirty() && m_animations_layer.visible();

        if (dirty) {
            xSemaphoreGive(m_mutex);
            continue;
        }

        // Update display if needed
        draw_buffer(buffer);

        xSemaphoreGive(m_mutex);

        m_strip.show(buffer);
    }
}


void Control::init()
{
    m_builtin.init();
    m_strip.init();

    m_animations[static_cast<uint>(m_mode)]->start();

    m_task = xTaskCreateStatic([](auto arg){ reinterpret_cast<Control*>(arg)->run(); }, "LED", TASK_STACK_SIZE, this, LED_TASK_PRIORITY, m_task_stack, &m_task_buf);
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


void Control::set_mode(Mode mode) 
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);

    if (mode==m_mode)
        goto out;

    if (static_cast<size_t>(mode) >= static_cast<size_t>(Mode::_LAST)) 
        goto out;

    if (m_animations[static_cast<uint>(m_mode)]) {
        m_animations[static_cast<uint>(m_mode)]->stop();
    }

    m_mode = mode;
    if (m_animations[static_cast<uint>(m_mode)]) {
        m_animations[static_cast<uint>(m_mode)]->start();
    }

  out:
    xSemaphoreGive(m_mutex);
}



}