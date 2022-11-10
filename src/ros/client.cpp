#include "client.h"

#include <stdio.h>
#include <pico/stdlib.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <rmw_microros/rmw_microros.h>


#include "pico_cdc_transport.h"


namespace ROS {



static inline void RCCHECK(rcl_ret_t rc) {
    assert(rc==RCL_RET_OK);
}


Client::Client():
    m_task { nullptr },
    m_connected { false }
{
    m_allocator = rcl_get_default_allocator();

}


void Client::init()
{
    m_buffer = xStreamBufferCreateStatic(BUFFER_SIZE, 1, m_buffer_data, &m_buffer_buf);
    assert(m_buffer);

    rmw_uros_set_custom_transport(
        true,
        m_buffer,
        #ifdef RASPBERRYPI_PICO_W
        pico_tcp_transport_open,
        pico_tcp_transport_close,
        pico_tcp_transport_write,
        pico_tcp_transport_read
        #else
        pico_cdc_transport_open,
        pico_cdc_transport_close,
        pico_cdc_transport_write,
        pico_cdc_transport_read
        #endif
    );

    m_task = xTaskCreateStatic([](auto args){ reinterpret_cast<Client*>(args)->run(); }, "ROS", TASK_STACK_SIZE, this, ROS_TASK_PRIORITY, m_task_stack, &m_task_buf);
    assert(m_task);
}


inline void Client::transport_connect()
{
    #ifdef RASPBERRYPI_PICO_W
    #endif
}

inline bool Client::transport_is_connected()
{
    #ifdef RASPBERRYPI_PICO_W
    return pico_tcp_transport_connected();
    #else
    return pico_cdc_transport_connected();
    #endif
}


bool Client::on_connect()
{
    printf("ROS Connected\n");

    rcl_ret_t ret = rmw_uros_ping_agent(PING_TIMEOUT_MS, 1);
    if (ret != RCL_RET_OK) {
        return false;
    }
    printf("ROS Got ping\n");

    RCCHECK(rclc_support_init(&m_support, 0, NULL, &m_allocator));
    RCCHECK(rclc_node_init_default(&m_node, NODE_NAME, NODE_NAMESPACE, &m_support));
    RCCHECK(rclc_executor_init(&m_executor, &m_support.context, EXECUTOR_HANDLES, &m_allocator));

    m_connect_cb(*this);

    m_connected = true;
    return true;
}


void Client::on_disconnect()
{
    printf("ROS Disconnect\n");

    m_disconnect_cb(*this);

    RCCHECK(rclc_executor_fini(&m_executor));
    RCCHECK(rcl_node_fini(&m_node));
    RCCHECK(rclc_support_fini(&m_support));

    m_connected = false;
}



void Client::run()
{
    TickType_t last_time = xTaskGetTickCount();

    while (true) {
        while (!m_connected) {
            if (xTaskGetTickCount()-last_time >= pdMS_TO_TICKS(CONNECT_INTERVAL_MS)) {
                transport_connect();
                last_time = xTaskGetTickCount();
            }
            if (transport_is_connected()) {
                if (on_connect()) {
                    break;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(CONNECT_INTERVAL_MS));
        }

        rclc_executor_spin_some(&m_executor, RCL_US_TO_NS(SPIN_TIMEOUT_US));
    }
}



#if 0
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;

rcl_timer_t timer;
rcl_node_t node;
rcl_allocator_t allocator;
rclc_support_t support;
rclc_executor_t executor;





void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
    printf("Bonk\n");
    rcl_ret_t ret = rcl_publish(&publisher, &msg, NULL);
    msg.data++;
}


void ros_init()
{
    printf("ROS::Init %d\n", __LINE__);

    // Wait for agent successful ping for 2 minutes.
    const int timeout_ms = 1000; 
    const uint8_t attempts = 120;

    //return;

    printf("ROS::Init %d\n", __LINE__);
    rcl_ret_t ret = rmw_uros_ping_agent(timeout_ms, attempts);
    printf("ROS::Init %d\n", __LINE__);

    if (ret != RCL_RET_OK)
    {
        // Unreachable agent, exiting program.
        assert(false);
    }

    allocator = rcl_get_default_allocator();


    printf("ROS::Init %d\n", __LINE__);
    rclc_support_init(&support, 0, NULL, &allocator);
    printf("ROS::Init %d\n", __LINE__);

    rclc_node_init_default(&node, "pico_node", "", &support);
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "pico_publisher");

    rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(1000),
        timer_callback);

    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_timer(&executor, &timer);

    msg.data = 0;

    printf("ROS::Init %d\n", __LINE__);
}


absolute_time_t ros_update()
{
    rclc_executor_spin_some(&executor, RCL_US_TO_NS(100));
    return make_timeout_time_us(1000);
}


#endif
}
