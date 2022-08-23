#include "usb_bus.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/irq.h>
#include <tusb.h>

#ifndef PICO_USB_TASK_INTERVAL_US
#define PICO_USB_TASK_INTERVAL_US 1000
#endif

#ifndef NDEBUG
static uint8_t usb_core_num;
#endif

mutex_t usb_mutex;

#if 0

#ifdef PICO_STDIO_USB_LOW_PRIORITY_IRQ
static_assert(PICO_STDIO_USB_LOW_PRIORITY_IRQ >= NUM_IRQS - NUM_USER_IRQS, "");
#define low_priority_irq_num PICO_STDIO_USB_LOW_PRIORITY_IRQ
#else
static uint8_t low_priority_irq_num;
#endif

static void low_priority_worker_irq(void) {
    // if the mutex is already owned, then we are in user code
    // in this file which will do a tud_task itself, so we'll just do nothing
    // until the next tick; we won't starve
    tud_int_handler();
    printf("USB>\n");
    if (mutex_try_enter(&usb_mutex, NULL)) {
        tud_task();
        mutex_exit(&usb_mutex);
    }
}

static void usb_irq(void) {
    irq_set_pending(low_priority_irq_num);
}

static int64_t timer_task(__unused alarm_id_t id, __unused void *user_data) {
    assert(usb_core_num == get_core_num()); // if this fails, you have initialized stdio_usb on the wrong core
    irq_set_pending(low_priority_irq_num);
    return PICO_USB_TASK_INTERVAL_US;
}
#endif


void usb_bus_init()
{
    #ifndef NDEBUG
    usb_core_num = (uint8_t)get_core_num();
    #endif

    mutex_init(&usb_mutex);

    tusb_init();

#if 0
    #ifdef PICO_STDIO_USB_LOW_PRIORITY_IRQ
    user_irq_claim(PICO_STDIO_USB_LOW_PRIORITY_IRQ);
    #else
    low_priority_irq_num = (uint8_t) user_irq_claim_unused(true);
    #endif
    irq_set_exclusive_handler(low_priority_irq_num, low_priority_worker_irq);
    irq_set_enabled(low_priority_irq_num, true);

    if (irq_has_shared_handler(USBCTRL_IRQ)) {
        // we can use a shared handler to notice when there may be work to do
        irq_add_shared_handler(USBCTRL_IRQ, usb_irq, PICO_SHARED_IRQ_HANDLER_LOWEST_ORDER_PRIORITY);
    } else {
        add_alarm_in_us(PICO_USB_TASK_INTERVAL_US, timer_task, NULL, true);
    }
#endif

}



absolute_time_t usb_bus_update()
{
    tud_task();
    return make_timeout_time_us(1000);
}
