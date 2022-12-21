/**
 * @file receiver.cpp
 * @author Peter Christoffersen
 * @brief 
 * @version 0.1
 * @date 2022-07-22
 * 
 * @copyright Copyright (c) 2022
 */

#include <fbus2/mapping.h>

#include <stdio.h>


namespace FBus2 {


TaranisX9DPlus::TaranisX9DPlus() :
    m_sa { Toggle::P1 },
    m_sb { Toggle::P0 },
    m_sc { Toggle::P0 },
    m_sd { Toggle::P0 },
    m_se { Toggle::P0},
    m_sf { false },
    m_sg { Toggle::P0 },
    m_sh { false },
    m_si { false }
{

}


void TaranisX9DPlus::set(const Channels &channels)
{
    m_sa = static_cast<Toggle>(channels[8].asButton(3));

    // Sticks
    m_left_x = channels[0];
    m_left_y = channels[1];
    m_right_x = channels[2];
    m_right_y = channels[3];

    // Slides 
    m_s1 = channels[4];
    m_s2 = channels[5];
    m_slider_l = channels[6];
    m_slider_r = channels[7];

    // Buttons
    // Fancy mapping of a toggle switch and 3x 3 way switches, and into one channel
    // by setting scale of 32, 1, 4, 16. Effectively mapping it into a bitfield. 
    // By trial and error (and spreadsheets) I have worked out a formula that maps 
    // the values to a 7 bit value with 2 bits per 3 way switch and on bit for the toggle.
    constexpr uint BASE = 471;
    constexpr uint DIVISOR = 98;
    constexpr uint OFFSET = 16;
    constexpr uint SCALE = 10;
    uint value;

    value = ((std::max(ChannelValue::CHANNEL_MIN, channels[8].raw())-BASE)*SCALE+OFFSET)/DIVISOR;
    m_sa = static_cast<Toggle>(value & 0b11);
    m_sb = static_cast<Toggle>((value>>2) & 0b11);
    m_se = static_cast<Toggle>((value>>4) & 0b11);
    m_sf =   static_cast<bool>((value>>6) & 0b1);

    value = ((std::max(ChannelValue::CHANNEL_MIN, channels[9].raw())-BASE)*SCALE+OFFSET)/DIVISOR;
    m_sc = static_cast<Toggle>(value & 0b11);
    m_sd = static_cast<Toggle>((value>>2) & 0b11);
    m_sg = static_cast<Toggle>((value>>4) & 0b11);
    m_sh =   static_cast<bool>((value>>6) & 0b1);

    m_si = channels[10].asToggle();

}



#ifndef NDEBUG
void TaranisX9DPlus::print() const 
{
    printf("CTRL: ");
    printf("left=(%.2f,%.2f) ", m_left_x.asFloat(), m_left_y.asFloat());
    printf("right=(%.2f,%.2f) ", m_right_x.asFloat(), m_right_y.asFloat());
    printf("s1=%.2f ", m_s1.asPercent());
    printf("s2=%.2f ", m_s2.asPercent());
    printf("sL=%.2f ", m_slider_l.asFloat());
    printf("sR=%.2f ", m_slider_r.asFloat());
    printf("sa=%u ", static_cast<uint>(m_sa));
    printf("sb=%u ", static_cast<uint>(m_sb));
    printf("sc=%u ", static_cast<uint>(m_sc));
    printf("sd=%u ", static_cast<uint>(m_sd));
    printf("se=%u ", static_cast<uint>(m_se));
    printf("sf=%u ", static_cast<uint>(m_sf));
    printf("sg=%u ", static_cast<uint>(m_sg));
    printf("sh=%u ", static_cast<uint>(m_sh));
    printf("si=%u ", static_cast<uint>(m_si));
    printf("\n");
}
#endif



}
