#include "robot.h"

#include <boardconfig.h>


Robot::Robot():
    // Sensors
    m_sys_sensor { BATTERY_SENSE_PIN, BATTERY_SENSE_R1, BATTERY_SENSE_R2 },
    m_battery_sensor { Sensor::INA219::Address::INA0 },
    m_imu { Sensor::BNO055::Address::PRIMARY },

    // Motors and servms
    m_servos {
        Motor::Servo { 0, SERVO1_PIN },
        Motor::Servo { 1, SERVO2_PIN },
    },
    m_motors {
        Motor::DCMotor { 0, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR2_ENCA_PIN, MOTOR2_ENCB_PIN, true  }, // Front left
        Motor::DCMotor { 1, MOTOR3_IN1_PIN, MOTOR3_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR3_ENCA_PIN, MOTOR3_ENCB_PIN, false }, // Front right
        Motor::DCMotor { 2, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR1_ENCA_PIN, MOTOR1_ENCB_PIN, true  }, // Rear left
        Motor::DCMotor { 3, MOTOR4_IN1_PIN, MOTOR4_IN2_PIN, MOTOR_ENCODER_PIO, MOTOR4_ENCA_PIN, MOTOR4_ENCB_PIN, false }, // Rear right
    },

    // Radio
    m_receiver { RADIO_RECEIVER_UART, RADIO_RECEIVER_BAUD_RATE, RADIO_RECEIVER_TX_PIN, RADIO_RECEIVER_RX_PIN },
    m_receiver_listener {},

    // LED/Displays
    m_led_builtin { PICO_DEFAULT_LED_PIN },
    m_led_strip { LED_STRIP_PIO, LED_STRIP_PIN, LED_STRIP_IS_RGBW },
    m_display { OLED_ADDRESS, OLED_TYPE },

    // Higher level objects
    m_telemetry_provider { *this },
    m_display_render { m_display, *this },
    m_led_render { m_led_strip, *this }

{

}





void Robot::init()
{
    m_led_builtin.init();
    m_led_builtin.on();

    for (auto &servo : m_servos) {
        servo.init();
        servo.set_enabled(true);
    }
    for (auto &motor : m_motors) {
        motor.init();
    }

    printf("Init --------- 1\n");

    m_led_strip.init();
    m_sys_sensor.init();
    m_battery_sensor.init();
    m_imu.init();
    m_display.init();
    m_display_render.init();

    printf("Init --------- 2\n");

    m_receiver.init();
    m_receiver_listener.init(m_receiver);
    m_telemetry_provider.init();
    m_receiver.set_telemetry_provider(&m_telemetry_provider);

    printf("Init --------- 3\n");

    // Register callbacks

    m_sys_sensor.add_battery_cb([](auto voltage){
        Motor::DCMotor::set_supply_voltage(voltage);
        if (voltage<battery_critical()) {
            Motor::DCMotor::set_global_enable(false);
        }
    });

    m_receiver_listener.add_callback([this](const auto &channels, const auto &mapping){
        auto connected = channels.sync() && !channels.flags().frameLost();
        if (m_connected!=connected) {
            m_connected = connected;
            m_connected_callback(m_connected);
            m_led_render.update_connected(m_connected);
        }
    });

    m_armed_callback.add([this](auto armed){
        m_display_render.update_armed(armed);
    });
}



void Robot::term()
{
    m_display_render.off();
    m_led_strip.fill(LED::Color::BLACK);
    m_led_strip.show();
    m_led_builtin.off();
}


void Robot::set_armed(bool armed) 
{
    if (armed==m_armed) 
        return ;

    m_armed = armed;
    if (m_armed) {
        printf("Armed!!\n");
        Motor::DCMotor::set_global_enable(true);
        for (auto &motor : m_motors) {
            motor.set_enabled(true);
        }
        for (auto &servo : m_servos) {
            servo.set_enabled(true);
        }
    }
    else {
        printf("Disarmed\n");
        Motor::DCMotor::set_global_enable(false);
        for (auto &motor : m_motors) {
            motor.set_enabled(false);
            motor.set_duty(0.0f);
        }
        for (auto &servo : m_servos) {
            servo.set_enabled(false);
        }
    }
    m_armed_callback(m_armed);
}


