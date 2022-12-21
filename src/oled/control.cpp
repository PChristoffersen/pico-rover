#include "control.h"

#include <algorithm>

#include <util/battery.h>
#include <robot.h>

#include <resources/raspberry_logo.image.h>
#include <resources/battery.image.h>
#include <resources/signal1.image.h>
#include <resources/signal2.image.h>
#include <resources/signal3.image.h>
#include <resources/signal4.image.h>
#include <resources/warning.image.h>
#include <resources/noconnection.image.h>
#include <resources/check.image.h>
#include <resources/cancel.image.h>

#include <liberationmono_16.font.h>
#include <liberationmono_24.font.h>
#include <liberationsans_16.font.h>
#include <liberationsans_24.font.h>
#include <ssd1306/fixed_5x8.font.h>
#include <ssd1306/fixed_8x8.font.h>
#include <ssd1306/fixed_12x16.font.h>
#include <ssd1306/fixed_16x32.font.h>

namespace OLED {


Control::State::State() : 
    armed { false },
    battery_level { UINT32_MAX }
{

}


Control::Control(Robot &robot) :
    m_display { OLED_ADDRESS },
    m_framebuffer(m_display.framebuffer()),
    m_robot { robot },
    m_sem { nullptr },
    m_task { nullptr }
{
    m_sem = xSemaphoreCreateMutexStatic(&m_sem_buf);
    assert(m_sem);
    xSemaphoreGive(m_sem);
}



inline void Control::run()
{
    TickType_t last_time = xTaskGetTickCount();
    TickType_t last_radio = last_time;
    TickType_t last_battery = last_time;
    TickType_t last_armed = last_time;

    xTaskDelayUntil(&last_time, pdMS_TO_TICKS(START_DELAY_MS));

    m_framebuffer.clear();
    update_armed(true);
    update_battery(true);
    update_radio(true);
    

    while (true) {
        TickType_t now = xTaskGetTickCount();

        if (now-last_radio >= pdMS_TO_TICKS(RADIO_INTERVAL_MS)) {
            update_radio(false);
            last_radio = now;
        }
        if (now-last_battery >= pdMS_TO_TICKS(BATTERY_INTERVAL_MS)) {
            update_battery(false);
            last_battery = now;
        }
        if (now-last_armed >= pdMS_TO_TICKS(ARMED_INTERVAL_MS)) {
            update_armed(false);
            last_armed = now;
        }

        if (m_display.update_needed()) {
            m_display.update();
        }

        xTaskDelayUntil(&last_time, pdMS_TO_TICKS(UPDATE_INTERVAL_MS));
    }
}


void Control::init()
{
    m_battery_show = true;

    m_display.init();
    m_framebuffer.draw_bitmap((m_framebuffer.width()-Resource::Image::Raspberry_Logo.width())/2, (m_framebuffer.height()-Resource::Image::Raspberry_Logo.height())/2, Resource::Image::Raspberry_Logo);
    m_display.update_sync();

    if (m_display.present()) {
        m_task = xTaskCreateStatic([](auto arg){ reinterpret_cast<Control*>(arg)->run(); }, "Display", TASK_STACK_SIZE, this, DISPLAY_TASK_PRIORITY, m_task_stack, &m_task_buf);
        assert(m_task);
        vTaskSuspend(m_task);
    }
}


void Control::start() 
{
    if (m_task) {
        vTaskResume(m_task);
    }
}



void Control::off()
{
    if (!m_display.present()) 
        return;

    m_framebuffer.clear();
    m_display.update();
}



void Control::update_armed(bool force)
{
    bool armed = m_robot.is_armed();

    if (!force && m_state_drawn.armed==armed)
        return;

    m_state_drawn.armed = armed;

    constexpr int AREA_LEFT   {  8 };
    constexpr int AREA_TOP    {  0 };
    constexpr int AREA_WIDTH  { 24 };
    constexpr int AREA_HEIGHT {  8 };

    constexpr auto &font { ::SSD1306::Resource::Font::Fixed_5x8 };

    m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT, framebuffer_type::DrawOp::SUBTRACT);
    if (m_state_drawn.armed) {
        m_framebuffer.draw_text(AREA_LEFT, AREA_TOP, "ARMED", font);
    }
    else {
    }
}


