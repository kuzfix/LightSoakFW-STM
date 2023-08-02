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
 * @brief prints voltage sample to main serial in human readable format. for internal use
 * - use 0 to print all channels
 * @param sample sample to print
 * @param channel channel to measure
 */
void prv_meas_print_volt(t_daq_sample_convd sample, uint8_t channel){
  switch(channel){
    case 0:
      SCI_printf("VOLT[V]:\n");
      SCI_printf("CH1:%f\n", sample.ch1);
      SCI_printf("CH2:%f\n", sample.ch2);
      SCI_printf("CH3:%f\n", sample.ch3);
      SCI_printf("CH4:%f\n", sample.ch4);
      SCI_printf("CH5:%f\n", sample.ch5);
      SCI_printf("CH6:%f\n", sample.ch6);
      SCI_printf("TIME:%llu YY\n", sample.timestamp);
      break;
    case 1:
      SCI_printf("VOLT[V]:\nCH1:%f\nTIME:%llu\n", sample.ch1, sample.timestamp);
      break;
    case 2:
      SCI_printf("VOLT[V]:\nCH2:%f\nTIME:%llu\n", sample.ch2, sample.timestamp);
      break;
    case 3:
      SCI_printf("VOLT[V]:\nCH3:%f\nTIME:%llu\n", sample.ch3, sample.timestamp);
      break;
    case 4:
      SCI_printf("VOLT[V]:\nCH4:%f\nTIME:%llu\n", sample.ch4, sample.timestamp);
      break;
    case 5:
      SCI_printf("VOLT[V]:\nCH5:%f\nTIME:%llu\n", sample.ch5, sample.timestamp);
      break;
    case 6:
      SCI_printf("VOLT[V]:\nCH6:%f\nTIME:%llu\n", sample.ch6, sample.timestamp);
      break;
    default:
      break;
  }
}

/**
 * @brief prints voltage sample to main serial in human readable format. for internal use
 * - use 0 to print all channels
 * @param sample sample to print
 * @param channel channel to measure
 */
void prv_meas_print_curr(t_daq_sample_convd sample, uint8_t channel){
  switch(channel){
    case 0:
      SCI_printf("CURR[mA]:\nCH1:%f\nCH2:%f\nCH3:%f\nCH4:%f\nCH5:%f\nCH6:%f\nTIME:%llu\n",
                 sample.ch1,sample.ch2, sample.ch3, sample.ch4, sample.ch5, sample.ch6, sample.timestamp);
      break;
    case 1:
      SCI_printf("CURR[mA]:\nCH1:%f\nTIME:%llu\n", sample.ch1, sample.timestamp);
      break;
    case 2:
      SCI_printf("CURR[mA]:\nCH2:%f\nTIME:%llu\n", sample.ch2, sample.timestamp);
      break;
    case 3:
      SCI_printf("CURR[mA]:\nCH3:%f\nTIME:%llu\n", sample.ch3, sample.timestamp);
      break;
    case 4:
      SCI_printf("CURR[mA]:\nCH4:%f\nTIME:%llu\n", sample.ch4, sample.timestamp);
      break;
    case 5:
      SCI_printf("CURR[mA]:\nCH5:%f\nTIME:%llu\n", sample.ch5, sample.timestamp);
      break;
    case 6:
      SCI_printf("CURR[mA]:\nCH6:%f\nTIME:%llu\n", sample.ch6, sample.timestamp);
      break;
  }
}

/**
 * @brief Measures voltage on one or all (param=0) channels. Prints to main serial
 * @param channel channel to measure
 */
void meas_get_voltage(uint8_t channel){
  t_daq_sample_convd meas;
  uint32_t t1, t2;
  t1 = usec_get_timestamp();
  dbg(Debug, "MEAS:get_voltage()\n");

  assert_param(channel <= 6);
  meas = daq_single_shot_volt(prv_meas_num_avg);
  //check if out of range
  meas_check_out_of_rng_volt(meas, channel);
  //print sample
  prv_meas_print_volt(meas, channel);

  t2 = usec_get_timestamp();
  dbg(Debug, "meas_get_voltage() took: %lu usec\n", t2-t1);
}

