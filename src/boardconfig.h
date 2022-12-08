#ifndef _BOARD_CONFIG_
#define _BOARD_CONFIG_

#include <limits.h>
#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/uart.h>
#include <hardware/i2c.h>


/* Servo */
static constexpr size_t SERVO_COUNT { 2 };
static constexpr uint SERVO1_PIN { 26 };
static constexpr uint SERVO2_PIN { 27 };


/* RADIO */
#define RADIO_RECEIVER_UART uart0
static constexpr uint RADIO_RECEIVER_TX_PIN { 16 };
static constexpr uint RADIO_RECEIVER_RX_PIN { 17 };
static constexpr uint RADIO_RECEIVER_BAUD_RATE { 460800 };
//static constexpr uint RADIO_RECEIVER_BAUD_RATE { 115200 };


/* LED */
#define LED_STRIP_PIO                       pio0
static constexpr uint LED_STRIP_PIN         { 19 };
static constexpr bool LED_STRIP_IS_RGBW     { false };
static constexpr uint LED_STRIP_PIXEL_COUNT { 2*24 };

/* Battery */
#define BATTERY_TYPE LIPO_2CELL
static constexpr uint  BATTERY_SENSE_PIN { 28 };
static constexpr float BATTERY_SENSE_R1  { 68200.0f };
static constexpr float BATTERY_SENSE_R2  { 32000.0f };


/* Motor control */
#define MOTOR_ENCODER_PIO pio1
static constexpr size_t MOTOR_COUNT    { 4 };
static constexpr uint MOTOR_ENABLE_PIN { 18 };
static constexpr uint MOTOR1_IN1_PIN   { 15 };
static constexpr uint MOTOR1_IN2_PIN   { 14 };
static constexpr uint MOTOR1_ENCA_PIN  { 12 };
static constexpr uint MOTOR1_ENCB_PIN  { 13 };
static constexpr uint MOTOR2_IN1_PIN   {  9 };
static constexpr uint MOTOR2_IN2_PIN   {  8 };
static constexpr uint MOTOR2_ENCA_PIN  { 10 };
static constexpr uint MOTOR2_ENCB_PIN  { 11 };
static constexpr uint MOTOR3_IN1_PIN   {  5 };
static constexpr uint MOTOR3_IN2_PIN   {  4 };
static constexpr uint MOTOR3_ENCA_PIN  {  6 };
static constexpr uint MOTOR3_ENCB_PIN  {  7 };
static constexpr uint MOTOR4_IN1_PIN   {  1 };
static constexpr uint MOTOR4_IN2_PIN   {  0 };
static constexpr uint MOTOR4_ENCA_PIN  {  2 };
static constexpr uint MOTOR4_ENCB_PIN  {  3 };
//static constexpr float MOTOR_PWM_FREQUENCY { 12500.0f }; // 1.25 kHz
static constexpr float MOTOR_PWM_FREQUENCY { 25000.0f }; // 25 kHz
static constexpr float MOTOR_TARGET_VOLTAGE { 7.0f };
static constexpr float MOTOR_SHAFT_CPR { 44 }; // Counts per revolution of motor shaft
static constexpr float MOTOR_GEAR_RATIO { 34.02 }; // Gear ratio of shaft to wheel


/* Misc */
static constexpr uint BOARD_GPIO0_PIN { 22 };

/* I2C */
#define BOARD_I2C i2c_default
static constexpr uint BOARD_I2C_SDA_PIN { PICO_DEFAULT_I2C_SDA_PIN };
static constexpr uint BOARD_I2C_SCL_PIN { PICO_DEFAULT_I2C_SCL_PIN };
//static constexpr uint BOARD_I2C_SPEED { 100000u  }; // Standard mode  (100kpbs)
//static constexpr uint BOARD_I2C_SPEED { 400000u  }; // Fast mode      (400kbps)
static constexpr uint BOARD_I2C_SPEED { 1000000u }; // Fast mode plus (1Mbps)

/* OLED */
#define OLED_ADDRESS OLED::Display::Address::DISPLAY1
#define OLED_TYPE    OLED::Display::Type::SSD1306_128x64


/* Debug */
#ifndef BOARD_GPIO_STDIO
#define BOARD_GPIO_STDIO 0
#endif
#define DEBUG_GPIO_PIO pio0
constexpr bool DEBUG_GPIO_ENABLED { BOARD_GPIO_STDIO };
constexpr uint DEBUG_GPIO_TX_PIN { BOARD_GPIO0_PIN };
//constexpr uint DEBUG_UART_SPEED { 115200 };
//constexpr uint DEBUG_GPIO_SPEED { 460800 };
constexpr uint DEBUG_GPIO_SPEED { 921600 };

#endif
