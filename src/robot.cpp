#include "robot.h"

#include <pico/stdlib.h>
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
    m_receiver {},

    // Higher level objects
    m_telemetry_provider { *this },
    m_oled { *this },
    m_leds { *this }

{

}





void Robot::init()
{
    m_leds.init();
    m_leds.builtin().on();
    m_oled.init();

    for (auto &servo : m_servos) {
        servo.init();
        servo.set_enabled(true);
    }
    for (auto &motor : m_motors) {
        motor.init();
    }

    m_sys_sensor.init();
    m_battery_sensor.init();
    m_imu.init();

    m_receiver.init();
    m_telemetry_provider.init();
    m_receiver.set_telemetry_provider(&m_telemetry_provider);

    // Register callbacks

    m_sys_sensor.add_battery_cb([](auto voltage){
        Motor::DCMotor::set_supply_voltage(voltage);
        if (voltage<battery_critical()) {
            Motor::DCMotor::set_global_enable(false);
        }
    });

    m_receiver.add_callback([this](const auto &channels, const auto &mapping){
        auto connected = channels.sync() && !channels.flags().frameLost();
        if (m_connected!=connected) {
            m_connected = connected;
            m_connected_callback(m_connected);
            m_leds.update_connected(m_connected);
        }
    });

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


