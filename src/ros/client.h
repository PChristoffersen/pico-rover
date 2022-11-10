#pragma once

#include <pico/stdlib.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <rtos.h>
#include <util/callback.h>

namespace ROS {

    class Client {
        public:
            using connect_callback_type = Callback<Client&>;
            using disconnect_callback_type = Callback<Client&>;

            Client();
            Client(const Client&) = delete; // No copy constructor
            Client(Client&&) = delete; // No move constructor

            void init();

            rcl_allocator_t &allocator() { return m_allocator; }
            rcl_node_t      &node()      { assert(m_connected); return m_node; }
            rclc_executor_t &executor()  { assert(m_connected); return m_executor; }
            rclc_support_t  &support()   { assert(m_connected); return m_support; }

        private:
            static constexpr uint TASK_STACK_SIZE { 2*configMINIMAL_STACK_SIZE };
            static constexpr size_t BUFFER_SIZE { 256u };
            static constexpr auto NODE_NAME { "pico_node" };
            static constexpr auto NODE_NAMESPACE { "" };
            static constexpr size_t EXECUTOR_HANDLES { 1 };
            static constexpr int PING_TIMEOUT_MS { 1000 };
            static constexpr uint64_t SPIN_TIMEOUT_US { 100000 };
            static constexpr int64_t CONNECT_INTERVAL_MS { 
                #ifdef RASPBERRYPI_PICO_W
                2000
                #else
                100
                #endif
            };

            StaticTask_t m_task_buf;
            StackType_t m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            bool m_connected;
            StreamBufferHandle_t m_buffer;
            StaticStreamBuffer_t m_buffer_buf;
            uint8_t m_buffer_data[BUFFER_SIZE];


            rcl_allocator_t m_allocator;
            rcl_node_t      m_node;
            rclc_support_t  m_support;
            rclc_executor_t m_executor;

            connect_callback_type m_connect_cb;
            disconnect_callback_type m_disconnect_cb;

            inline void transport_connect();
            inline bool transport_is_connected();
            bool on_connect();
            void on_disconnect();

            void run();
    };

}
