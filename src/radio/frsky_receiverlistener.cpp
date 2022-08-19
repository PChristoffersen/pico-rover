#include "frsky_receiverlistener.h"


namespace Radio::FrSky {

ReceiverListener::ReceiverListener() 
{
}

void ReceiverListener::init(Receiver &receiver)
{
    queue_init(&m_queue, sizeof(Channels), QUEUE_SIZE);

    m_last_event = make_timeout_time_us(POLL_INTERVAL);

    receiver.add_callback([this](auto channels) {
        queue_try_add(&m_queue, &channels);
    });

}


absolute_time_t ReceiverListener::update()
{
    if (absolute_time_diff_us(m_last_event, get_absolute_time())>POLL_INTERVAL) {
        if (queue_try_remove(&m_queue, &m_channels)) {
            m_mapping.set(m_channels);
            m_callback(m_channels, m_mapping);
        }
        m_last_event = delayed_by_us(m_last_event, POLL_INTERVAL);
    }
    return delayed_by_us(m_last_event, POLL_INTERVAL);
}


}
