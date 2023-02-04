#include <array>
#include <stdio.h>
#include <math.h>
#include <pico/stdlib.h>
#include <pico/unique_id.h>
#include <hardware/watchdog.h>
#include <hardware/clocks.h>
#include <tusb.h>
#include <rtos.h>

#include <i2c_bus.h>

#include "boardconfig.h"
#include "usb_device.h"
#include "wifi.h"
#include "robot.h"
#include "watchdog/watchdog.h"
#include "util/debug.h"
#include "util/battery.h"

#include "ros/client.h"


static Robot robot;
static Watchdog::Watchdog watchdog;
static ROS::Client ros;

#define WATCHDOG_INTERVAL 1000u // 1 second

//#define USE_WATCHDOG
#define USE_RADIO

#if 1
static void drive_wheels(float stick_x, float stick_y, float rotate) {
    stick_x = -stick_x;

    float duty_fl, duty_fr, duty_rl, duty_rr;

    auto angle = atan2f(stick_x, stick_y);
    auto magnitude = sqrtf(stick_x*stick_x+stick_y*stick_y);

    // Calculate translation
    duty_fl = -sinf(angle-M_PI_4) * magnitude;
    duty_fr =  sinf(angle+M_PI_4) * magnitude;
    duty_rl = duty_fr;
    duty_rr = duty_fl;

    // Add rotation
    duty_fl += rotate;
    duty_fr -= rotate;
    duty_rl += rotate;
    duty_rr -= rotate;

    // Scale down if any value is above 1.0
    auto scale = std::max(std::max(std::abs(duty_fl), std::abs(duty_fr)), std::max(std::abs(duty_fl), std::abs(duty_fr)));
    if (scale>1.0f) {
        duty_fl /= scale;
        duty_fr /= scale;
        duty_rl /= scale;
        duty_rr /= scale;
    }

    #if 0
    static absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(last, 0);
    if (absolute_time_diff_us(last, get_absolute_time())>200000) {
        printf("Drive:  %.2f,%.2f :  %.2f  %.2f  %.2f  %.2f\n", stick_x, stick_y, duty_fl, duty_fr, duty_rl, duty_rr);
        last = delayed_by_us(get_absolute_time(), 200000);
    }
    #endif
    auto &motors = robot.motors();
    motors[Motor::DCMotor::FRONT_LEFT].set_duty(duty_fl);
    motors[Motor::DCMotor::FRONT_RIGHT].set_duty(duty_fr);
    motors[Motor::DCMotor::REAR_LEFT].set_duty(duty_rl);
    motors[Motor::DCMotor::REAR_RIGHT].set_duty(duty_rr);
}
#endif


static void main_task(__unused void *params)
{
    constexpr uint INTERVAL { 1000 };
    constexpr uint n_states = 4;
    uint state = 0;

    robot.start();

    TickType_t last_time = xTaskGetTickCount();
    while (true) {
        printf("Tick: %d\n", state);

        switch (state) {
            case 0: 
                {
                    #ifndef NDEBUG
                    debug_print_task_state();
                    #endif
                }
                break;
            case 1: 
                {
                    //servos[0].put(500);
                }
                break;
            case 2: 
                {
                    //robot.receiver().print_stats();
                    //robot.telemetry_provider().print_stats();
                }
                break;
            case 3: 
                {
                    //robot.imu().print();
                    //servos[0].put(2500);
                }
                break;
        }


        state++;
        if (state>=n_states) state = 0;

        xTaskDelayUntil(&last_time, pdMS_TO_TICKS(INTERVAL));
    }
}





/**
 * @brief Wait for a connection on CDC serial
 */
static void wait_cdc() 
{
#if LIB_PICO_STDIO_USB && false
    robot.led_builtin().set_interval(100000u);
    auto blinking = robot.led_builtin().blink();
    while (!stdio_usb_connected()) {
        sleep_until(robot.led_builtin().update());
    }
    robot.led_builtin().set_interval(LED::Single::DEFAULT_BLINK_INTERVAL);
    if (!blinking) robot.led_builtin().on();
#endif
}


static void print_banner()
{
    printf("--------------------------------------------\n");
    printf("        Project: %s\n", PICO_PROGRAM_NAME);
    printf("        Version: %s\n", PICO_PROGRAM_VERSION_STRING);
    printf("          Build: %s\n", PICO_CMAKE_BUILD_TYPE);
    printf("           Date: %s\n", __DATE__);

    const char *rtos_name;
    #if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
    #else
    rtos_name = "FreeRTOS";
    #endif
    printf("           RTOS: %s\n", rtos_name);

    printf("  CPU Frequency: %.3f MHz\n", clock_get_hz(clk_sys)/1000000.0f);

    char board_id[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1] = { '\0', };
    pico_get_unique_board_id_string(board_id, sizeof(board_id));
    printf("       Board-id: %s\n", board_id);

    printf("           Boot: ");
    if (watchdog.caused_reboot()) {
        printf("Rebooted by Watchdog!\n");
    }
    else {
        printf("Clean boot\n");
    }
    if (robot.imu().present()) {
        printf("            IMU: Present (sw: %04x)\n", robot.imu().sw_rev());
    }
    else {
        printf("            IMU: Not found\n");
    }
    printf("        Display: %s\n", robot.oled().display().present()?"Present":"Not found");

    printf("--------------------------------------------\n");

    printf("\n");
}




