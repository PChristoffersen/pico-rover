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
#define LED_PIN PICO_DEFAULT_LED_PIN
#define LED_BLINK_INTERVAL 500000u
#define LED_STRIP_PIN 19
#define LED_STRIP_IS_RGBW false
#define LED_STRIP_PIXEL_COUNT 16
#define LED_STRIP_FREQUENCY 800000.0f
#define LED_STRIP_RESET_DELAY_US 400u
#define LED_STRIP_PIO pio0


/* Battery */
#define BATTERY_SENSE_PIN 28
#define BATTERY_SENSE_ADC 2
#define BATTERY_SENSE_R1 68200.0f
#define BATTERY_SENSE_R2 32000.0f
#define BATTERY_MIN_VOLTAGE 3.0f


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
#define MOTOR_TARGET_VOLTAGE 6.0f
#define MOTOR_BATT_VOLTAGE 8.4f
#define MOTOR_PWM_MAX 1000
#define MOTOR_PWM_WRAP (MOTOR_PWM_MAX+250)
//#define MOTOR_PWM_FREQUENCY 12500.0f // 1.25 kHz
#define MOTOR_PWM_FREQUENCY 25000.0f // 25 kHz
//#define MOTOR_PWM_FREQUENCY 250.0f // 2.5 kHz


/* Misc */
#define GENERIC_INTR_PIN 22

/* I2C */
#define BOARD_I2C_SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
#define BOARD_I2C_SCL_PIN PICO_DEFAULT_I2C_SCL_PIN
#define BOARD_I2C i2c_default

//#define BOARD_I2C_SPEED 100000u  // Standard mode  (100kpbs)
//#define BOARD_I2C_SPEED 400000u  // Fast mode      (400kbps)
#define BOARD_I2C_SPEED 1000000u // Fast mode plus (1Mbps)

/* OLED */
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDRESS 0x3D


/* Debug */
#if LIB_PICO_STDIO_UART
#if  PICO_DEFAULT_UART_TX_PIN == RADIO_RECEIVER_TX_PIN
#define DEBUG_USE_RECEIVER_UART 1
#endif

#endif

#ifndef GENERIC_INTR_STDIO
#define GENERIC_INTR_STDIO 0
#endif
#if GENERIC_INTR_STDIO
#define DEBUG_UART_PIO pio0
#define DEBUG_UART_TX GENERIC_INTR_PIN
#define DEBUG_UART_SPEED 115200
#endif

#endif
