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
#include "util/time.h"
#include "i2c_bus.h"
#include "util/debug.h"
#include "led/single.h"
#include "led/strip.h"
#include "servo/servo.h"
#include "motor/motor.h"
#include "oled/oled.h"
#include "radio/frsky_receiver.h"
#include "sensors/bno0055.h"
#include "sensors/pico_adc.h"
#include "sensors/ina219.h"



// Sensors
static PicoADC picoADC { BATTERY_SENSE_PIN, BATTERY_SENSE_R1, BATTERY_SENSE_R2 };
static INA219 currentSensor { INA219::Address::INA0 };
static BNO055 imu;

// Motors and servos
Servo servos[] = {
    { SERVO1_PIN },
    { SERVO2_PIN },
};
Motor motors[] = {
    { MOTOR1_IN1_PIN, MOTOR1_IN2_PIN, MOTOR1_ENCA_PIN, MOTOR1_ENCB_PIN },
    { MOTOR2_IN1_PIN, MOTOR2_IN2_PIN, MOTOR2_ENCA_PIN, MOTOR2_ENCB_PIN },
    { MOTOR3_IN1_PIN, MOTOR3_IN2_PIN, MOTOR3_ENCA_PIN, MOTOR3_ENCB_PIN },
    { MOTOR4_IN1_PIN, MOTOR4_IN2_PIN, MOTOR4_ENCA_PIN, MOTOR4_ENCB_PIN },
};

// LED/Displays
LED::Single builtinLED { PICO_DEFAULT_LED_PIN };
LED::Strip<LED_STRIP_PIXEL_COUNT> led_strip { LED_STRIP_PIO, LED_STRIP_PIN, LED_STRIP_IS_RGBW };
OLED::Display display { OLED_ADDRESS, OLED_TYPE };

// Radio
Radio::FrSky::Receiver receiver { RADIO_RECEIVER_UART, RADIO_RECEIVER_BAUD_RATE, RADIO_RECEIVER_TX_PIN, RADIO_RECEIVER_RX_PIN };



#define WATCHDOG_INTERVAL 1000u // 1 second

//#define USE_WATCHDOG
//#define USE_RADIO




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
    printf("  CPU Frequency: %.3f MHz\n", clock_get_hz(clk_sys)/1000000.0f);
    if (imu.present()) {
        printf("            IMU: Present (sw: %04x)\n", imu.sw_rev());
    }
    else {
        printf("            IMU: Not found\n");
    }
    printf("        Display: %s\n", display.present()?"Present":"Not found");

    printf("--------------------------------------------\n");

    printf("\n");
}




static void init() 
{

    builtinLED.init();
    i2c_bus_init();
    bus_scan();

    #if 0
    for (uint i=0; i<count_of(servos); ++i) {
        servos->init();
    }
    #endif
    #if 0
    for (uint i=0; i<count_of(motors); ++i) {
        motors->init();
    }
    #endif
    led_strip.init();
    #ifdef USE_RADIO
    radio_receiver_init();
    #endif
    picoADC.init();
    currentSensor.init();
    imu.init();
    display.init();

    // Register callbacks
    picoADC.set_battery_cb([](auto voltage){
        // Set motor supply voltage
        Motor::set_supply_voltage(voltage);
        
        #if 0
        // Push radio telemetry event
        radio_telemetry_cells(&event, 0x00, 0, 2, voltage/2.0f, voltage/2.0f);
        radio_receiver_telemetry_push(&event);
        #endif
    });
    picoADC.set_vsys_cb([](auto voltage){
        // Push radio telemetry event
        Radio::FrSky::radio_telemetry_t event;
        Radio::FrSky::radio_telemetry_a3(&event, 0, voltage);
        receiver.telemetry_push(event);
    });
    picoADC.set_temp_cb([](auto temp){
        // Push radio telemetry event
        Radio::FrSky::radio_telemetry_t event;
        Radio::FrSky::radio_telemetry_temperature1(&event, 0, temp);
        receiver.telemetry_push(event);
    });
    currentSensor.set_callback([](auto voltage, auto current, auto power){
        Radio::FrSky::radio_telemetry_t event;

        // Push radio telemetry event
        Radio::FrSky::radio_telemetry_cells(&event, 0x00, 0, 2, voltage/2.0f, voltage/2.0f);
        receiver.telemetry_push(event);

        Radio::FrSky::radio_telemetry_current(&event, 0, current);
        receiver.telemetry_push(event);
    });

}




