#include "usb_device.h"
#include <tusb.h>
#include <pico/unique_id.h>
#include <rtos.h>



//--------------------------------------------------------------------+
// Device task
//--------------------------------------------------------------------+

static TaskHandle_t g_usb_task = NULL;
static StaticTask_t g_usb_task_buf;
static StackType_t  g_usb_task_stack[configMINIMAL_STACK_SIZE];

static void usb_device_task(__unused void *param)
{
    tusb_init();
    #if FREE_RTOS_KERNEL_SMP && configNUM_CORES > 1
    vTaskCoreAffinitySet(NULL, 0b11);
    #endif

    while (true) {
        tud_task();
    }
}



void usb_device_init()
{
    g_usb_task = xTaskCreateStatic(usb_device_task, "usbd", configMINIMAL_STACK_SIZE, NULL, USBD_TASK_PRIORITY, g_usb_task_stack, &g_usb_task_buf);
    #if FREE_RTOS_KERNEL_SMP && configNUM_CORES > 1
    vTaskCoreAffinitySet(g_usb_task, 0b01);
    #endif
}


//--------------------------------------------------------------------+
// CDC Device callbacks
//--------------------------------------------------------------------+

static StreamBufferHandle_t g_cdc_rx[CFG_TUD_CDC] = { NULL };

void usb_cdc_set_buffer(uint8_t itf, StreamBufferHandle_t handle)
{
    g_cdc_rx[itf] = handle;
}


// Invoked when received new data
void tud_cdc_rx_cb(uint8_t itf)
{
    printf("CDC RX: %u\n", itf);
    if (g_cdc_rx[itf]) {
        while (tud_cdc_n_available(itf)) {
            uint8_t buf[64];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            printf("CDC RC %u  sz=%lu\n", itf, count);
            xStreamBufferSend(g_cdc_rx[itf], buf, count, portMAX_DELAY);
        }
    }
}

// Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    printf("CDC Line state: %u  dtr=%d  rts=%d\n", itf, dtr, rts);
}
