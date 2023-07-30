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

#ifndef LIGHTSOAKFW_STM_DAQ_H
#define LIGHTSOAKFW_STM_DAQ_H

#include "stm32g4xx_hal.h"

//buffer size. buffSize = 6CH x numSamplesInBuffer
// MUST BE MULTIPLE OF 6 !!!
//same number used for voltage and current to allow for concurrent sampling
#define DAQ_BUFF_SIZE 6000
#define SAMPLE_TIMER_HANDLE &htim20

// structure typedef for one sample of all channels
//can be voltage or current (raw adc data)
typedef struct {
    uint16_t ch1;
    uint16_t ch2;
    uint16_t ch3;
    uint16_t ch4;
    uint16_t ch5;
    uint16_t ch6;
} t_daq_sample_raw;

// structure typedef for one sample of all channels - converted to voltage or current
//can be voltage or current in actual units (mV or mA) todo: decide on units
typedef struct {
    float ch1;
    float ch2;
    float ch3;
    float ch4;
    float ch5;
    float ch6;
} t_daq_sample_convd;


//externs
extern volatile uint16_t daq_buffer_volt[DAQ_BUFF_SIZE];
extern volatile uint16_t daq_buffer_curr[DAQ_BUFF_SIZE];
extern volatile uint8_t daq_sampling_done_volt;
extern volatile uint8_t daq_sampling_done_curr;


//general control functions
void daq_init(void);
void daq_prepare_for_sampling(uint32_t num_samples);
void daq_start_sampling(void);
void daq_is_sampling_done(void);
void daq_start_sample_timer(void);
void daq_stop_sample_timer(void);
void daq_reset_sample_timer(void);

//getting raw data from buffer indexed by sample number (for all channels)
t_daq_sample_raw daq_get_from_buffer_volt(uint32_t sample_idx);
t_daq_sample_raw daq_get_from_buffer_curr(uint32_t sample_idx);


//calculating average of raw data from buffer (for all channels)
t_daq_sample_raw daq_volt_raw_get_average(uint32_t num_samples);
t_daq_sample_raw daq_curr_raw_get_average(uint32_t num_samples);



// raw to voltage and current conversions (for all channels). Should handle calibration and corrections
t_daq_sample_convd daq_raw_to_volt(t_daq_sample_raw raw);
t_daq_sample_convd daq_raw_to_curr(t_daq_sample_raw raw); //todo: add enum for current ranges

//single shot measurement of voltage and current (for all channels), with specified number of samples averaged
t_daq_sample_convd daq_single_shot_volt(uint32_t num_samples);
t_daq_sample_convd daq_single_shot_curr(uint32_t num_samples);











//debug
//getting raw data from buffer indexed by sample number and channel (mostly for testing)
uint16_t daq_get_single_from_buffer_volt(uint8_t ch, uint32_t sample_idx);
uint16_t daq_get_single_from_buffer_curr(uint8_t ch, uint32_t sample_idx);



#endif //LIGHTSOAKFW_STM_DAQ_H
