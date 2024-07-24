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
#include "main_serial.h"
#include <math.h>

#define MEAS_NUM_AVG_DEFAULT 64
#define MEAS_NOCONVERGE_REPORT 0

#define MEAS_DUT_SETTLING_TIME_DEFAULT_MS 1 //ms
#define MEAS_FORCE_VOLT_CLOSE_ENOUGH 0.002f //V
#define MEAS_FORCE_VOLT_ITER_MAX 10
//sampling for MEAS_FLASH_DUMP_SAMPLEBORDER_US before and after led is turned on and off
#define MEAS_FLASH_DUMP_SAMPLEBORDER_US 2000

#define NOISE_MEASURE_NUMSAMPLES 2000

#define MEAS_IV_CHAR_MIN_CURR_THR 0.08 //uA
#define MEAS_IV_CHAR_MIN_STEPS_THR 3 //
#define MEAS_IV_CHAR_MIN_DELTA_V 0.002 //V  //PWM ripple up to 0.62mV and step size 1.6 mV

#define MPPT_DURATION 2000  //us
#define MPPT_VOLTAGE_STEP 0.002 //V
#define MPPT_SEARCH_VOLTAGE_STEP 0.10 //V
#define MPPT_SETTLING_TIME_DEFAULT  100000  //us
#define MPPT_ALL_ON   0x3F
#define MPPT_ALL_OFF  0x00
#define MPPT_MAX_START_STEPS  100
#define MPPT_FVOC 0.75  //first guess is Vmpp = 75% of Voc
#define MPPT_DIR_REVERSE_THRESHOLD  4  //When first looking for MPP, MPP is reached when search algorithem reverses direction this many times
#define MPPT_VMAX 1.5   //V
#define MPPT_VMIN -0.1  //V
#define MPPT_IMIN_OF_RANGE 0.01 //Minimum current depends on range. 0.01 is 1% of current range.
#define MPPT_MAX_VOLTAGE_DEFAULT  1.2 //V


//each approach iteration, shunt voltage is calculated and compensated
//for better stability we can compensate a bit less
#define MEAS_FORCE_VOLT_APPROACH_K 1.0f


void measurements_init(void);


//call with 0 for all channels
void meas_volt_sample_and_dump(uint8_t channel, uint32_t num_samples);
void meas_curr_sample_and_dump(uint8_t channel, uint32_t num_samples);
void meas_iv_sample_and_dump(uint8_t channel, uint32_t num_samples);

// set/get averaging number
void meas_set_num_avg(uint32_t num_avg_smpl);
uint32_t meas_get_num_avg(void);

// set/get settling time
void meas_set_settling_time(uint32_t stltm);
uint32_t meas_get_settling_time(void);

// call with 0 for all channels
void meas_get_voltage(uint8_t channel);

// call with 0 for all channels
void meas_get_noise_volt(uint8_t channel);

//for testing purposes
//call with 0 for all channels
void meas_get_current(uint8_t channel);

// call with 0 for all channels
void meas_get_noise_curr(uint8_t channel);


//call with 0 for all channels
void meas_get_voltage_and_current(uint8_t channel);

//only single channel
float meas_get_exact_IV_point(uint8_t channel, float voltage, uint8_t disable_current_when_finished, uint8_t noident);

void autorange_IV_point(uint8_t channel, float voltage, uint32_t settling_time, t_daq_sample_convd* convd_volt, t_daq_sample_convd* convd_curr);
void meas_stepV_for_IV_point(uint8_t channel, uint8_t channel_mask, float voltage, t_daq_sample_convd* convd_volt, t_daq_sample_convd* convd_curr);
//Return results through the two pointers.
void meas_get_IV_point(uint8_t channel, uint8_t channel_mask, uint32_t settle_time_us, t_daq_sample_convd* convd_volt, t_daq_sample_convd* convd_curr);

//only single channel
void meas_get_iv_characteristic(uint8_t channel, float start_volt, float end_volt, float step_volt, uint32_t step_time, uint32_t Npoints_per_step);

//flash measurements (measures Vf as quickly as possible)
//call with 0 for all channels
void meas_flashmeasure_singlesample(uint8_t channel, float illum, uint32_t flash_dur_us, uint32_t measure_at_us, uint32_t numavg);
void meas_flashmeasure_dumpbuffer(uint8_t channel, float illum, uint32_t flash_dur_us);


//checks sample for over/under range, reports to main serial
void meas_check_out_of_rng_volt(t_daq_sample_convd sample, uint8_t channel);
void meas_check_out_of_rng_curr(t_daq_sample_convd sample, uint8_t channel);


void meas_end_of_sequence(void);

void mppt_start(uint8_t channel, uint32_t settling_time, uint32_t report_every_xth_point);
void mppt_resume();
void mppt_stop();
void mppt();


