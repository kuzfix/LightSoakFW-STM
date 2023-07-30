//
// Created by Matej Planinšek on 30/07/2023.
//

#include "daq.h"

// voltage ADC buffer
volatile uint16_t daq_buffer_volt[DAQ_BUFF_SIZE];
volatile uint16_t daq_buffer_curr[DAQ_BUFF_SIZE];

// sampling done flags
volatile uint8_t daq_sampling_done_volt;
volatile uint8_t adc_sampling_done_curr;