static void init() 
{
    static bool stabilize_camera = false;


    #if 1
    // Register callbacks
    robot.receiver().add_callback([](auto &receiver, auto &channels, auto &mapping){
        if (!channels.sync() || channels.flags().frameLost()) {
            robot.set_armed(false);
            return;
        }

        robot.set_armed(mapping.sf());
        if (robot.is_armed()) {
            auto &servos = robot.servos();
            //servos[0].put(mapping.right_y().asServoPulse());
            //servos[1].put(mapping.right_x().asServoPulse());
            //servos[0].put((-mapping.s1()).asServoPulse());
            servos[1].put((-mapping.s1()).asServoPulse());
            drive_wheels(mapping.right_x().asFloat(), mapping.right_y().asFloat(), mapping.left_x().asFloat());
        }

        // LEDS
        auto &leds = robot.leds();
        switch (mapping.sc()) {
            case Radio::Toggle::P0:
                leds.set_animation_mode(LED::Control::AnimationMode::BLACK);
                break;
            case Radio::Toggle::P1:
                leds.set_animation_mode(LED::Control::AnimationMode::KNIGHT_RIDER);
                break;
            case Radio::Toggle::P2:
                leds.set_animation_mode(LED::Control::AnimationMode::COLOR_CYCLE);
                break;
        }
        switch (mapping.sd()) {
            case Radio::Toggle::P0:
            case Radio::Toggle::P2:
                leds.set_light_mode(LED::Control::LightMode::OFF);
                break;
            case Radio::Toggle::P1:
                leds.set_light_mode(LED::Control::LightMode::ON);
                break;
        }
        if (mapping.sd()==Radio::Toggle::P2) {
            leds.set_indicator_mode(LED::Control::IndicatorMode::HAZARD);
        }
        else {
            switch (mapping.sg()) {
                case Radio::Toggle::P0:
                    leds.set_indicator_mode(LED::Control::IndicatorMode::LEFT);
                    break;
                case Radio::Toggle::P1:
                    leds.set_indicator_mode(LED::Control::IndicatorMode::NONE);
                    break;
                case Radio::Toggle::P2:
                    leds.set_indicator_mode(LED::Control::IndicatorMode::RIGHT);
                    break;
            }
        }
        leds.set_brightness(mapping.s2().asPercent());

        // Camera

        switch (mapping.sa()) {
            case Radio::Toggle::P0:
                stabilize_camera = false;
                robot.camera().put(CAMERA_LEVEL_PULSE);
                break;
            case Radio::Toggle::P1:
                stabilize_camera = false;
                robot.camera().put(mapping.slider_l().asServoPulse());
                break;
            case Radio::Toggle::P2:
                stabilize_camera = true;
                break;
        }
    });
    #endif

    #if 1
    robot.imu().add_callback([](auto &imu, auto tick_delta){
        //printf("IMU: %f %f %f\n", heading, pitch, roll);
        if (stabilize_camera) {
            auto pitch = imu.pitch();
            Motor::Servo::value_t pulse = std::clamp<Motor::Servo::value_t>(CAMERA_LEVEL_PULSE+(CAMERA_LEVEL_PULSE-CAMERA_UP_PULSE)*pitch/M_PI_2, Motor::Servo::PULSE_MIN, Motor::Servo::PULSE_MAX);
            robot.camera().put(pulse);
            //printf("IMU: %.2f  %.2f  %u    %d %d %d\n", pitch, pitch/M_PI_2, pulse, (int)imu.mag_calib(), (int)imu.accel_calib(), (int)imu.gyro_calib());
        }
    });
    #endif

    #if 0
    robot.receiver()_listener.add_callback([](auto &channels, auto &mapping){
        static absolute_time_t last = get_absolute_time();
        if (absolute_time_diff_us(last, get_absolute_time())>200000) {
            last = get_absolute_time();
            #if 0
            //printf("Data: fl=%d fs=%d rssi=%3d ch=%d  ", channels.flags().frameLost(), channels.flags().failsafe(), channels.rssi(), channels.count());
            printf("Raw: ");
            for (auto ch : channels) {
                printf("%4d ", ch.raw());
            }
            printf("\n");
            #endif
            #if 1
            //printf("%d %d ", channels.flags().frameLost(), channels.flags().failsafe());
            mapping.print();
            #endif
            //printf("Servo: %4d %4d\n", mapping.s1().asServoPulse(), mapping.s2().asServoPulse());
        }
    });
    #endif

}



//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

static void vLaunch() 
{
    xTaskCreate(main_task, "Main", configMINIMAL_STACK_SIZE, nullptr, TEST_TASK_PRIORITY, nullptr);

    /* Start the tasks and timer running. */
    printf("Starting scheduler\n");
    vTaskStartScheduler();
}


int main()
{
    stdio_init_all();
    debug_init();
    i2c_bus_init(BOARD_I2C_SPEED);
    //usb_device_init();

    // Init basic systems
    //watchdog.init();
    robot.init();
    ros.init();
    init();

    #ifdef RASPBERRYPI_PICO_W
    wifi_init();
    #endif

    // All subsystems running
    wait_cdc();
    print_banner();

    // Launch RTOS
    vLaunch();

}
