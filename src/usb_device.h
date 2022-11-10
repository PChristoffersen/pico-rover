#pragma once

#include <rtos.h>

#ifdef __cplusplus
extern "C"
{
#endif

void usb_device_init();

void usb_cdc_set_buffer(uint8_t itf, StreamBufferHandle_t handle);


#ifdef __cplusplus
} // extern "C"
#endif
