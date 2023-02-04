#include "radio.h"

namespace Radio {


void Receiver::on_data(const channels_type &channels)
{
    m_mapping.set(channels);
    m_control_callback(*this, channels, m_mapping);
}

Telemetry Receiver::get_next_telemetry() 
{
    return m_telemetry_provider->get_next_telemetry();
}


}