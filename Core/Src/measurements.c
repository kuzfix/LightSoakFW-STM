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
  printf("YEEET\n");
  //sent start of sampling timestamp
  //todo: this printf of timestamp not working if close to 1000 samples take. WTF???
  splstrt = daq_get_sampling_start_timestamp();
  printf("T:%llu\n", splstrt);
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
    printf("V:%f\n", volt);
  }
  //end sequence
  printf("YOOOT\n");


  t2 = usec_get_timestamp();
  dbg(Debug, "Meas&send end. Took: %lu us\n", t2-t1);

}