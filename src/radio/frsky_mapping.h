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

#include "frsky_channels.h"

namespace Radio::FrSky {

    enum class Toggle : uint {
        P0 = 0,
        P1 = 1,
        P2 = 2,
    };


    class TaranisX9DPlus {
        public:
            TaranisX9DPlus();

            // Axis
            ChannelValue left_x() const { return m_left_x; }
            ChannelValue left_y() const { return m_left_y; }
            ChannelValue right_x() const { return m_right_x; }
            ChannelValue right_y() const { return m_right_y; }

            // Dials and sliders
            ChannelValue s1() const { return m_s1; }// Left front dial
            ChannelValue s2() const { return m_s2; }// Right front dial
            ChannelValue slider_l() const { return m_slider_l; } // Left side slider
            ChannelValue slider_r() const { return m_slider_r; } // Right side slider

            // Buttons
            Toggle sa() const { return m_sa; }; // 3 way toggle (left front L)
            Toggle sb() const { return m_sb; }; // 3 way toggle (left front R)
            Toggle sc() const { return m_sc; }; // 3 way toggle (right front L)
            Toggle sd() const { return m_sd; }; // 3 way toggle (right front R)
            Toggle se() const { return m_se; }; // 3 way toggle (left top front) 
            bool   sf() const { return m_sf; }; // 2 way toggle (left top rear) 
            Toggle sg() const { return m_sg; }; // 3 way toggle (right top front) 
            bool   sh() const { return m_sh; }; // 2 way toggle (right top rear), momentary
            bool   si() const { return m_si; }; // top button, momentary

            void set(const Channels &channels);
            void set_lost_sync(Channels &channels);

            #ifndef NDEBUG
            void print() const;
            #endif

        private:
            ChannelValue m_left_x;
            ChannelValue m_left_y;
            ChannelValue m_right_x;
            ChannelValue m_right_y;

            ChannelValue m_s1;
            ChannelValue m_s2;
            ChannelValue m_slider_l;
            ChannelValue m_slider_r;

            Toggle m_sa;
            Toggle m_sb;
            Toggle m_sc;
            Toggle m_sd;
            Toggle m_se;
            bool   m_sf;
            Toggle m_sg;
            bool   m_sh;
            bool   m_si;


    };

}
