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

//define number of averaged samples for all measurements here
//todo: do value setting in init
uint32_t prv_meas_num_avg = MEAS_NUM_AVG_DEFAULT;



/**
 * @brief Sets number of samples to take and average for each voltage / current measurement
 * @param num_avg_smpl samples to average

 */
void meas_set_num_avg(uint32_t num_avg_smpl){
  //no paramete validity checking, use wisely
  prv_meas_num_avg = num_avg_smpl;
}

void meas_basic_volt_test_dump_single_ch(uint8_t channel, uint32_t num_samples){
  uint32_t t1, t2;
  static uint64_t splstrt;

  //save starting time
  t1 = usec_get_timestamp();

  //prepare for sampling
  daq_prepare_for_sampling(num_samples);
  //start sampling
  daq_start_sampling();
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //calculate average and report to debug serial
  t_daq_sample_raw avg_raw = daq_volt_raw_get_average(num_samples);
  t_daq_sample_convd avg_conv = daq_raw_to_volt(avg_raw);
  dbg(Debug, "DAQ end. AVGS: %f, %f, %f, %f, %f, %f\n",
      avg_conv.ch1,avg_conv.ch2, avg_conv.ch3, avg_conv.ch4, avg_conv.ch5, avg_conv.ch6);

  //send data to UART. This will be human-readable for testing
  //start sequence
  SCI_printf("YEEET\n");
  //sent start of sampling timestamp
  //todo: this printf of timestamp not working if close to 1000 samples take. WTF???
  splstrt = daq_get_sampling_start_timestamp();
  SCI_printf("T:%llu\n", splstrt);
  //convert raw and send for one channel
  for(uint32_t i = 0; i < num_samples; i++){
    //wait if uart buffer is full
    while(SCI_get_tx_buffer_remaining() < 64);
    //get raw sample
    t_daq_sample_raw raw = daq_get_from_buffer_volt(i);
    //convert to voltage
    t_daq_sample_convd conv = daq_raw_to_volt(raw);
    //select channel
    float volt;
    switch(channel){
      case 1:
        volt = conv.ch1;
        break;
      case 2:
        volt = conv.ch2;
        break;
      case 3:
        volt = conv.ch3;
        break;
      case 4:
        volt = conv.ch4;
        break;
      case 5:
        volt = conv.ch5;
        break;
      case 6:
        volt = conv.ch6;
        break;
    }
    //send
    SCI_printf("V:%f\n", volt);
  }
  //end sequence
  SCI_printf("YOOOT\n");


  t2 = usec_get_timestamp();
  dbg(Debug, "Meas&send end. Took: %lu us\n", t2-t1);
}

/**
 * @brief Measures voltage on one or all (param=0) channels. Prints to main serial
 * @param channel channel to measure
 */
void meas_get_voltage(uint8_t channel){
  dbg(Debug, "MEAS:get_voltage()\n");
  assert_param(channel <= 6);
  t_daq_sample_convd meas = daq_single_shot_volt(prv_meas_num_avg);
  switch(channel){
    case 0:
      SCI_printf("VOLT[V]:\nCH1:%f\nCH2:%f\nCH3:%f\nCH4:%f\nCH5:%f\nCH6:%f\nTIME:%llu\n",
          meas.ch1,meas.ch2, meas.ch3, meas.ch4, meas.ch5, meas.ch6, meas.timestamp);
      break;
    case 1:
      SCI_printf("VOLT[V]:\nCH1:%f\nTIME:%llu\n", meas.ch1, meas.timestamp);
      break;
    case 2:
      SCI_printf("VOLT[V]:\nCH2:%f\nTIME:%llu\n", meas.ch2, meas.timestamp);
      break;
    case 3:
      SCI_printf("VOLT[V]:\nCH3:%f\nTIME:%llu\n", meas.ch3, meas.timestamp);
      break;
    case 4:
      SCI_printf("VOLT[V]:\nCH4:%f\nTIME:%llu\n", meas.ch4, meas.timestamp);
      break;
    case 5:
      SCI_printf("VOLT[V]:\nCH5:%f\nTIME:%llu\n", meas.ch5, meas.timestamp);
      break;
    case 6:
      SCI_printf("VOLT[V]:\nCH6:%f\nTIME:%llu\n", meas.ch6, meas.timestamp);
      break;
  }
}

/**
 * @brief Measures current on one or all (param=0) channels. Prints to main serial
 * Not that usefull to measure jut current - more for testing
 * @param channel channel to measure
 */
void meas_get_current(uint8_t channel){
  //todo: switch current ranges
}