void Control::update_battery(bool force)
{
    float vbat = m_robot.battery_sensor().get_bus_voltage();
    float vsys = m_robot.sys_sensor().get_vsys();
    float current = m_robot.battery_sensor().get_current();
    bool critical = vbat < battery_critical();
    auto level = battery_level(vbat);

    if (force || critical || m_state_drawn.battery_level != level) {
        // Update battery icon and percent
        constexpr int AREA_LEFT  { 128-Resource::Image::Battery.width() };
        constexpr int AREA_TOP   {  0 };
        constexpr int BAR_LEFT   {  7 };
        constexpr int BAR_TOP    { 11 };
        constexpr int BAR_HEIGHT { 30 };

        // Clear
        m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, Resource::Image::Battery.width(),Resource::Image::Battery.height()+16, framebuffer_type::DrawOp::SUBTRACT);

        if (critical) 
            m_battery_show = !m_battery_show;
        else
            m_battery_show = true;

        if (m_battery_show) {
            // Icon
            m_framebuffer.draw_bitmap(AREA_LEFT, AREA_TOP, Resource::Image::Battery);
            int bar_h = BAR_HEIGHT*level/100;
            m_framebuffer.fill_rect(AREA_LEFT+BAR_LEFT, AREA_TOP+BAR_TOP+BAR_HEIGHT-bar_h, Resource::Image::Battery.width()-BAR_LEFT*2, bar_h);

        }

        // Percent
        const Font &font = Resource::Font::LiberationSans_16;
        char text[8];
        sprintf(text, "%u", level);
        m_framebuffer.draw_text(AREA_LEFT+(Resource::Image::Battery.width()-font.width(text))/2, AREA_TOP+Resource::Image::Battery.height(), text, font);

        m_state_drawn.battery_level = level;
    }

    {
        // Update voltage and amps
        const Font &font = SSD1306::Resource::Font::Fixed_8x8;
        const Font &font2 = Resource::Font::LiberationSans_16;
        constexpr int AREA_TOP  { 64-16*3 };
        constexpr int AREA_LEFT { 0 };
        constexpr uint AREA_WIDTH { 128-Resource::Image::Battery.width() };
        constexpr uint AREA_HEIGHT { 16*3 };

        // Clear
        m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT, framebuffer_type::DrawOp::SUBTRACT);
        //m_framebuffer.draw_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT);

        char text[16];
        int ypos = AREA_TOP;

        sprintf(text, "%.2fV", vbat);
        m_framebuffer.draw_text(AREA_LEFT, ypos, "Batt", font);
        m_framebuffer.draw_text(AREA_LEFT+4*8+10, ypos, text, font2);
        ypos+=16;

        sprintf(text, "%.2fV", vsys);
        m_framebuffer.draw_text(AREA_LEFT, ypos, "Sys", font);
        m_framebuffer.draw_text(AREA_LEFT+4*8+10, ypos, text, font2);
        ypos+=16;

        if (current<0.0f)
            current = 0.0f;
        sprintf(text, "%.2fA", current/1000.0f);
        m_framebuffer.draw_text(AREA_LEFT, ypos, "Power", font);
        m_framebuffer.draw_text(AREA_LEFT+4*8+10, ypos, text, font2);
        ypos+=16;
    }
}


void Control::update_radio(bool force)
{
    m_robot.receiver().lock();
    auto flags = m_robot.receiver().flags();
    auto sync= m_robot.receiver().sync();
    auto rssi = m_robot.receiver().rssi();
    m_robot.receiver().unlock();

    constexpr int TEXT_WIDTH  { 12 };
    constexpr int AREA_LEFT   { 128-Resource::Image::Battery.width()-20-TEXT_WIDTH };
    constexpr int AREA_TOP    {  0 };
    constexpr int AREA_WIDTH  { 16+TEXT_WIDTH };
    constexpr int AREA_HEIGHT { 16 };
    constexpr int ICON_LEFT  { AREA_LEFT+TEXT_WIDTH };

    m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT, framebuffer_type::DrawOp::SUBTRACT);

    if (!sync) {
        // Not in sync with receiver module
        m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Resource::Image::Warning);
    }
    else if (flags.frameLost()) {
        // Receiver module lost connection to transmitter
        m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Resource::Image::NoConnection);
    }
    else {
        // All is good
        constexpr auto &font { SSD1306::Resource::Font::Fixed_8x8 };
        if (rssi>99) 
            rssi = 99;
        char text[4];
        sprintf(text, "%u", rssi);
        m_framebuffer.draw_text(AREA_LEFT, AREA_TOP, text, font);
        if (rssi > 80) {
            m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Resource::Image::Signal4);
        }
        else if (rssi > 60) {
            m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Resource::Image::Signal3);
        }
        else if (rssi > 40) {
            m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Resource::Image::Signal2);
        }
        else {
            m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Resource::Image::Signal1);
        }
    }
}

}
