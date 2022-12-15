#include "wifi.h"

#ifdef RASPBERRYPI_PICO_W

#include <rtos.h>
#include <pico/cyw43_arch.h>

#define WIFI_SSID "somewhere"
#define WIFI_PASSWORD "somehow"

void wifi_init()
{
    if (cyw43_arch_init()) {
        printf("failed to initialise WiFi\n");
        return;
    }
    #if 0
    cyw43_arch_enable_sta_mode();
    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        exit(1);
    } else {
        printf("Connected.\n");
    }
    #endif
}


#endif
