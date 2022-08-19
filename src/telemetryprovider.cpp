#include "telemetryprovider.h"

#include <robot.h>

TelemetryProvider::TelemetryProvider(Robot &robot) :
    m_robot { robot }
{

}


void TelemetryProvider::init()
{
}



Radio::FrSky::Telemetry TelemetryProvider::get_next_telemetry()
{
    m_count++;

    if (m_count-m_last_battery_cell > BATTERY_CELL_FREQUENCY) {
        m_last_battery_cell = m_count;
        m_battery_count++;
        
        auto voltage = m_robot.battery_sensor().get_bus_voltage();
        return Radio::FrSky::Telemetry::cells(0x00, 0, 2, voltage/2.0f, voltage/2.0f);
    }
    if (m_count-m_last_battery_cur > BATTERY_CUR_FREQUENCY) {
        m_last_battery_cur = m_count;
        m_battery_count++;
        
        auto current = m_robot.battery_sensor().get_current();
        return Radio::FrSky::Telemetry::current(0x00, current);
    }

    if (m_count-m_last_cpu_temp > CPU_TEMP_FREQUENCY) {
        m_last_cpu_temp = m_count;
        m_cpu_temp_count++;

        auto temp = m_robot.sys_sensor().get_temp();
        return Radio::FrSky::Telemetry::temperature1(0, temp);
    }

    if (m_count-m_last_vsys > VSYS_FREQUENCY) {
        m_last_vsys = m_count;
        m_vsys_count++;

        auto voltage = m_robot.sys_sensor().get_vsys();
        return Radio::FrSky::Telemetry::a3(0, voltage);
    }


    {
        m_rpm_count++;
        auto &encoder = m_robot.motors()[m_next_encoder].encoder();

        m_next_encoder++;
        if (m_next_encoder>=MOTOR_COUNT) {
            m_next_encoder = 0;
        }

        return Radio::FrSky::Telemetry::rpm(encoder.id(), encoder.rpm());
    }

    return Radio::FrSky::Telemetry::null();
}



#ifndef NDEBUG
void TelemetryProvider::print_stats()
{
    printf("Telemetry: count=%u  batt=%u  cpu=%u  vsys=%u  rpm=%u\n", m_count, m_battery_count, m_cpu_temp_count, m_vsys_count, m_rpm_count);
}
#endif
