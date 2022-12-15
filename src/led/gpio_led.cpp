#include "gpio_led.h"

#include <stdio.h>
#include <pico/stdlib.h>

#include "../boardconfig.h"

namespace LED {

GPIOLed::GPIOLed(uint pin) :
    m_task { nullptr },
    m_pin { pin },
    m_blinking { false },
    m_interval { pdMS_TO_TICKS(DEFAULT_BLINK_INTERVAL_MS) }
{
    m_mutex = xSemaphoreCreateMutexStatic(&m_mutex_buf);
    configASSERT(m_mutex);
    xSemaphoreGive(m_mutex);
}


void GPIOLed::set(bool on)
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_state = on;
    gpio_put(m_pin, m_state);
    if (m_blinking) {
        vTaskSuspend(m_task);
        m_blinking = false;
    }
    xSemaphoreGive(m_mutex);
}


inline void GPIOLed::run()
{
    while (true) {
        xSemaphoreTake(m_mutex, portMAX_DELAY);
        m_state = !m_state;
        gpio_put(m_pin, m_state);
        xSemaphoreGive(m_mutex);
        vTaskDelay(m_interval);
    }
}




void GPIOLed::init()
{
    gpio_init(m_pin);
    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, true);

    m_task = xTaskCreateStatic([](auto arg){ reinterpret_cast<GPIOLed*>(arg)->run(); }, "LED", TASK_STACK_SIZE, this, LED_BLINK_TASK_PRIORITY, m_task_stack, &m_task_buf);
    configASSERT(m_task);
    vTaskSuspend(m_task);
    m_blinking = false;
}



void GPIOLed::blink()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    if (m_blinking) 
        return;
    vTaskResume(m_task);
    m_blinking = true;
    xSemaphoreGive(m_mutex);
}


}