//sample printing to main serial functions
void prv_meas_print_timestamp(uint64_t timestamp);
void prv_meas_print_ch_ident(uint8_t channel, uint8_t sample_timestamp);
void prv_meas_print_ch_ident_by_mask(uint8_t channel_mask, uint8_t sample_timestamp);
void prv_meas_print_data_ident_voltage(void);
void prv_meas_print_data_ident_current(void);
void prv_meas_print_data_ident_IV_point(void);
void prv_meas_print_data_ident_dump_text_volt(void);
void prv_meas_print_data_ident_dump_text_curr(void);
void prv_meas_print_data_ident_dump_text_IV(void);
void prv_meas_print_data_ident_IV_characteristic(void);
void prv_meas_print_data_ident_MPP(void);
void prv_meas_print_data_ident_flashmeasure_single(void);
void prv_meas_print_data_ident_flashmeasure_dump(void);
void prv_meas_print_dump_end(void);
void prv_meas_print_sample(t_daq_sample_convd sample, uint8_t channel);
void prv_meas_print_IV_point_ts(t_daq_sample_convd sample_volt, t_daq_sample_convd sample_curr, uint8_t channel, uint8_t channel_mask);
void prv_meas_print_IV_point(t_daq_sample_convd sample_volt, t_daq_sample_convd sample_curr, uint8_t channel);
void prv_meas_print_mpp(uint8_t channel_mask, t_daq_sample_convd *sample_volt, t_daq_sample_convd *sample_curr);
void prv_meas_dump_from_buffer_human_readable_volt(uint8_t channel, uint32_t num_samples);
void prv_meas_dump_from_buffer_human_readable_curr(uint8_t channel, uint32_t num_samples);
void prv_meas_dump_from_buffer_human_readable_iv(uint8_t channel, uint32_t num_samples);


//parameter structures for all functions that can be scheduled
//meas_get_voltage
typedef struct{
  uint8_t channel;
  uint32_t settling_time;
  uint32_t report_every_xth_point;
} mppt_param_t;

//meas_get_voltage
typedef struct{
    uint8_t channel;
} meas_get_voltage_param_t;

//meas_get_current
typedef struct{
    uint8_t channel;
} meas_get_current_param_t;

//meas_get_IV_point
typedef struct{
    uint8_t channel;
    float voltage;
    uint8_t disable_current_when_finished;
    uint8_t noident;
} meas_get_IV_point_param_t;

//meas_get_iv_characteristic
typedef struct{
    uint8_t channel;
    float start_volt;
    float end_volt;
    float step_volt;
    uint32_t step_time;
    uint32_t Npoints_per_step;
} meas_get_iv_characteristic_param_t;

//meas_##_sample_and_dump (for v i and iv)
typedef struct{
    uint8_t channel;
    uint32_t num_samples;
} meas_sample_and_dump_param_t;

//ledctrl_set_current
typedef struct{
    float current;
} ledctrl_set_current_param_t;

//ledctrl_set_illum
typedef struct{
    float illum;
} ledctrl_set_illum_param_t;

//meas_flashmeasure_dumpbuffer
typedef struct{
    uint8_t channel;
    float illum;
    uint32_t flash_dur_us;
} meas_flashmeasure_dumpbuffer_param_t;

//meas_flashmeasure_singlesample
typedef struct{
    uint8_t channel;
    float illum;
    uint32_t flash_dur_us;
    uint32_t measure_at_us;
    uint32_t numavg;
} meas_flashmeasure_singlesample_param_t;

//fec_enable/disable_current
typedef struct{
    uint8_t channel;
} fec_enable_disable_current_param_t;

// setshunt
typedef struct{
    uint8_t channel;
    uint32_t shunt;
} fec_setshunt_param_t;

// setforcevolt
typedef struct{
    uint8_t channel;
    float volt;
} fec_setforcevolt_param_t;

// calibillum
typedef struct{
    float illum;
    float curr;
} ledctrl_calibillum_param_t;

// setnumavg
typedef struct{
    uint32_t numavg;
} meas_set_num_avg_param_t;

// set settle time
typedef struct{
    uint32_t settle_time;
} meas_set_stltm_param_t;

//meas_get_noise
typedef struct{
    uint8_t channel;
    uint8_t volt_flag;
    uint8_t curr_flag;
} meas_get_noise_param_t;



//typedef enum for cmd ids. IDs needed for cmd scheduling
typedef enum {
    mppt_start_id,
    mppt_resume_id,
    mppt_stop_id,
    meas_get_voltage_id,
    meas_get_current_id,
    meas_get_IV_point_id,
    meas_get_iv_characteristic_id,
    meas_volt_sample_and_dump_id,
    meas_curr_sample_and_dump_id,
    meas_iv_sample_and_dump_id,
    ledctrl_set_current_id,
    ledctrl_set_illum_id,
    meas_flashmeasure_dumpbuffer_id,
    meas_flashmeasure_singlesample_id,
    end_of_sequence_id,
    fec_enable_current_id,
    fec_disable_current_id,
    setshunt_id,
    setforcevolt_id,
    autorange_id,
    getledtemp_id,
    calibillum_id,
    meas_set_numavg_id,
    meas_get_numavg_id,
    meas_set_settle_time_id,
    meas_get_settle_time_id,
    meas_get_noise_id
} meas_funct_id;



#endif //LIGHTSOAKFW_STM_MEASUREMENTS_H