/**
 * @brief Measures current on one or all (param=0) channels. Prints to main serial
 * Not that usefull to measure jut current - more for testing
 * - does shunt autoranging
 * - Enabling current and setting the voltage setpoint should be done before this.
 * This function just autoranges and measures current
 * @param channel channel to measure
 */
void meas_get_current(uint8_t channel){
  //todo: change delays to RTOS delyas
  t_daq_sample_convd meas;
  uint8_t shunts_switched;
  uint32_t t1, t2;

  t1 = usec_get_timestamp();

  dbg(Debug, "MEAS:get_current()\n");
  //check channel validity
  assert_param(channel <= 6);

  // ######## START AT SHUNT 1X ########

  //set all shunts to 1x
  for(uint8_t i=1 ; i<=DAQ_NUM_CH ; i++){
    fec_set_shunt_1x(i);
  }
  //wait to settle
  usec_delay(SHUNT_SWITCH_SETTLING_TIME);

  //measure currents
  meas = daq_single_shot_curr_no_autorng(prv_meas_num_avg);

  shunts_switched = 0;
  //check if any shunt needs to be switched to 10x
  if(meas.ch1 < FEC_SHNT_1X_LOWTHR){
    fec_set_shunt_10x(1);
    shunts_switched++;
  }
  if(meas.ch2 < FEC_SHNT_1X_LOWTHR){
    fec_set_shunt_10x(2);
    shunts_switched++;
  }
  if(meas.ch3 < FEC_SHNT_1X_LOWTHR){
    fec_set_shunt_10x(3);
    shunts_switched++;
  }
  if(meas.ch4 < FEC_SHNT_1X_LOWTHR){
    fec_set_shunt_10x(4);
    shunts_switched++;
  }
  if(meas.ch5 < FEC_SHNT_1X_LOWTHR){
    fec_set_shunt_10x(5);
    shunts_switched++;
  }
  if(meas.ch6 < FEC_SHNT_1X_LOWTHR){
    fec_set_shunt_10x(6);
    shunts_switched++;
  }

  //switched any shunts?
  if(shunts_switched == 0){
    //no shunts switched - all currents are in range
    //print sample
    meas_check_out_of_rng_curr(meas, channel);
    prv_meas_print_curr(meas, channel);
    fec_report_shunt_ranges_dbg();
    t2 = usec_get_timestamp();
    dbg(Debug, "meas_get_current() took: %d usec\n", t2-t1);
    //done
    return;
  }

  // ######## SOME SHUNTS WERE SWITCHED TO 10X ########

  //shunts were switched, wait to settle and measure again
  usec_delay(SHUNT_SWITCH_SETTLING_TIME);
  meas = daq_single_shot_curr_no_autorng(prv_meas_num_avg);

  shunts_switched = 0;
  //check if any shunt needs to be switched to 10x
  if(meas.ch1 < FEC_SHNT_10X_LOWTHR){
    fec_set_shunt_100x(1);
    shunts_switched++;
  }
  if(meas.ch2 < FEC_SHNT_10X_LOWTHR){
    fec_set_shunt_100x(2);
    shunts_switched++;
  }
  if(meas.ch3 < FEC_SHNT_10X_LOWTHR){
    fec_set_shunt_100x(3);
    shunts_switched++;
  }
  if(meas.ch4 < FEC_SHNT_10X_LOWTHR){
    fec_set_shunt_100x(4);
    shunts_switched++;
  }
  if(meas.ch5 < FEC_SHNT_10X_LOWTHR){
    fec_set_shunt_100x(5);
    shunts_switched++;
  }
  if(meas.ch6 < FEC_SHNT_10X_LOWTHR){
    fec_set_shunt_100x(6);
    shunts_switched++;
  }

  //switched any shunts?
  if(shunts_switched == 0){
    //no shunts switched - all currents are in range
    //print sample
    meas_check_out_of_rng_curr(meas, channel);
    prv_meas_print_curr(meas, channel);
    fec_report_shunt_ranges_dbg();
    t2 = usec_get_timestamp();
    dbg(Debug, "meas_get_current() took: %d usec\n", t2-t1);
    //done
    return;
  }

  // ######## SOME SHUNTS WERE SWITCHED TO 100X ########

  //shunts were switched, wait to settle and measure again
  usec_delay(SHUNT_SWITCH_SETTLING_TIME);
  meas = daq_single_shot_curr_no_autorng(prv_meas_num_avg);

  shunts_switched = 0;
  //check if any shunt needs to be switched to 10x
  if(meas.ch1 < FEC_SHNT_100X_LOWTHR){
    fec_set_shunt_1000x(1);
    shunts_switched++;
  }
  if(meas.ch2 < FEC_SHNT_100X_LOWTHR){
    fec_set_shunt_1000x(2);
    shunts_switched++;
  }
  if(meas.ch3 < FEC_SHNT_100X_LOWTHR){
    fec_set_shunt_1000x(3);
    shunts_switched++;
  }
  if(meas.ch4 < FEC_SHNT_100X_LOWTHR){
    fec_set_shunt_1000x(4);
    shunts_switched++;
  }
  if(meas.ch5 < FEC_SHNT_100X_LOWTHR){
    fec_set_shunt_1000x(5);
    shunts_switched++;
  }
  if(meas.ch6 < FEC_SHNT_100X_LOWTHR){
    fec_set_shunt_1000x(6);
    shunts_switched++;
  }

  //switched any shunts?
  if(shunts_switched == 0){
    //no shunts switched - all currents are in range
    //print sample
    meas_check_out_of_rng_curr(meas, channel);
    prv_meas_print_curr(meas, channel);
    fec_report_shunt_ranges_dbg();
    t2 = usec_get_timestamp();
    dbg(Debug, "meas_get_current() took: %d usec\n", t2-t1);
    //done
    return;
  }

  // ######## SOME SHUNTS WERE SWITCHED TO 1000X ########

  //shunts were switched, wait to settle and measure again
  usec_delay(SHUNT_SWITCH_SETTLING_TIME);
  meas = daq_single_shot_curr_no_autorng(prv_meas_num_avg);

  //we've run out of shunts to switch, print sample
  meas_check_out_of_rng_curr(meas, channel);
  prv_meas_print_curr(meas, channel);

  //report shunt config to debug serial
  fec_report_shunt_ranges_dbg();
  t2 = usec_get_timestamp();
  dbg(Debug, "meas_get_current() took: %d usec\n", t2-t1);

}

