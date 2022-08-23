#include "pico_cdc_transport.h"

#include <algorithm>
#include <pico/stdlib.h>
#include <tusb.h>

#include <util/usb_bus.h>



bool pico_cdc_transport_open(struct uxrCustomTransport * transport)
{
    return true;
}


bool pico_cdc_transport_close(struct uxrCustomTransport * transport)
{
    return true;
}


size_t pico_cdc_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err)
{
    if (!tud_cdc_n_connected(ROS_CDC))
        return 0;

    auto avail = tud_cdc_write_available();
    len = std::min<size_t>(len, avail);

    auto res = tud_cdc_n_write(ROS_CDC, buf, len);
    tud_task();
    tud_cdc_n_write_flush(ROS_CDC);

    return res;
}


size_t pico_cdc_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err)
{
    if (!tud_cdc_n_connected(ROS_CDC))
        return 0;

    absolute_time_t start = get_absolute_time();
    size_t res = 0;
    while (len) {
        if (timeout>0 && absolute_time_diff_us(start, get_absolute_time())> timeout*1000) {
            break;
        }
        size_t rd = std::min<size_t>(len, tud_cdc_n_available(ROS_CDC));
        if (rd==0) {
            sleep_us(100);
            continue;
        }
        else {
            auto r = tud_cdc_n_read(ROS_CDC, buf, rd);
            len-=r;
            buf+=r;
            res+=r;
        }
    }

    return res;
}

