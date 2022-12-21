/**
 * @author Peter Christoffersen
 * @brief FrSky channel data
 * @date 2022-08-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <array>
#include <pico/stdlib.h>

namespace FBus2 {

    class ChannelValue {
        public:
            using raw_type = uint16_t;

            static constexpr raw_type CHANNEL_MIN { 8u };
            static constexpr raw_type CHANNEL_MAX { 1976u };
            static constexpr raw_type CHANNEL_RANGE { CHANNEL_MAX-CHANNEL_MIN };
            static constexpr raw_type CHANNEL_CENTER { CHANNEL_MAX/2u };

            static constexpr uint SERVO_MIN { 500 };
            static constexpr uint SERVO_MAX { 2500 };

            static const ChannelValue CENTER;
            static const ChannelValue MIN;
            static const ChannelValue MAX;

            constexpr ChannelValue() : m_value { CHANNEL_CENTER } {}
            constexpr ChannelValue(raw_type value) : m_value { value } { }
            constexpr ChannelValue(const ChannelValue &other) : m_value { other.m_value } { }
            constexpr ChannelValue(const ChannelValue &&other) : m_value { other.m_value } { }

            ChannelValue constexpr operator-() const { return (CHANNEL_MAX-m_value+CHANNEL_MIN); }

            ChannelValue &operator=(const ChannelValue &other) { m_value = other.m_value; return *this; }
            ChannelValue &operator=(const raw_type value) { m_value = value; return *this; }

            inline constexpr bool operator==(const ChannelValue& other) const { return m_value == other.m_value; }
            inline constexpr bool operator!=(const ChannelValue& other) const { return m_value != other.m_value; }

            constexpr raw_type raw() const { return m_value; }
            
            uint  asServoPulse() const           { return SERVO_MIN+static_cast<uint>(m_value-CHANNEL_MIN)*(SERVO_MAX-SERVO_MIN)/(CHANNEL_MAX-CHANNEL_MIN); }
            float asPercent() const              { return static_cast<float>(m_value-CHANNEL_MIN)/CHANNEL_RANGE; }
            float asFloat() const                { return static_cast<float>((2.0f*(static_cast<int32_t>(m_value)-CHANNEL_CENTER)))/CHANNEL_RANGE; }
            uint  asButton(uint positions) const { auto range = CHANNEL_RANGE/(positions-1); return (m_value-CHANNEL_MIN+range/2) / range; }
            bool  asToggle() const               { return asButton(2); }

        private:
            raw_type m_value;
    };

    constexpr ChannelValue ChannelValue::CENTER { ChannelValue::CHANNEL_CENTER };
    constexpr ChannelValue ChannelValue::MIN    { ChannelValue::CHANNEL_MIN };
    constexpr ChannelValue ChannelValue::MAX    { ChannelValue::CHANNEL_MAX };


    class Flags {
        public:
            using value_type = uint8_t;

            static constexpr value_type CH17_MASK       { 1<<0 };
            static constexpr value_type CH18_MASK       { 1<<1 };
            static constexpr value_type FRAME_LOST_MASK { 1<<2 };
            static constexpr value_type FAILSAFE_MASK   { 1<<3 };

            static constexpr value_type INITIAL_VALUE { FRAME_LOST_MASK };

            constexpr Flags() : m_value { INITIAL_VALUE } {}
            constexpr Flags(value_type value) : m_value { value } {}
            constexpr Flags(const Flags &other) : m_value { other.m_value } {}
            constexpr Flags(const Flags &&other) : m_value { other.m_value } {}

            Flags &operator=(const Flags &other) { m_value = other.m_value; return *this; }
            Flags &operator=(const value_type value) { m_value = value; return *this; }

            bool operator==(const Flags &other) { return m_value==other.m_value; }
            bool operator==(const value_type value) { return m_value==value; }
            bool operator!=(const Flags &other) { return m_value!=other.m_value; }
            bool operator!=(const value_type value) { return m_value!=value; }

            inline bool ch17() const { return m_value & CH17_MASK; }
            inline bool ch18() const { return m_value & CH18_MASK; }
            inline bool frameLost() const { return m_value & FRAME_LOST_MASK; }
            inline bool failsafe() const { return m_value & FAILSAFE_MASK; }

        private:
            value_type m_value;
    };


    class Channels {
        public: 
            using value_type = ChannelValue;
            using flag_type = Flags;
            using rssi_type = uint8_t;
            using const_iterator = const value_type*;


            static constexpr size_t MAX_CHANNELS { 24 };
            static constexpr size_t INITIAL_COUNT { 16 };

            Channels() : m_sync { false }, m_seq{ 0 }, m_rssi { 0 }, m_count { INITIAL_COUNT } { }

            bool sync() const { return m_sync; }
            uint seq() const { return m_seq; }

            const flag_type &flags() const { return m_flags; }
            const rssi_type &rssi() const  { return m_rssi; }

            size_t count() const { return m_count; }

            value_type &operator[](size_t n) noexcept { assert(n<m_count); return m_data[n]; }
            constexpr const value_type &operator[](size_t n) const noexcept { assert(n<m_count); return m_data[n]; }

            void set_count(size_t count) { assert(count<=MAX_CHANNELS); m_count = count; }
            void set_channel(size_t n, value_type::raw_type value) { assert(n<m_count); m_data[n] = value; }
            void set_flags(uint8_t flags) { m_flags = flags; }
            void set_rssi(uint8_t rssi) { m_rssi = rssi; }
            void set_sync(bool sync) { m_sync = sync; }
            void set_seq(uint seq) { m_seq = seq; }

            const_iterator begin() const noexcept { return m_data.begin(); }
            const_iterator end() const noexcept { return m_data.begin()+m_count; }

        private:
            using array_type = std::array<value_type, MAX_CHANNELS>;

            bool      m_sync;
            uint      m_seq;
            flag_type m_flags;
            rssi_type m_rssi;

            size_t m_count;
            array_type m_data;
    };




};