/**
 * @brief checks if voltage sample (per channel check) is out of range
 * @param sample sample to check
 */
void meas_check_out_of_rng_volt(t_daq_sample_convd sample, uint8_t channel){

  switch(channel){
    case 0:
      if(sample.ch1 > FEC_VOLT_OVRNG || sample.ch1 < FEC_VOLT_UNDRNG){
        SCI_printf("CH1:VOLT:OUT_OF_RANGE\n");
      }

      if(sample.ch2 > FEC_VOLT_OVRNG || sample.ch2 < FEC_VOLT_UNDRNG){
        SCI_printf("CH2:VOLT:OUT_OF_RANGE\n");
      }

      if(sample.ch3 > FEC_VOLT_OVRNG || sample.ch3 < FEC_VOLT_UNDRNG){
        SCI_printf("CH3:VOLT:OUT_OF_RANGE\n");
      }

      if(sample.ch4 > FEC_VOLT_OVRNG || sample.ch4 < FEC_VOLT_UNDRNG){
        SCI_printf("CH4:VOLT:OUT_OF_RANGE\n");
      }

      if(sample.ch5 > FEC_VOLT_OVRNG || sample.ch5 < FEC_VOLT_UNDRNG){
        SCI_printf("CH5:VOLT:OUT_OF_RANGE\n");
      }

      if(sample.ch6 > FEC_VOLT_OVRNG || sample.ch6 < FEC_VOLT_UNDRNG){
        SCI_printf("CH6:VOLT:OUT_OF_RANGE\n");
      }
      break;

    case 1:
      if(sample.ch1 > FEC_VOLT_OVRNG || sample.ch1 < FEC_VOLT_UNDRNG){
        SCI_printf("CH1:VOLT:OUT_OF_RANGE\n");
      }
      break;

    case 2:
      if(sample.ch2 > FEC_VOLT_OVRNG || sample.ch2 < FEC_VOLT_UNDRNG){
        SCI_printf("CH2:VOLT:OUT_OF_RANGE\n");
      }
      break;

    case 3:
      if(sample.ch3 > FEC_VOLT_OVRNG || sample.ch3 < FEC_VOLT_UNDRNG){
        SCI_printf("CH3:VOLT:OUT_OF_RANGE\n");
      }
      break;

    case 4:
      if(sample.ch4 > FEC_VOLT_OVRNG || sample.ch4 < FEC_VOLT_UNDRNG){
        SCI_printf("CH4:VOLT:OUT_OF_RANGE\n");
      }
      break;

    case 5:
      if(sample.ch5 > FEC_VOLT_OVRNG || sample.ch5 < FEC_VOLT_UNDRNG){
        SCI_printf("CH5:VOLT:OUT_OF_RANGE\n");
      }
      break;

    case 6:
      if(sample.ch6 > FEC_VOLT_OVRNG || sample.ch6 < FEC_VOLT_UNDRNG){
        SCI_printf("CH6:VOLT:OUT_OF_RANGE\n");
      }
      break;

    default:
      break;


  }


}

