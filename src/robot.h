#pragma once

#include <util/callback.h>

#include <sensor/pico_adc.h>
#include <sensor/sensors.h>
#include <motor/dcmotor.h>
#include <motor/servo.h>
#include <led/gpio_led.h>
#include <led/cyw43_led.h>
#include <radio/radio.h>

#include <telemetry/provider.h>
#include <oled/control.h>
#include <led/control.h>


class Robot {
    public:
        using armed_callback_type = Callback<bool>;
        using connected_callback_type = Callback<bool>;
        
        Robot();
        Robot(const Robot&) = delete; // No copy constructor
        Robot(Robot&&) = delete; // No move constructor

        void init();
        void start();

        bool is_armed() const { return m_armed; }        
        void set_armed(bool armed);
        void add_armed_callback(armed_callback_type::call_type cb) { m_armed_callback.add(cb); }

        // Sensors
        Sensor::PicoADC &sys_sensor() { return m_sys_sensor; }
        Sensor::INA219  &battery_sensor() { return m_battery_sensor; }
        Sensor::BNO055  &imu() { return m_imu; }
    
        // Motors and servos
        Motor::Servo::array_type   &servos() { return m_servos; }
        Motor::DCMotor::array_type &motors() { return m_motors; }
        Motor::Servo &camera() { return m_servos[CAMERA_SERVO]; }

        // Radio
        Radio::Receiver &receiver() { return m_receiver; }

        OLED::Control &oled() { return m_oled; }
        LED::Control &leds() { return m_leds; }

    private:
        Sensor::PicoADC m_sys_sensor;
        Sensor::INA219  m_battery_sensor;
        Sensor::BNO055  m_imu;

        Motor::Servo::array_type   m_servos;
        Motor::DCMotor::array_type m_motors;

        Radio::Receiver m_receiver;

        Telemetry::Provider m_telemetry_provider;
        OLED::Control m_oled;
        LED::Control m_leds;

        bool m_armed;
        armed_callback_type m_armed_callback;

        bool m_connected;
        connected_callback_type m_connected_callback;

};

