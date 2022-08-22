#include "telemetryprovider.h"

#include <robot.h>
#include <math.h>

TelemetryProvider::TelemetryProvider(Robot &robot) :
    m_robot { robot }, 
    m_primary { 0 },
    m_secondary { 0 }
{

}


void TelemetryProvider::init()
{
    #ifndef NDEBUG
    m_last_print = get_absolute_time();
    #endif
}




Radio::FrSky::Telemetry TelemetryProvider::get_secondary()
{
    switch (m_secondary++) {
        case 0:
            {
                auto voltage = m_robot.battery_sensor().get_bus_voltage();
                return Radio::FrSky::Telemetry::cells(0x00, 0, 2, voltage/2.0f, voltage/2.0f);
            }
        case 1:
            {
                auto current = m_robot.battery_sensor().get_current();
                return Radio::FrSky::Telemetry::current(0x00, current);
            }
        case 2:
            {
                auto temp = m_robot.sys_sensor().get_temp();
                return Radio::FrSky::Telemetry::temperature1(0, temp);
            }
        case 4:
            {
                m_secondary = 0;
                auto voltage = m_robot.sys_sensor().get_vsys();
                return Radio::FrSky::Telemetry::a3(0, voltage);
            }
    }

    return Radio::FrSky::Telemetry::null();
}


Radio::FrSky::Telemetry TelemetryProvider::get_next_telemetry()
{
    m_count++;

    auto prim = m_primary++;
    switch (prim) {
        case 0:
        case 1:
        case 2:
        case 3:
            {
                auto &encoder = m_robot.motors()[prim].encoder();
                return Radio::FrSky::Telemetry::rpm(encoder.id(), encoder.rpm());
            }
        case 4:
            return Radio::FrSky::Telemetry::diy(0, m_robot.imu().heading()*180.0f/static_cast<float>(M_PI));
        case 5:
            return Radio::FrSky::Telemetry::diy(1, m_robot.imu().pitch()*180.0f/static_cast<float>(M_PI));
        case 6:
            return Radio::FrSky::Telemetry::diy(2, m_robot.imu().roll()*180.0f/static_cast<float>(M_PI));
        default:
            m_primary = 0;
            return get_secondary();
    }

    return Radio::FrSky::Telemetry::null();
}



#ifndef NDEBUG
void TelemetryProvider::print_stats()
{
    auto now = get_absolute_time();
    auto tdiff = absolute_time_diff_us(m_last_print, now);
    auto cdiff = m_count-m_last_count;

    float prc = 0.0f;
    if (tdiff>=0) {
        prc = static_cast<float>(cdiff)*1000000.0f/tdiff;
    }

    printf("Telemetry: count=%u  dc=%u  dt=%llu    %.2f\n", m_count, cdiff, tdiff, prc);
    m_last_count = m_count;
    m_last_print = now;
}
#endif