/**
 * @brief checks if current sample (per channel check) is out of range
 * does not check for propperly selected shunt !!!
 * @param sample sample to check
 */
void meas_check_out_of_rng_curr(t_daq_sample_convd sample, uint8_t channel){
  switch(channel){
    case 0:
      if(sample.ch1 > FEC_CURR_OVRNG || sample.ch1 < FEC_CURR_UNDRNG){
        SCI_printf("CH1:CURR:OUT_OF_RANGE\n");
      }

      if(sample.ch2 > FEC_CURR_OVRNG || sample.ch2 < FEC_CURR_UNDRNG){
        SCI_printf("CH2:CURR:OUT_OF_RANGE\n");
      }

      if(sample.ch3 > FEC_CURR_OVRNG || sample.ch3 < FEC_CURR_UNDRNG){
        SCI_printf("CH3:CURR:OUT_OF_RANGE\n");
      }

      if(sample.ch4 > FEC_CURR_OVRNG || sample.ch4 < FEC_CURR_UNDRNG){
        SCI_printf("CH4:CURR:OUT_OF_RANGE\n");
      }

      if(sample.ch5 > FEC_CURR_OVRNG || sample.ch5 < FEC_CURR_UNDRNG){
        SCI_printf("CH5:CURR:OUT_OF_RANGE\n");
      }

      if(sample.ch6 > FEC_CURR_OVRNG || sample.ch6 < FEC_CURR_UNDRNG){
        SCI_printf("CH6:CURR:OUT_OF_RANGE\n");
      }
      break;

    case 1:
      if(sample.ch1 > FEC_CURR_OVRNG || sample.ch1 < FEC_CURR_UNDRNG){
        SCI_printf("CH1:CURR:OUT_OF_RANGE\n");
      }
      break;

    case 2:
      if(sample.ch2 > FEC_CURR_OVRNG || sample.ch2 < FEC_CURR_UNDRNG){
        SCI_printf("CH2:CURR:OUT_OF_RANGE\n");
      }
      break;

    case 3:
      if(sample.ch3 > FEC_CURR_OVRNG || sample.ch3 < FEC_CURR_UNDRNG){
        SCI_printf("CH3:CURR:OUT_OF_RANGE\n");
      }
      break;

    case 4:
      if(sample.ch4 > FEC_CURR_OVRNG || sample.ch4 < FEC_CURR_UNDRNG){
        SCI_printf("CH4:CURR:OUT_OF_RANGE\n");
      }
      break;

    case 5:
      if(sample.ch5 > FEC_CURR_OVRNG || sample.ch5 < FEC_CURR_UNDRNG){
        SCI_printf("CH5:CURR:OUT_OF_RANGE\n");
      }
      break;

    case 6:
      if(sample.ch6 > FEC_CURR_OVRNG || sample.ch6 < FEC_CURR_UNDRNG){
        SCI_printf("CH6:CURR:OUT_OF_RANGE\n");
      }
      break;

    default:
      break;


  }
}

/**
 * @brief Measures current and voltage at the exact same time. on one or all (param=0) channels. Prints to main serial
 * - autoranges current before making a final measurement
 * does not check for propperly selected shunt !!!
 * @param channel channel to sample. 0 for all channels
 */
void meas_get_voltage_and_current(uint8_t channel){

}

