//
// Created by Matej Planinšek on 19/07/2023.
//

#ifndef LIGHTSOAKFW_STM_LED_CONTROL_H
#define LIGHTSOAKFW_STM_LED_CONTROL_H

#include "stm32g4xx_hal.h"
#include "dac.h"
#include "main.h"
#include "debug.h"

#define LEDCTRL_DAC_HANDLE &hdac1
#define LEDCTRL_DAC_CH DAC_CHANNEL_1
#define LEDCTRL_DAC_MAX_RAW 4095
#define LEDCTRL_VREF 3.0f
#define LEDCTRL_ZERO_CURRENT_CTRL 0.0927f
//current gain in V/A starting from 92.7mV offset
#define LEDCTRL_CURRENT_GAIN 1.88f
#define LED_CURRENT_MAX 1.5f

void ledctrl_init(void);
void ledctrl_set_dac_raw(uint32_t dac_value);
uint32_t ledctrl_get_raw_from_current(float current);
void ledctrl_set_current(float current);

float ledctrl_get_temperature(void);
float ledctrl_set_illumination(float illumination);


#endif //LIGHTSOAKFW_STM_LED_CONTROL_H
