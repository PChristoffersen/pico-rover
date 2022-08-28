#include "pico_cdc_transport.h"

#include <algorithm>
#include <pico/stdlib.h>
#include <tusb.h>

namespace ROS {

static constexpr uint64_t PICO_CDC_TRANSPORT_TIMEOUT_US { 500000 };



bool pico_cdc_transport_connected()
{
    return tud_cdc_n_connected(ROS_CDC);
}



bool pico_cdc_transport_open(struct uxrCustomTransport * transport)
{
    printf("ROS::CDC Transport open\n");


    return true;
}


bool pico_cdc_transport_close(struct uxrCustomTransport * transport)
{
    printf("ROS::CDC Transport close\n");

    return true;
}


size_t pico_cdc_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *errcode)
{
    static uint64_t last_avail_time = 0;

    if (!tud_cdc_n_connected(ROS_CDC)) {
        last_avail_time = 0;
        *errcode = 1;
        return 0;
    }

    //printf("ROS::Write: %u\n", len);
 
    auto avail = tud_cdc_n_write_available(ROS_CDC);
    auto n = std::min<size_t>(len, avail);

    if (n) {
        auto r = tud_cdc_n_write(ROS_CDC, buf, n);
        tud_task();
        tud_cdc_n_write_flush(ROS_CDC);
        last_avail_time = time_us_64();
        return r;
    }
    else {
        tud_task();
        tud_cdc_n_write_flush(ROS_CDC);
        if (!tud_cdc_n_connected(ROS_CDC) || (!tud_cdc_n_write_available(ROS_CDC) && time_us_64() > last_avail_time + PICO_CDC_TRANSPORT_TIMEOUT_US)) {
            // Give up
            return len;
        }
    }

    return 0;
}


size_t pico_cdc_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* errcode)
{
    uint64_t start_time_us = time_us_64();
    
    size_t res = 0;
    while (res < len) {
        int64_t elapsed_time_us = static_cast<int64_t>(timeout) * 1000ll - (time_us_64() - start_time_us);
        if (elapsed_time_us < 0) {
            *errcode = 1;
            return res;
        }
        if (!tud_cdc_n_connected(ROS_CDC)) {
            tud_task();
            sleep_us(100);
            continue;
        }

        auto avail = tud_cdc_n_available(ROS_CDC);
        auto n = std::min<size_t>(len-res, avail);
        auto r = tud_cdc_n_read(ROS_CDC, buf+res, n);
        res+= r;

        tud_task();
    }

    return len;
}


}
