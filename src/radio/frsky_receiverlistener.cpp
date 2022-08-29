#include "frsky_receiverlistener.h"


namespace Radio::FrSky {

ReceiverListener::ReceiverListener() 
{
}


void ReceiverListener::run()
{
    while (true) {
        auto res = xMessageBufferReceive(m_buffer, &m_channels, sizeof(m_channels), portMAX_DELAY);
        if (res!=sizeof(channels_type)) {
            xMessageBufferReset(m_buffer);
            continue;
        }

        m_mapping.set(m_channels);
        m_callback(m_channels, m_mapping);
    }
}


void ReceiverListener::init(Receiver &receiver)
{
    m_buffer = xMessageBufferCreateStatic(sizeof(m_buffer_data), m_buffer_data, &m_buffer_buf);
    assert(m_buffer);

    m_task = xTaskCreateStatic([](auto args){ reinterpret_cast<ReceiverListener*>(args)->run(); }, "RCListener", TASK_STACK_SIZE, this, RECEIVERLISTENER_TASK_PRIORITY, m_task_stack, &m_task_buf);
    assert(m_task);

    receiver.add_callback([this](auto channels) {
        xMessageBufferSend(m_buffer, &channels, sizeof(channels), 0);
    });

}

}