#pragma once

#include <bno055.h>
#include <ina219.h>
#include <util/callback.h>

namespace Sensor {

    class INA219 : public ::INA219::Sensor {
        public:
            using callback_type = Callback<float,float,float>; // voltage, current, power;
            //CurrentSensor

            INA219(addr_type addr, UBaseType_t task_priority) : ::INA219::Sensor { addr, task_priority } {}

            void add_callback(callback_type::call_type cb) { m_callback.add(cb); }

        private: 
            callback_type m_callback;

            virtual void on_data(float bus_v, float current, float power) override 
            {
                m_callback(bus_v, current, power);
            }

    };

    class BNO055 : public ::BNO055::Sensor {
        public:
            BNO055(addr_type addr, UBaseType_t task_priority) : ::BNO055::Sensor { addr, task_priority } {}

    };
};