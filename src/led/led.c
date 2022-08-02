#include "led.h"

#include <stdio.h>
#include <pico/stdlib.h>

#include "../boardconfig.h"


static bool g_blinking = false;
static absolute_time_t g_blink_last;


void led_init()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
}


void led_on()
{
    gpio_put(LED_PIN, 1);
    g_blinking = false;
}


void led_off()
{
    gpio_put(LED_PIN, 0);
    g_blinking = false;
}


void led_blink()
{
    g_blink_last = get_absolute_time();
    g_blinking = true;
}


void led_update()
{
    if (!g_blinking)
        return;

    static bool state = false;
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(g_blink_last, now)>LED_BLINK_INTERVAL) {
        state = !state;
        if (state) {
            gpio_put(LED_PIN, 1);
        }
        else {
            gpio_put(LED_PIN, 0);
        }
        g_blink_last = now;
    }
}
