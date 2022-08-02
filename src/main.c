#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/unique_id.h>
#include <pico/multicore.h>
#include <hardware/clocks.h>
#include <hardware/watchdog.h>
#include <hardware/i2c.h>


#if LIB_PICO_STDIO_USB
#include <tusb.h>
#endif

#include "boardconfig.h"
#include "debug/debug.h"
#include "led/led.h"
#include "led/ledstrip.h"
#include "servo/servo.h"
#include "motor/motor.h"
#include "oled/oled.h"
#include "radio/receiver.h"
#include "sensors/board_adc.h"


#define WATCHDOG_INTERVAL 1000u // 1 second

//#define USE_WATCHDOG
//#define USE_RADIO



/**
 * @brief Wait for a connection on CDC serial
 */
static void wait_cdc() 
{
#if LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        printf(".");
        led_on();
        sleep_ms(100);
        led_off();
        sleep_ms(100);
    }
#endif
}


static void print_banner()
{
    printf("--------------------------------------------\n");
    printf("        Project: %s\n", PICO_PROGRAM_NAME);
    printf("        Version: %s\n", PICO_PROGRAM_VERSION_STRING);
    printf("          Build: %s\n", PICO_CMAKE_BUILD_TYPE);
    printf("           Date: %s\n", __DATE__);
    printf("  CPU Frequency: %.3f MHz\n", clock_get_hz(clk_sys)/1000000.0f);

    char board_id[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1] = { '\0', };
    pico_get_unique_board_id_string(board_id, sizeof(board_id));
    printf("       Board-id: %s\n", board_id);

    printf("           Boot: ");
    if (watchdog_caused_reboot()) {
        printf("Rebooted by Watchdog!\n");
    }
    else {
        printf("Clean boot\n");
    }

    printf("--------------------------------------------\n");

    printf("\n");
}


static void board_i2c_init()
{
    i2c_init(i2c_default, BOARD_I2C_SPEED);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
}



/**
 * @brief Main function for core1
 * 
 * Use core1 to run the radio receiver and board health monitor
 */
static void main_core1() 
{
    printf("Core 1 running\n");
    radio_receiver_begin();
    while (true) {
        if (!radio_receiver_update()) {
            board_adc_update();
            sleep_us(100);
        }
    }
}



// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

static void bus_scan()
{
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}



static void main_init() {
    motor_set_drivers_enabled(true);
    //servo_set_enabled(SERVO_1, true);
    //servo_set_enabled(SERVO_2, true);
    motor_set_enabled(MOTOR_1, true);
    motor_set_enabled(MOTOR_2, true);
    motor_set_enabled(MOTOR_3, true);
    motor_set_enabled(MOTOR_4, true);
}

static void main_update() 
{
    static absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(last_main, 0);
    absolute_time_t now = get_absolute_time();

    if (absolute_time_diff_us(last_main, now)>4000000u) {
        static const uint n_states = 4;
        static uint state = 0;


        motor_encoder_fetch_request(MOTOR_1);
        motor_encoder_fetch_request(MOTOR_2);
        motor_encoder_fetch_request(MOTOR_3);
        motor_encoder_fetch_request(MOTOR_4);

        int32_t enc1 = motor_encoder_fetch(MOTOR_1);
        int32_t enc2 = motor_encoder_fetch(MOTOR_2);
        int32_t enc3 = motor_encoder_fetch(MOTOR_3);
        int32_t enc4 = motor_encoder_fetch(MOTOR_4);
        printf("Encoder: %8d  %8d  %8d  %8d\n", enc1, enc2, enc3, enc4);

        //gpio_put(GENERIC_INTR_PIN, state&1);

        //bus_scan();
        #if 1
        
        #if 1
        switch (state) {
            case 0: 
                {
                    motor_put(MOTOR_1, 0.0f);
                    motor_put(MOTOR_2, 0.0f);
                    motor_put(MOTOR_3, 0.0f);
                    motor_put(MOTOR_4, 0.0f);
                }
                break;
            case 1: 
                {
                    motor_put(MOTOR_1, 0.5f);
                    motor_put(MOTOR_2, 0.5f);
                    motor_put(MOTOR_3, 0.5f);
                    motor_put(MOTOR_4, 0.5f);
                }
                break;
            case 2: 
                {
                    motor_put(MOTOR_1, 1.0f);
                    motor_put(MOTOR_2, 1.0f);
                    motor_put(MOTOR_3, 1.0f);
                    motor_put(MOTOR_4, 1.0f);
                }
                break;
            case 3: 
                {
                    motor_put(MOTOR_1, 0.5f);
                    motor_put(MOTOR_2, 0.5f);
                    motor_put(MOTOR_3, 0.5f);
                    motor_put(MOTOR_4, 0.5f);
                }
                break;
        }
        #else
        //motor_set_enabled(motor, true);
        //motor_set_duty(motor, MOTOR_PWM_MAX);
        #endif
        #endif
        state++;
        if (state>=n_states) state = 0;
        last_main = now;
    }
}




int main()
{
    stdio_init_all();
    debug_init();

    // Init all subsystems
    board_i2c_init();
    led_init();
    servo_init();
    motor_init();
    #ifdef USE_RADIO
    radio_receiver_init();
    stats_adc_init();
    #endif
    led_strip_init();
    oled_init();

    // All subsystems running
    wait_cdc();
    print_banner();

    // Start core1
    #ifdef USE_RADIO
    printf("Booting core1\n");
    multicore_launch_core1(main_core1);
    #endif


    #ifdef USE_WATCHDOG
    printf("Enabling watchdog\n");
    watchdog_enable(WATCHDOG_INTERVAL, true);
    #endif

    //gpio_init(GENERIC_INTR_PIN);
    //gpio_set_dir(GENERIC_INTR_PIN, GPIO_OUT);

    led_blink();
    printf("Entering main loop\n");

    absolute_time_t now = get_absolute_time();
    absolute_time_t last = now;

    main_init();
    while (true) {
        main_update();

        sleep_ms(1);
        led_update();
        #ifdef USE_WATCHDOG
        watchdog_update();
        #endif
    }
}
