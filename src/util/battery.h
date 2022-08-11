/**
 * @author Peter Christoffersen
 * @brief Battery utility function
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>
#include <pico/stdlib.h>

#include <boardconfig.h>

// LiPo voltage levels in 5% increments 
// from https://blog.ampow.com/lipo-voltage-chart/

static constexpr uint BATTERY_LEVEL_INCREMENT { 5u };
static constexpr uint BATTERY_CRITICAL_INDEX { 16 };

static constexpr float LIPO_1CELL[] = {
    4.20f, // 100%
    4.15f, //  95%
    4.11f, //  90%
    4.08f, //  85%
    4.02f, //  80%
    3.98f, //  75%
    3.95f, //  70%
    3.91f, //  65%
    3.87f, //  60%
    3.85f, //  55%
    3.84f, //  50%
    3.82f, //  45%
    3.80f, //  40%
    3.79f, //  35%
    3.77f, //  30%
    3.75f, //  25%
    3.73f, //  20%
    3.71f, //  15%
    3.69f, //  10%
    3.61f, //   5%
    3.27f, //   0%
};


static constexpr float LIPO_2CELL[] = {
    8.40f, // 100%
    8.30f, //  95%
    8.22f, //  90%
    8.16f, //  85%
    8.05f, //  80%
    7.97f, //  75%
    7.91f, //  70%
    7.83f, //  65%
    7.75f, //  60%
    7.71f, //  55%
    7.67f, //  50%
    7.63f, //  45%
    7.59f, //  40%
    7.57f, //  35%
    7.53f, //  30%
    7.49f, //  25%
    7.45f, //  20%
    7.41f, //  15%
    7.37f, //  10%
    7.22f, //   5%
    6.55f, //   0%
};


static constexpr float LIPO_3CELL[] = {
    12.60f, // 100%
    12.45f, //  95%
    12.33f, //  90%
    12.25f, //  85%
    12.07f, //  80%
    11.95f, //  75%
    11.86f, //  70%
    11.74f, //  65%
    11.62f, //  60%
    11.56f, //  55%
    11.51f, //  50%
    11.45f, //  45%
    11.39f, //  40%
    11.36f, //  35%
    11.30f, //  30%
    11.24f, //  25%
    11.18f, //  20%
    11.12f, //  15%
    11.06f, //  10%
    10.83f, //   5%
    9.82f,  //   0%
};

static constexpr float LIPO_4CELL[] = {
    16.80f, // 100%
    16.60f, //  95%
    16.45f, //  90%
    16.33f, //  85%
    16.09f, //  80%
    15.93f, //  75%
    15.81f, //  70%
    15.66f, //  65%
    15.50f, //  60%
    15.42f, //  55%
    15.34f, //  50%
    15.26f, //  45%
    15.18f, //  40%
    15.14f, //  35%
    15.06f, //  30%
    14.99f, //  25%
    14.91f, //  20%
    14.83f, //  15%
    14.75f, //  10%
    14.43f, //   5%
    13.09f, //   0%
};


static constexpr float LIPO_5CELL[] = {
    21.00f,
    20.75f,
    20.56f,
    20.41f,
    20.11f,
    19.92f,
    19.77f,
    19.57f,
    19.37f,
    19.27f,
    19.18f,
    19.08f,
    18.98f,
    18.93f,
    18.83f,
    18.73f,
    18.63f,
    18.54f,
    18.44f,
    18.04f,
    16.37f,
};


static constexpr float LIPO_6CELL[] = {
    25.20f,
    24.90f,
    24.67f,
    24.49f,
    24.14f,
    23.90f,
    23.72f,
    23.48f,
    23.25f,
    23.13f,
    23.01f,
    22.89f,
    22.77f,
    22.72f,
    22.60f,
    22.48f,
    22.36f,
    22.24f,
    22.12f,
    21.65f,
    19.64f,
};


/**
 * Return battery level in percent from the voltage 0-100
 */
static inline uint battery_level(float voltage)
{   
    uint level = 100u;
    float prev = BATTERY_TYPE[0];
    if (voltage>prev)
        return level;
    if (voltage<BATTERY_TYPE[count_of(BATTERY_TYPE)-1])
        return 0u;
    for (auto pvoltage : BATTERY_TYPE) {
        if (voltage>=pvoltage) {
            level+= (voltage-pvoltage)*BATTERY_LEVEL_INCREMENT/(prev-pvoltage);
            break;
        }
        prev = pvoltage;
        level -= BATTERY_LEVEL_INCREMENT;
    }
    return level;
}

static inline bool battery_critical(float voltage)
{
    return voltage<= BATTERY_TYPE[BATTERY_CRITICAL_INDEX];
}

static constexpr inline float battery_max()
{
    return BATTERY_TYPE[0];
}