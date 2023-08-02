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

// set/get averaging number
void meas_set_num_avg(uint32_t num_avg_smpl);
uint32_t meas_get_num_avg(void);

// call with 0 for all channels
void meas_get_voltage(uint8_t channel);

//for testing purposes
//call with 0 for all channels
void meas_get_current(uint8_t channel);


//call with 0 for all channels
void meas_get_voltage_and_current(uint8_t channel);

//call with 0 for all channels
void meas_get_current_at_voltage(uint8_t channel, float voltage);

//call with 0 for all channels
void meas_get_iv_characteristic(uint8_t channel, float start_volt, float end_volt, float step_volt);

//checks sample for over/under range, reports to main serial
void meas_check_out_of_rng_volt(t_daq_sample_convd sample, uint8_t channel);
void meas_check_out_of_rng_curr(t_daq_sample_convd sample, uint8_t channel);


void prv_meas_print_volt(t_daq_sample_convd sample, uint8_t channel);
void prv_meas_print_curr(t_daq_sample_convd sample, uint8_t channel);


#endif //LIGHTSOAKFW_STM_MEASUREMENTS_H
