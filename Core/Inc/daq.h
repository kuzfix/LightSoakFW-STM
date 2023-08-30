//
// Created by Matej Planinšek on 30/07/2023.
//

/**
 * @brief Data acquisition module. Provides functions to control sampling of
 * analog data (Voltage and Current on 6 channels) and
 * read the data from the buffer.
 * - sampling is triggered by timer TIM20 overflow -> TRGO event. Sample time is set by period of TIM20.
 * - Default sampling is all 6 channels at 100kHz / 10us period per ADC.
 * - Sampling is done in DMA mode.
 * - voltage and current are always sampled concurrently.
 *
 */

//todo: handle min opamp output voltages (10mVish)

#ifndef LIGHTSOAKFW_STM_DAQ_H
#define LIGHTSOAKFW_STM_DAQ_H

#include "stm32g4xx_hal.h"
#include "adc.h"
#include "tim.h"
#include "debug.h"
#include "front_end_control.h"
#include "micro_sec.h"

//buffer size. buffSize = 6CH x numSamplesInBuffer
// MUST BE MULTIPLE OF 6 !!!
//same number used for voltage and current to allow for concurrent sampling
#define DAQ_NUM_CH 6
#define DAQ_BUFF_SIZE 12000
#define DAQ_SAMPLE_TIMER_HANDLE &htim20
#define DAQ_SAMPLE_TIMER_PERIOD_100KSPS 1699
#define DAQ_SAMPLE_TIME_100KSPS 10 //us
#define DAQ_VOLT_ADC ADC1
#define DAQ_CURR_ADC ADC3
#define DAQ_VOLT_ADC_HANDLE &hadc1
#define DAQ_CURR_ADC_HANDLE &hadc3

#define DAQ_VREF 3.0f
#define DAQ_MAX_ADC_VAL 0xFFFF
#define DAQ_VOLT_AMP_GAIN_CH1 2.012f
#define DAQ_VOLT_AMP_GAIN_CH2 2.012f
#define DAQ_VOLT_AMP_GAIN_CH3 2.012f
#define DAQ_VOLT_AMP_GAIN_CH4 2.012f
#define DAQ_VOLT_AMP_GAIN_CH5 2.012f
#define DAQ_VOLT_AMP_GAIN_CH6 2.012f

#define DAQ_SHUNT_AMP_GAIN 31.30303030f

#define DAQ_SAMPLE_BITSIHFT 4

//samples to take for autoranging measurements
#define DAQ_AUTORANGE_SAMPLES 16


// structure typedef for one sample of all channels
//can be voltage or current (raw adc data)
typedef struct {
    uint16_t ch1;
    uint16_t ch2;
    uint16_t ch3;
    uint16_t ch4;
    uint16_t ch5;
    uint16_t ch6;
    uint64_t timestamp;
} t_daq_sample_raw;

// structure typedef for one sample of all channels - converted to voltage or current
//can be voltage or current in actual units (mV or mA)
typedef struct {
    float ch1;
    float ch2;
    float ch3;
    float ch4;
    float ch5;
    float ch6;
    uint64_t timestamp;
} t_daq_sample_convd;


//externs
//todo: check if all are needed
extern volatile uint16_t g_daq_buffer_volt[DAQ_BUFF_SIZE];
extern volatile uint16_t g_daq_buffer_curr[DAQ_BUFF_SIZE];
extern volatile uint8_t daq_sampling_done_volt;
extern volatile uint8_t daq_sampling_done_curr;
extern volatile uint64_t daq_sampling_volt_done_timestamp;
extern volatile uint64_t daq_sampling_curr_done_timestamp;


//general control functions
void daq_init(void);
void daq_prepare_for_sampling(uint32_t num_samples);
void daq_start_sampling(void);
uint8_t daq_is_sampling_done(void);
void daq_calibrate_adcs(void);

//getting raw data from buffer indexed by sample number (for all channels)
t_daq_sample_raw daq_get_from_buffer_volt(uint32_t sample_idx);
t_daq_sample_raw daq_get_from_buffer_curr(uint32_t sample_idx);


//calculating average of raw data from buffer (for all channels)
t_daq_sample_raw daq_volt_raw_get_average(uint32_t num_samples);
t_daq_sample_raw daq_curr_raw_get_average(uint32_t num_samples);

float daq_get_from_sample_convd_by_index(t_daq_sample_convd sample, uint8_t ch);

//autorange control
void daq_autorange(void);


// raw to voltage and current conversions (for all channels). Should handle calibration and corrections
//todo: current uses a function from front_end_control do determine shunt resistance. This is not ideal.
t_daq_sample_convd daq_raw_to_volt(t_daq_sample_raw raw);
t_daq_sample_convd daq_raw_to_curr(t_daq_sample_raw raw);

//single shot measurement of voltage and current (for all channels), with specified number of samples averaged
t_daq_sample_convd daq_single_shot_volt(uint32_t num_samples);
//uses currently set shunts, no autoranging!
t_daq_sample_convd daq_single_shot_curr_no_autorng(uint32_t num_samples);

uint64_t daq_get_sampling_start_timestamp(void);











//debug
//getting raw data from buffer indexed by sample number and channel (mostly for testing)
uint16_t daq_get_single_from_buffer_volt(uint8_t ch, uint32_t sample_idx);
uint16_t daq_get_single_from_buffer_curr(uint8_t ch, uint32_t sample_idx);



#endif //LIGHTSOAKFW_STM_DAQ_H
