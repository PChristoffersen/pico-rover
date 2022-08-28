#include "usb_bus.h"

#include <tusb.h>


void usb_bus_init()
{
    tusb_init();
}


absolute_time_t usb_bus_update()
{
    tud_task();
    return make_timeout_time_ms(1);
}
