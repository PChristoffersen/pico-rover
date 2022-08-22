#include "watchdog.h"

#include <stdio.h>
#include <hardware/watchdog.h>

#include <util/debug.h>

namespace Watchdog {

Watchdog::Watchdog()
{
    ABSOLUTE_TIME_INITIALIZED_VAR(m_last_core1_ping, 0);
    hw_write_masked(&watchdog_hw->ctrl, 0, WATCHDOG_CTRL_ENABLE_BITS);
}


bool Watchdog::caused_reboot() const 
{
    return watchdog_caused_reboot();
}


void Watchdog::init()
{
}


void Watchdog::term()
{
    // TODO how to stop watchdog
}


void Watchdog::begin()
{
    if constexpr (!debug_build) {
        watchdog_enable(INTERVAL, true);
    }
    watchdog_update();
    m_last_core1_ping = get_absolute_time();
}

absolute_time_t Watchdog::ping_core0()
{
    if (absolute_time_diff_us(m_last_core1_ping, get_absolute_time())<INTERVAL*1000) {
        // Only update watchdog if core1 has pinged us recently
        watchdog_update();
    }
    return make_timeout_time_ms(PING_INTERVAL);
}


absolute_time_t Watchdog::ping_core1()
{
    absolute_time_t now = get_absolute_time();
    m_last_core1_ping = now;
    return delayed_by_ms(now, PING_INTERVAL);
}


}
