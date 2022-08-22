#pragma once

#include <util/callback.h>

#include <sensor/pico_adc.h>
#include <sensor/ina219.h>
#include <sensor/bno0055.h>
#include <motor/dcmotor.h>
#include <motor/servo.h>
#include <led/single.h>
#include <led/strip.h>
#include <oled/display.h>
#include <radio/frsky_receiver.h>
#include <radio/frsky_receiverlistener.h>

#include <telemetryprovider.h>
#include <displayrender.h>
#include <ledrender.h>


class Robot {
    public:
        using armed_callback_type = Callback<bool>;
        using connected_callback_type = Callback<bool>;

        Robot();
        Robot(const Robot&) = delete; // No copy constructor
        Robot(Robot&&) = delete; // No move constructor

        void init();
        void term();

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

        // LED/Displays
        LED::Single &led_builtin() { return m_led_builtin; };
        LED::Strip<LED_STRIP_PIXEL_COUNT> &led_strip() { return m_led_strip; }
        OLED::Display &display() { return m_display; }

        // Radio
        Radio::FrSky::Receiver &receiver() { return m_receiver; }
        Radio::FrSky::ReceiverListener &receiver_listener() { return m_receiver_listener; }

        TelemetryProvider &telemetry_provider() { return m_telemetry_provider; }
        DisplayRender &display_render() { return m_display_render; }
        LEDRender &led_render() { return m_led_render; }

    private:
        Sensor::PicoADC m_sys_sensor;
        Sensor::INA219  m_battery_sensor;
        Sensor::BNO055  m_imu;

        Motor::Servo::array_type   m_servos;
        Motor::DCMotor::array_type m_motors;

        Radio::FrSky::Receiver m_receiver;
        Radio::FrSky::ReceiverListener m_receiver_listener;

        LED::Single m_led_builtin;
        LED::Strip<LED_STRIP_PIXEL_COUNT> m_led_strip;
        OLED::Display m_display;

        TelemetryProvider m_telemetry_provider;
        DisplayRender m_display_render;
        LEDRender m_led_render;

        bool m_armed;
        armed_callback_type m_armed_callback;

        bool m_connected;
        connected_callback_type m_connected_callback;

};

