#pragma once

#include <pico/stdlib.h>

#ifdef RASPBERRYPI_PICO_W

#ifdef __cplusplus
extern "C"
{
#endif

void wifi_init();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
