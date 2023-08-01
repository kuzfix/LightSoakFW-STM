//
// Created by Matej Planinšek on 01/08/2023.
//

/**
 * @brief A set of high-level measurement functions
 * - handles setup, data acquisition, data processing, and sending to UART
 * these functions will be called by the CLI interface

 */

#ifndef LIGHTSOAKFW_STM_MEASUREMENTS_H
#define LIGHTSOAKFW_STM_MEASUREMENTS_H

#include "stm32g4xx_hal.h"
#include "debug.h"
#include "front_end_control.h"
#include "micro_sec.h"
#include "daq.h"
#include "led_control.h"
#include "SCI.h"

#define MEAS_NUM_AVG_DEFAULT 100


void measurements_init(void);

void meas_basic_volt_test_dump_single_ch(uint8_t channel, uint32_t num_samples);

//set averaging number
void meas_set_num_avg(uint32_t num_avg_smpl);

//all functions have number of averaging samples as argument. Can be fast and noisy or slow and exact
//these averages might be fixed or CLI parameter
void meas_get_voltage_single_ch(uint8_t channel);
void meas_get_voltage_all_ch(uint32_t num_avg_smpl);

void meas_get_current_single_ch(uint8_t channel);
void meas_get_current_all_ch(uint32_t num_avg_smpl);


#endif //LIGHTSOAKFW_STM_MEASUREMENTS_H
