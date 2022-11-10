#include "pico_cdc_transport.h"

#include <algorithm>
#include <pico/stdlib.h>
#include <tusb.h>
#include <rtos.h>
#include <usb_device.h>

namespace ROS {

static constexpr uint64_t PICO_CDC_TRANSPORT_TIMEOUT_US { 500000 };



bool pico_cdc_transport_connected()
{
    return tud_cdc_n_connected(ROS_CDC);
}



bool pico_cdc_transport_open(struct uxrCustomTransport * transport)
{
    printf("ROS::CDC Transport open\n");

    usb_cdc_set_buffer(ROS_CDC, reinterpret_cast<StreamBufferHandle_t>(transport->args));

    return true;
}


bool pico_cdc_transport_close(struct uxrCustomTransport * transport)
{
    printf("ROS::CDC Transport close\n");
    
    usb_cdc_set_buffer(ROS_CDC, nullptr);

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

    printf("ROS::Write: %u\n", len);
 
    auto avail = tud_cdc_n_write_available(ROS_CDC);
    auto n = std::min<size_t>(len, avail);

    if (n) {
        auto r = tud_cdc_n_write(ROS_CDC, buf, n);
        tud_cdc_n_write_flush(ROS_CDC);
        last_avail_time = time_us_64();
        return r;
    }
    else {
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
    StreamBufferHandle_t buffer = reinterpret_cast<StreamBufferHandle_t>(transport->args);

    xStreamBufferSetTriggerLevel(buffer, len);

    auto res = xStreamBufferReceive(buffer, buf, len, pdMS_TO_TICKS(timeout));
    printf("ROS Read: %u\n", res);
    return res;
}


}
