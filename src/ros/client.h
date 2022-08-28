#pragma once

#include <pico/stdlib.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <util/callback.h>
#include "pico_tcp_transport.h"

namespace ROS {

    class Client {
        public:
            using connect_callback_type = Callback<Client&>;
            using disconnect_callback_type = Callback<Client&>;

            Client();
            Client(const Client&) = delete; // No copy constructor
            Client(Client&&) = delete; // No move constructor

            void init();

            absolute_time_t update();

            rcl_allocator_t &allocator() { return m_allocator; }
            rcl_node_t      &node()      { assert(m_connected); return m_node; }
            rclc_executor_t &executor()  { assert(m_connected); return m_executor; }
            rclc_support_t  &support()   { assert(m_connected); return m_support; }

        private:
            static constexpr auto NODE_NAME { "pico_node" };
            static constexpr auto NODE_NAMESPACE { "" };
            static constexpr size_t EXECUTOR_HANDLES { 1 };
            static constexpr int PING_TIMEOUT_MS { 1000 };
            static constexpr uint64_t TICK_INTERVAL_US { 1000 };
            static constexpr uint64_t SPIN_TIMEOUT_US { 1000 };
            static constexpr int64_t CONNECT_INTERVAL_US { 
                #ifdef RASPBERRYPI_PICO_W
                2000000 
                #else
                100000 
                #endif
            };

            bool m_connected;
            absolute_time_t m_last_connect;

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

    };

}
