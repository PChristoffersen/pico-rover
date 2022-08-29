#pragma once

#include <pico/stdlib.h>
#include <rtos.h>

#include "strip.h"

namespace LED::Animation {

    class Base {
        public:
            Base(StripBase &strip) : m_strip { strip } {}
            virtual ~Base() {}

            virtual void start() = 0;
            virtual void stop() = 0;

        protected:
            StripBase &m_strip;
    };

    class Periodic : public Base {
        public:
            using interval_type = uint32_t;

            Periodic(StripBase &strip, interval_type interval) :
                Base { strip },
                m_interval { interval },
                m_task { nullptr },
                m_mutex { nullptr }
            {
                m_mutex = xSemaphoreCreateBinaryStatic(&m_mutex_buf);
            }
            virtual ~Periodic()
            {
                assert(m_task==nullptr);
                if (m_mutex) {
                    vSemaphoreDelete(m_mutex);
                    m_mutex = nullptr;
                }
            }

            virtual void start() override
            {
                xSemaphoreTake(m_mutex, portMAX_DELAY);
                do_update();
                xTaskCreateStatic([](auto args) { reinterpret_cast<Periodic*>(args)->run(); }, "Anim", TASK_STACK_SIZE, this, LED_ANIMATION_TASK_PRIORITY, m_task_stack, &m_task_buf);
                xSemaphoreGive(m_mutex);
            }
            virtual void stop() override
            {
                assert(m_task);
                xSemaphoreTake(m_mutex, portMAX_DELAY);
                vTaskDelete(m_task);
                m_task = nullptr;
                xSemaphoreGive(m_mutex);
            }

        protected:
            interval_type m_interval;
            TaskHandle_t m_task;
            SemaphoreHandle_t m_mutex;

            virtual void do_update() = 0;

        private:
            static constexpr uint TASK_STACK_SIZE { configMINIMAL_STACK_SIZE };
            StaticTask_t m_task_buf;
            StackType_t m_task_stack[TASK_STACK_SIZE];
            StaticSemaphore_t m_mutex_buf;

            inline void run() 
            {
                TickType_t last_time = xTaskGetTickCount();

                while (true) {
                    xSemaphoreTake(m_mutex, portMAX_DELAY);
                    do_update();
                    xSemaphoreGive(m_mutex);
                    xTaskDelayUntil(&last_time, pdMS_TO_TICKS(m_interval));
                }
            }
    };

};
