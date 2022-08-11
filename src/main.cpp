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
#include "util/debug.h"
#include "util/i2c_bus.h"
#include "util/battery.h"
#include "led/single.h"
#include "led/strip.h"
#include "servo/servo.h"
#include "motor/dcmotor.h"
#include "oled/display.h"
#include "radio/frsky_receiver.h"
#include "sensors/bno0055.h"
#include "sensors/pico_adc.h"
#include "sensors/ina219.h"

#include "displayrender.h"

// Sensors
static Sensor::PicoADC picoADC { BATTERY_SENSE_PIN, BATTERY_SENSE_R1, BATTERY_SENSE_R2 };
static Sensor::INA219 currentSensor { Sensor::INA219::Address::INA0 };
static Sensor::BNO055 imu;

// Motors and servos
Servo servos[] = {
    { SERVO1_PIN },
    { SERVO2_PIN },
};
Motor::DCMotor motors[] = {
    { MOTOR1_IN1_PIN, MOTOR1_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR1_ENCA_PIN, MOTOR1_ENCB_PIN },
    { MOTOR2_IN1_PIN, MOTOR2_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR2_ENCA_PIN, MOTOR2_ENCB_PIN },
    { MOTOR3_IN1_PIN, MOTOR3_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR3_ENCA_PIN, MOTOR3_ENCB_PIN },
    { MOTOR4_IN1_PIN, MOTOR4_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR4_ENCA_PIN, MOTOR4_ENCB_PIN },
};

// LED/Displays
LED::Single led_builtin { PICO_DEFAULT_LED_PIN };
LED::Strip<LED_STRIP_PIXEL_COUNT> led_strip { LED_STRIP_PIO, LED_STRIP_PIN, LED_STRIP_IS_RGBW };
OLED::Display display { OLED_ADDRESS, OLED_TYPE };
DisplayRender display_render { display };

// Radio
Radio::FrSky::Receiver receiver { RADIO_RECEIVER_UART, RADIO_RECEIVER_BAUD_RATE, RADIO_RECEIVER_TX_PIN, RADIO_RECEIVER_RX_PIN };



#define WATCHDOG_INTERVAL 1000u // 1 second

//#define USE_WATCHDOG
#define USE_RADIO




static absolute_time_t main_update() 
{
    constexpr int64_t INTERVAL { 4000000ll };

    static absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(last_main, 0);

    if (absolute_time_diff_us(last_main, get_absolute_time())>INTERVAL) {
        static const uint n_states = 4;
        static uint state = 0;

        printf("Tick: %d\n", state);

        if (state==0) {
            led_strip.fill(LED::Color::WHITE);
            led_strip.show();
        }
        else {
            led_strip.fill(LED::Color::BLACK);
            led_strip.show();
        }

        #if 0
        static int height = 24;
        static int pos = 0;
        display.framebuffer().clear();
        display.framebuffer().draw_rect(32, pos, 64, height);
        display.update();

        pos++;
        if (pos>=64) pos = -height;
        #endif
        #if 0
        {
            //main_oled_update();
            display.framebuffer().fill_rect(0, 64-8, 64, 8, OLED::Framebuffer::DrawOp::SUBTRACT);
            char buf[64];
            sprintf(buf, "%.2fV", currentSensor.get_bus_voltage());
            display.framebuffer().draw_text(0, 64-8, buf, OLED::Resource::Font::Fixed_8x8);
            display.update();
        }
        #endif
        /*
        {
            char buf[8];
            sprintf(buf, "%u", state);
            display.framebuffer().fill_rect(128-32, 64-16, 32, 16, OLED::Framebuffer::DrawOp::SUBTRACT);
            display.framebuffer().draw_text(128-32, 64-16, buf, OLED::Resource::Font::Fixed_12x16);
            display.update();
        }
        */

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
        last_main = delayed_by_us(last_main, INTERVAL);
    }

    return delayed_by_us(last_main, INTERVAL);
}






/**
 * @brief Wait for a connection on CDC serial
 */
