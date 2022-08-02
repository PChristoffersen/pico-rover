#include <pico/binary_info.h>

#include "boardconfig.h"

bi_decl(bi_1pin_with_name(MOTOR_ENABLE_PIN, "Motor enable"));

bi_decl(bi_1pin_with_name(MOTOR1_IN1_PIN, "Motor 1 IN1"));
bi_decl(bi_1pin_with_func(MOTOR1_IN1_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR1_IN2_PIN, "Motor 1 IN2"));
bi_decl(bi_1pin_with_func(MOTOR1_IN2_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR1_ENCA_PIN, "Motor 1 ENCA"));
bi_decl(bi_1pin_with_func(MOTOR1_ENCA_PIN, GPIO_FUNC_PIO1))
bi_decl(bi_1pin_with_name(MOTOR1_ENCB_PIN, "Motor 1 ENCB"));
bi_decl(bi_1pin_with_func(MOTOR1_ENCB_PIN, GPIO_FUNC_PIO1))

bi_decl(bi_1pin_with_name(MOTOR2_IN1_PIN, "Motor 2 IN1"));
bi_decl(bi_1pin_with_func(MOTOR2_IN1_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR2_IN2_PIN, "Motor 2 IN2"));
bi_decl(bi_1pin_with_func(MOTOR2_IN2_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR2_ENCA_PIN, "Motor 2 ENCA"));
bi_decl(bi_1pin_with_func(MOTOR2_ENCA_PIN, GPIO_FUNC_PIO1))
bi_decl(bi_1pin_with_name(MOTOR2_ENCB_PIN, "Motor 2 ENCB"));
bi_decl(bi_1pin_with_func(MOTOR2_ENCB_PIN, GPIO_FUNC_PIO1))

bi_decl(bi_1pin_with_name(MOTOR3_IN1_PIN, "Motor 3 IN1"));
bi_decl(bi_1pin_with_func(MOTOR3_IN1_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR3_IN2_PIN, "Motor 3 IN2"));
bi_decl(bi_1pin_with_func(MOTOR3_IN2_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR3_ENCA_PIN, "Motor 3 ENCA"));
bi_decl(bi_1pin_with_func(MOTOR3_ENCA_PIN, GPIO_FUNC_PIO1))
bi_decl(bi_1pin_with_name(MOTOR3_ENCB_PIN, "Motor 3 ENCB"));
bi_decl(bi_1pin_with_func(MOTOR3_ENCB_PIN, GPIO_FUNC_PIO1))

bi_decl(bi_1pin_with_name(MOTOR4_IN1_PIN, "Motor 4 IN1"));
bi_decl(bi_1pin_with_func(MOTOR4_IN1_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR4_IN2_PIN, "Motor 4 IN2"));
bi_decl(bi_1pin_with_func(MOTOR4_IN2_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(MOTOR4_ENCA_PIN, "Motor 4 ENCA"));
bi_decl(bi_1pin_with_func(MOTOR4_ENCA_PIN, GPIO_FUNC_PIO1))
bi_decl(bi_1pin_with_name(MOTOR4_ENCB_PIN, "Motor 4 ENCB"));
bi_decl(bi_1pin_with_func(MOTOR4_ENCB_PIN, GPIO_FUNC_PIO1))

bi_decl(bi_1pin_with_name(SERVO1_PIN, "Servo 1"));
bi_decl(bi_1pin_with_func(SERVO1_PIN, GPIO_FUNC_PWM))
bi_decl(bi_1pin_with_name(SERVO2_PIN, "Servo 2"));
bi_decl(bi_1pin_with_func(SERVO2_PIN, GPIO_FUNC_PWM))

bi_decl(bi_1pin_with_name(LED_STRIP_PIN, "WS281x LED"));
bi_decl(bi_1pin_with_func(LED_STRIP_PIN, GPIO_FUNC_PIO0))

bi_decl(bi_1pin_with_name(BATTERY_SENSE_PIN, "Battery voltage"));

bi_decl(bi_1pin_with_name(RADIO_RECEIVER_TX_PIN, "Radio TX"));
bi_decl(bi_1pin_with_func(RADIO_RECEIVER_TX_PIN, GPIO_FUNC_UART));
bi_decl(bi_1pin_with_name(RADIO_RECEIVER_RX_PIN, "Radio RX"));;
bi_decl(bi_1pin_with_func(RADIO_RECEIVER_RX_PIN, GPIO_FUNC_UART));

bi_decl(bi_1pin_with_name(BOARD_I2C_SDA_PIN, "I2C SDA"));
bi_decl(bi_1pin_with_func(BOARD_I2C_SDA_PIN, GPIO_FUNC_I2C));
bi_decl(bi_1pin_with_name(BOARD_I2C_SCL_PIN, "I2C SCL"));
bi_decl(bi_1pin_with_func(BOARD_I2C_SCL_PIN, GPIO_FUNC_I2C));

bi_decl(bi_1pin_with_name(GENERIC_INTR_PIN, "Generic IO 0"));
