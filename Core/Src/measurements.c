//
// Created by Matej Planinšek on 01/08/2023.
//
#include "measurements.h"

/**
 * @brief measures num_samples on channel, sends data to UART
 * Implemented for testing purposes as a first measure&send function
 * @param channel channel to measure
 * @param num_samples number of samples to measure

 */
void meas_basic_volt_test_single_ch(uint8_t channel, uint32_t num_samples){
  uint32_t t1, t2;

  //save starting time
  t1 = usec_get_timestamp();


}