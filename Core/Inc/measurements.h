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

#define MEAS_NUM_AVG_DEFAULT 32

#define MEAS_DUT_SETTLING_TIME_MS 1 //ms
#define MEAS_FORCE_VOLT_CLOSE_ENOUGH 0.002f //V
#define MEAS_FORCE_VOLT_ITER_MAX 10

//each approach iteration, shunt voltage is calculated and compensated
//for better stability we can compensate a bit less
#define MEAS_FORCE_VOLT_APPROACH_K 1.0f


void measurements_init(void);

void meas_basic_volt_test_dump_single_ch(uint8_t channel, uint32_t num_samples);

//call with 0 for all channels
void meas_volt_sample_and_dump(uint8_t channel, uint32_t num_samples);

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

//only single channel
void meas_get_IV_point(uint8_t channel, float voltage, uint8_t disable_current_when_finished, uint8_t noident);

//call with 0 for all channels
void meas_get_iv_characteristic(uint8_t channel, float start_volt, float end_volt, float step_volt);

//checks sample for over/under range, reports to main serial
void meas_check_out_of_rng_volt(t_daq_sample_convd sample, uint8_t channel);
void meas_check_out_of_rng_curr(t_daq_sample_convd sample, uint8_t channel);


//sample printing to main serial functions
void prv_meas_print_timestamp(uint64_t timestamp);
void prv_meas_print_ch_ident(uint8_t channel);
void prv_meas_print_data_ident_voltage(void);
void prv_meas_print_data_ident_current(void);
void prv_meas_print_data_ident_IV_point(void);
void prv_meas_print_data_ident_dump_text_volt(void);
void prv_meas_print_data_ident_dump_text_curr(void);
void prv_meas_print_data_ident_dump_text_IV(void);
void prv_meas_print_data_ident_IV_characteristic(void);
void prv_meas_print_sample(t_daq_sample_convd sample, uint8_t channel);
void prv_meas_print_IV_point(t_daq_sample_convd sample_volt, t_daq_sample_convd sample_curr, uint8_t channel);
void prv_meas_dump_from_buffer_human_readable_volt(uint8_t channel, uint32_t num_samples);
void prv_meas_dump_from_buffer_human_readable_curr(uint8_t channel, uint32_t num_samples);
void prv_meas_dump_from_buffer_human_readable_IV(uint8_t channel, uint32_t num_samples);


#endif //LIGHTSOAKFW_STM_MEASUREMENTS_H
