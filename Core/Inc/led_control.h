//
// Created by Matej Planinšek on 19/07/2023.
//

#ifndef LIGHTSOAKFW_STM_LED_CONTROL_H
#define LIGHTSOAKFW_STM_LED_CONTROL_H

#include "stm32g4xx_hal.h"
#include "dac.h"
#include "main.h"
#include "debug.h"
#include "ds18b20.h"
#include "main_serial.h"
#include "measurements.h"

#define LEDCTRL_DAC_HANDLE &hdac1
#define LEDCTRL_DAC_CH DAC_CHANNEL_1
#define LEDCTRL_DAC_MAX_RAW 4095
#define LEDCTRL_VREF 3.0f
#define LEDCTRL_ZERO_CURRENT_CTRL 0.0927f
//pull down factor: internal DAC output impedance 12k and 1M pulldown resistor make a voltage divider
#define LEDCTRL_PULLDOWN_FACTOR 0.98814f
//current gain in V/A starting from 92.7mV offset
#define LEDCTRL_CURRENT_GAIN 1.88f
#define LED_CURRENT_MAX 1.5f

#define LED_REF_TEMP 25.0f
// LED light output temperature coefficient (unit: ratio change per degree)
#define LED_PER_DEG_TEMP_COEFF (-0.0028f)
//#define LED_PER_DEG_TEMP_COEFF (-0.0f)

#define LEDCTRL_PERIODIC_TEMP_REPORT_MAINSER 0
#define LEDCTRL_TEMP_READ_TIME_US 100000ULL

void ledctrl_init(void);
void ledctrl_set_dac_raw(uint32_t dac_value);
uint32_t ledctrl_get_raw_from_current(float current);
void ledctrl_set_current_tempcomp(float current);
void ledctrl_set_illum(float illum);

float ledctrl_get_temperature(void);
void ledctrl_print_temperature_mainser(void);
float ledctrl_illumination_to_current(float illumination);
float ledctrl_compensate_current_for_temp(float current);

void ledctrl_calibrate_illum_curr(float illum, float current);

void ledctrl_handler(void);


#endif //LIGHTSOAKFW_STM_LED_CONTROL_H
