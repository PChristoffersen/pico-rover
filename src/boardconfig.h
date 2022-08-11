#ifndef _BOARD_CONFIG_
#define _BOARD_CONFIG_

#include <limits.h>
#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/uart.h>
#include <hardware/i2c.h>


/* Servo */
#define SERVO1_PIN 26
#define SERVO2_PIN 27


/* RADIO */
#define RADIO_RECEIVER_TX_PIN 16
#define RADIO_RECEIVER_RX_PIN 17
#define RADIO_RECEIVER_BAUD_RATE 460800
//#define RADIO_RECEIVER_BAUD_RATE 115200
#define RADIO_RECEIVER_UART uart0


/* LED */
#define LED_STRIP_PIO              pio0
#define LED_STRIP_PIN              19
#define LED_STRIP_IS_RGBW          false
#define LED_STRIP_PIXEL_COUNT      16

/* Battery */
#define BATTERY_SENSE_PIN 28
#define BATTERY_SENSE_R1 68200.0f
#define BATTERY_SENSE_R2 32000.0f
#define BATTERY_TYPE LIPO_2CELL


/* Motor control */
#define MOTOR_ENABLE_PIN 18
#define MOTOR1_IN1_PIN   15
#define MOTOR1_IN2_PIN   14
#define MOTOR1_ENCA_PIN  12
#define MOTOR1_ENCB_PIN  13
#define MOTOR2_IN1_PIN    9
#define MOTOR2_IN2_PIN    8
#define MOTOR2_ENCA_PIN  10
#define MOTOR2_ENCB_PIN  11
#define MOTOR3_IN1_PIN    5
#define MOTOR3_IN2_PIN    4
#define MOTOR3_ENCA_PIN   6
#define MOTOR3_ENCB_PIN   7
#define MOTOR4_IN1_PIN    1
#define MOTOR4_IN2_PIN    0
#define MOTOR4_ENCA_PIN   2
#define MOTOR4_ENCB_PIN   3
#define MOTOR_ENCODER_PIO pio1
#define MOTOR_TARGET_VOLTAGE 6.5f


/* Misc */
#define BOARD_GPIO0_PIN 22

/* I2C */
#define BOARD_I2C_SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
#define BOARD_I2C_SCL_PIN PICO_DEFAULT_I2C_SCL_PIN
#define BOARD_I2C i2c_default


//#define BOARD_I2C_SPEED 100000u  // Standard mode  (100kpbs)
//#define BOARD_I2C_SPEED 400000u  // Fast mode      (400kbps)
#define BOARD_I2C_SPEED 1000000u // Fast mode plus (1Mbps)

/* OLED */
#define OLED_ADDRESS OLED::Display::Address::DISPLAY1
#define OLED_TYPE OLED::Display::Type::SSD1306_128x64


/* Debug */
#if LIB_PICO_STDIO_UART
#if  PICO_DEFAULT_UART_TX_PIN == RADIO_RECEIVER_TX_PIN
#define DEBUG_USE_RECEIVER_UART 1
#endif

#endif

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
