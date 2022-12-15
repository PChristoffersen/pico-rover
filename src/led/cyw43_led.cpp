#include "cyw43_led.h"

#include <stdio.h>
#include <pico/stdlib.h>

#ifdef RASPBERRYPI_PICO_W

#include <pico/cyw43_arch.h>

namespace LED {

CYW43Led::CYW43Led(uint pin) :
    m_task { nullptr },
    m_pin { pin },
    m_blinking { false },
    m_interval { pdMS_TO_TICKS(DEFAULT_BLINK_INTERVAL_MS) }
{
    m_mutex = xSemaphoreCreateMutexStatic(&m_mutex_buf);
    configASSERT(m_mutex);
    xSemaphoreGive(m_mutex);
}


void CYW43Led::set(bool on)
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_state = on;
    cyw43_arch_gpio_put(m_pin, m_state);
    if (m_blinking) {
        vTaskSuspend(m_task);
        m_blinking = false;
    }
    xSemaphoreGive(m_mutex);
}


inline void CYW43Led::run()
{
    while (true) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        m_state = !m_state;
        cyw43_arch_gpio_put(m_pin, m_state);
        xSemaphoreGive(m_mutex);
        vTaskDelay(m_interval);
    }
}




void CYW43Led::init()
{
    cyw43_arch_gpio_put(m_pin, m_state);

    m_task = xTaskCreateStatic([](auto arg){ reinterpret_cast<CYW43Led*>(arg)->run(); }, "LED", TASK_STACK_SIZE, this, LED_BLINK_TASK_PRIORITY, m_task_stack, &m_task_buf);
    configASSERT(m_task);
    vTaskSuspend(m_task);
    m_blinking = false;
}



void CYW43Led::blink()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    if (m_blinking) 
        return;
    vTaskResume(m_task);
    m_blinking = true;
    xSemaphoreGive(m_mutex);
}


}

#endif