static void wait_cdc() 
{
#if LIB_PICO_STDIO_USB
    led_builtin.set_interval(100000u);
    auto blinking = led_builtin.blink();
    while (!stdio_usb_connected()) {
        sleep_until(led_builtin.update());
    }
    led_builtin.set_interval(LED::Single::DEFAULT_BLINK_INTERVAL);
    if (!blinking) led_builtin.on();
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

    led_builtin.init();
    led_builtin.on();

    debug_pin0(true);
    i2c_bus_init();

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
    receiver.init();
    #endif
    picoADC.init();
    debug_pin1(true);
    currentSensor.init();
    debug_pin2(true);
    imu.init();
    debug_pin3(true);
    display.init();
    display_render.init();

    // Register callbacks
    picoADC.set_battery_cb([](auto voltage){
        // Set motor supply voltage
        Motor::DCMotor::set_supply_voltage(voltage);
        
        #if 0
        // Push radio telemetry event
        radio_telemetry_cells(&event, 0x00, 0, 2, voltage/2.0f, voltage/2.0f);
        radio_receiver_telemetry_push(&event);
        #endif
    });
    picoADC.set_vsys_cb([](auto voltage){
        // Push radio telemetry event
        receiver.telemetry_push(Radio::FrSky::Telemetry::a3(0, voltage));
    });
    picoADC.set_temp_cb([](auto temp){
        // Push radio telemetry event
        receiver.telemetry_push(Radio::FrSky::Telemetry::temperature1(0, temp));
    });
    currentSensor.set_callback([](auto voltage, auto current, auto power){
        // Push radio telemetry event
        receiver.telemetry_push(Radio::FrSky::Telemetry::cells(0x00, 0, 2, voltage/2.0f, voltage/2.0f));
        receiver.telemetry_push(Radio::FrSky::Telemetry::current(0, current));
    });

}




/******************************************************************************
 * Core 1
 ******************************************************************************/
static semaphore_t core1_sem;
static volatile bool core1_running { true };

static void main_core1() 
{
    printf("Core 1 running\n");

    #ifdef USE_RADIO
    receiver.begin();
    #endif

    absolute_time_t wait;

    sem_acquire_blocking(&core1_sem);
    while (core1_running) {
        #ifdef USE_RADIO
        wait = receiver.update();
        #else
        wait = make_timeout_time_ms(10000);
        #endif
        
        wait = earliest_time(wait, picoADC.update());
        wait = earliest_time(wait, currentSensor.update());

        sleep_until(wait);
    }
    sem_release(&core1_sem);
}


static void core1_init()
{
    sem_init(&core1_sem, 1, 1);
}

static void core1_start()
{
    core1_running = true;
    multicore_launch_core1(main_core1);
}

static void core1_stop()
{
    core1_running = false;
    sem_acquire_blocking(&core1_sem);
}


/******************************************************************************
 * Core 0
 ******************************************************************************/

static void main_core0() 
{
    printf("Core 0 running\n");
    led_builtin.blink();
    display_render.begin();

    absolute_time_t wait;

    while (true) {
        wait = led_builtin.update();
        if (display.present()) {
            wait = earliest_time(wait, display_render.update_battery(currentSensor));
            wait = earliest_time(wait, display_render.update_radio(receiver));
            if (display.update_needed()) {
                display.update_block_until(wait);
            }
        }
        wait = earliest_time(wait, main_update());

        //printf("Sleep: %lld\n", absolute_time_diff_us(get_absolute_time(), wait));
        sleep_until(wait);

        #ifdef USE_WATCHDOG
        watchdog_update();
        #endif
    }
}




int main()
{
    stdio_init_all();
    debug_init();

    // Init basic systems
    init();

    // All subsystems running
    wait_cdc();
    print_banner();

    // Start core1
    printf("Booting core1\n");
    core1_init();
    core1_start();

    #ifdef USE_WATCHDOG
    printf("Enabling watchdog\n");
    watchdog_enable(WATCHDOG_INTERVAL, true);
    #endif

    // Enter main loop
    main_core0();

    // Shutdown
    core1_stop();
    display_render.off();
    led_strip.fill(LED::Color::BLACK);
    led_strip.show();
    led_builtin.off();

    while (true);
}