/******************************************************************************
 * Core 1
 ******************************************************************************/

static void main_core1() 
{
    printf("Core 1 running\n");
    #ifdef USE_RADIO
    radio_receiver_begin();
    #endif

    absolute_time_t wait;
    absolute_time_t w_;

    while (true) {
        #ifdef USE_RADIO
        wait = radio_receiver_update();
        #else
        wait = make_timeout_time_ms(1000);
        #endif
        
        w_ = picoADC.update();
        wait = earliest_time(wait, w_);
        
        w_ = currentSensor.update();
        wait = earliest_time(wait, w_);

        sleep_until(wait);
    }
}




/******************************************************************************
 * Core 0
 ******************************************************************************/

static void main_init() {

    sleep_ms(1000);
    display.frame().clear();
    display.frame().draw_rect(32, 34, 64, 4);
    display.update();



    #if 0
    motor_set_drivers_enabled(true);
    //servo_set_enabled(SERVO_1, true);
    //servo_set_enabled(SERVO_2, true);
    motor_set_enabled(MOTOR_1, true);
    motor_set_enabled(MOTOR_2, true);
    motor_set_enabled(MOTOR_3, true);
    motor_set_enabled(MOTOR_4, true);
    #endif
}

static void main_update() 
{
    static absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(last_main, 0);
    absolute_time_t now = get_absolute_time();

    //currentSensor.update();

    if (absolute_time_diff_us(last_main, now)>100000ll) {
        static const uint n_states = 4;
        static uint state = 0;

        //printf("Tick: %d\n", state);
        #if 1
        static int height = 24;
        static int pos = 0;
        display.frame().clear();
        display.frame().draw_rect(32, pos, 64, height);
        display.update();

        pos++;
        if (pos>=64) pos = -height;
        #endif

        #if 0
        motor_encoder_fetch_request(MOTOR_1);
        motor_encoder_fetch_request(MOTOR_2);
        motor_encoder_fetch_request(MOTOR_3);
        motor_encoder_fetch_request(MOTOR_4);

        int32_t enc1 = motor_encoder_fetch(MOTOR_1);
        int32_t enc2 = motor_encoder_fetch(MOTOR_2);
        int32_t enc3 = motor_encoder_fetch(MOTOR_3);
        int32_t enc4 = motor_encoder_fetch(MOTOR_4);
        printf("Encoder: %8d  %8d  %8d  %8d\n", enc1, enc2, enc3, enc4);
        #endif

        //gpio_put(GENERIC_INTR_PIN, state&1);

        //bus_scan();

        #if 0
        
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
                    motor_put(MOTOR_1, 0.0f);
                    motor_put(MOTOR_2, 0.0f);
                    motor_put(MOTOR_3, 0.0f);
                    motor_put(MOTOR_4, 0.0f);
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
                    motor_put(MOTOR_1, 1.0f);
                    motor_put(MOTOR_2, 1.0f);
                    motor_put(MOTOR_3, 1.0f);
                    motor_put(MOTOR_4, 1.0f);
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
    printf("Init hardware\n");

    // Init basic systems
    init();

    // All subsystems running
    wait_cdc();
    print_banner();


    // Start core1
    printf("Booting core1\n");
    //multicore_launch_core1(main_core1);


    #ifdef USE_WATCHDOG
    printf("Enabling watchdog\n");
    watchdog_enable(WATCHDOG_INTERVAL, true);
    #endif

    builtinLED.blink();
    printf("Entering main loop\n");

    main_init();
    while (true) {
        main_update();

        sleep_ms(1);
        builtinLED.update();
        #ifdef USE_WATCHDOG
        watchdog_update();
        #endif
    }
}
