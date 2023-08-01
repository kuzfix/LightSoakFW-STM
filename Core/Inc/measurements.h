//
// Created by Matej Planinšek on 01/08/2023.
//

/**
 * @brief A set of high-level measurement functions
 * - handles setup, data acquisition, data processing, and sending to UART

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


void measurements_init(void);

void meas_basic_volt_test_single_ch(uint8_t channel, uint32_t num_samples);


#endif //LIGHTSOAKFW_STM_MEASUREMENTS_H
