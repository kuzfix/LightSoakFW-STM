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

/**
 * @brief Gets number of samples to take and average for each voltage / current measurement
 * @return num_avg_smpl samples used to average
 */
uint32_t meas_get_num_avg(void){
  return prv_meas_num_avg;
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
  //todo: this printf of timestamp not working if close to 1000 samples take. WTF??? maybe? maybe not a prooblem anymore?
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
void prv_meas_print_sample(t_daq_sample_convd sample, uint8_t channel){
  switch(channel){
    case 0:
      SCI_printf("%f:%f:%f:%f:%f:%f\n", sample.ch1,sample.ch2, sample.ch3, sample.ch4, sample.ch5, sample.ch6);
      break;
    case 1:
      SCI_printf("%f\n", sample.ch1);
      break;
    case 2:
      SCI_printf("%f\n", sample.ch2);
      break;
    case 3:
      SCI_printf("%f\n", sample.ch3);
      break;
    case 4:
      SCI_printf("%f\n", sample.ch4);
      break;
    case 5:
      SCI_printf("%f\n", sample.ch5);
      break;
    case 6:
      SCI_printf("%f\n", sample.ch6);
      break;
    default:
      break;
  }
}


/**
 * @brief prints voltage and current to main serial in human readable format. for internal use
 * - use 0 to print all channels
 * @param sample_volt voltage to print
 * @param sample_curr current to print
 * @param channel channel to measure
 */
void prv_meas_print_volt_and_curr(t_daq_sample_convd sample_volt, t_daq_sample_convd sample_curr, uint8_t channel){
  //voltage and current should have same timestamp if we want to print them in one go. Timestamp for voltage is used.
  //warning if timestamps not equal
  if(sample_volt.timestamp != sample_curr.timestamp){
    dbg(Warning,"prv_meas_print_volt_and_curr(): Volt/Curr timestamps not equal!\n");
  }

  switch(channel){
    case 0:
      SCI_printf("VOLT[V]:\n");
      SCI_printf("CH1:%f\n", sample_volt.ch1);
      SCI_printf("CH2:%f\n", sample_volt.ch2);
      SCI_printf("CH3:%f\n", sample_volt.ch3);
      SCI_printf("CH4:%f\n", sample_volt.ch4);
      SCI_printf("CH5:%f\n", sample_volt.ch5);
      SCI_printf("CH6:%f\n", sample_volt.ch6);
      SCI_printf("CURR[mA]:\n");
      SCI_printf("CH1:%f\n", sample_curr.ch1);
      SCI_printf("CH2:%f\n", sample_curr.ch2);
      SCI_printf("CH3:%f\n", sample_curr.ch3);
      SCI_printf("CH4:%f\n", sample_curr.ch4);
      SCI_printf("CH5:%f\n", sample_curr.ch5);
      SCI_printf("CH6:%f\n", sample_curr.ch6);
      SCI_printf("TIME:%llu\n", sample_volt.timestamp);
      break;
    case 1:
      SCI_printf("VOLT[V]:\nCH1:%f\n", sample_volt.ch1);
      SCI_printf("CURR[mA]:\nCH1:%f\n", sample_curr.ch1);
      SCI_printf("TIME:%llu\n", sample_volt.timestamp);
      break;
    case 2:
      SCI_printf("VOLT[V]:\nCH2:%f\n", sample_volt.ch2);
      SCI_printf("CURR[mA]:\nCH2:%f\n", sample_curr.ch2);
      SCI_printf("TIME:%llu\n", sample_volt.timestamp);
      break;
    case 3:
      SCI_printf("VOLT[V]:\nCH3:%f\n", sample_volt.ch3);
      SCI_printf("CURR[mA]:\nCH3:%f\n", sample_curr.ch3);
      SCI_printf("TIME:%llu\n", sample_volt.timestamp);
      break;
    case 4:
      SCI_printf("VOLT[V]:\nCH4:%f\n", sample_volt.ch4);
      SCI_printf("CURR[mA]:\nCH4:%f\n", sample_curr.ch4);
      SCI_printf("TIME:%llu\n", sample_volt.timestamp);
      break;
    case 5:
      SCI_printf("VOLT[V]:\nCH5:%f\n", sample_volt.ch5);
      SCI_printf("CURR[mA]:\nCH5:%f\n", sample_curr.ch5);
      SCI_printf("TIME:%llu\n", sample_volt.timestamp);
      break;
    case 6:
      SCI_printf("VOLT[V]:\nCH6:%f\n", sample_volt.ch6);
      SCI_printf("CURR[mA]:\nCH6:%f\n", sample_curr.ch6);
      SCI_printf("TIME:%llu\n", sample_volt.timestamp);
      break;
    default:
      break;
  }
}

/**
 * @brief prints voltage and current to main serial in human readable format - as a IV curve point. for internal use
 * - use 0 to print all channels
 * @param sample_volt voltage to print
 * @param sample_curr current to print
 * @param channel channel to measure
 */
void prv_meas_print_IV_point(t_daq_sample_convd sample_volt, t_daq_sample_convd sample_curr, uint8_t channel){
  //voltage and current should have same timestamp if we want to print them in one go. Timestamp for voltage is used.
  //warning if timestamps not equal
  if(sample_volt.timestamp != sample_curr.timestamp){
    dbg(Warning,"prv_meas_print_volt_and_curr(): Volt/Curr timestamps not equal!\n");

  }

  switch(channel){
    case 0:
      SCI_printf("%f_%f:", sample_curr.ch1, sample_volt.ch1);
      SCI_printf("%f_%f:", sample_curr.ch2, sample_volt.ch2);
      SCI_printf("%f_%f:", sample_curr.ch3, sample_volt.ch3);
      SCI_printf("%f_%f:", sample_curr.ch4, sample_volt.ch4);
      SCI_printf("%f_%f:", sample_curr.ch5, sample_volt.ch5);
      SCI_printf("%f_%f\n", sample_curr.ch6, sample_volt.ch6);
      break;
    case 1:
      SCI_printf("%f_%f\n", sample_curr.ch1, sample_volt.ch1);
      break;
    case 2:
      SCI_printf("%f_%f\n", sample_curr.ch2, sample_volt.ch2);
      break;
    case 3:
      SCI_printf("%f_%f\n", sample_curr.ch3, sample_volt.ch3);
      break;
    case 4:
      SCI_printf("%f_%f\n", sample_curr.ch4, sample_volt.ch4);
      break;
    case 5:
      SCI_printf("I:%f_V:%f\n", sample_curr.ch5, sample_volt.ch5);
      break;
    case 6:
      SCI_printf("I:%f_V:%f\n", sample_curr.ch6, sample_volt.ch6);
      break;
    default:
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
  prv_meas_print_data_ident_voltage();
  prv_meas_print_ch_ident(channel);
  prv_meas_print_timestamp(meas.timestamp);
  prv_meas_print_sample(meas, channel);

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
  t_daq_sample_convd meas;
  uint32_t t1, t2;

  t1= usec_get_timestamp();

  dbg(Debug, "MEAS:meas_get_current()\n");

  //autorange shunts
  daq_autorange();

  //measure and print
  meas = daq_single_shot_curr_no_autorng(prv_meas_num_avg);
  meas_check_out_of_rng_curr(meas, channel);


  //print sample
  prv_meas_print_data_ident_current();
  prv_meas_print_ch_ident(channel);
  prv_meas_print_timestamp(meas.timestamp);
  prv_meas_print_sample(meas, channel);

  //report shunt config to debug serial
  t2 = usec_get_timestamp();
  dbg(Debug, "meas_get_current() took: %lu usec\n", t2-t1);

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
 * - does not manipulate enable current and voltage force setpoint
 * does not check for propperly selected shunt !!!
 * @param channel channel to sample. 0 for all channels
 */
void meas_get_voltage_and_current(uint8_t channel){
  t_daq_sample_raw raw_volt, raw_curr;
  t_daq_sample_convd convd_volt, convd_curr;
  uint32_t t1, t2;

  t1 = usec_get_timestamp();

  dbg(Debug, "MEAS:meas_get_voltage_and_current()\n");

  //autorange shunts
  daq_autorange();
  //prepare for sampling
  daq_prepare_for_sampling(MEAS_NUM_AVG_DEFAULT);
  //start sampling
  daq_start_sampling();
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //get raw averages from buffer
  raw_volt = daq_volt_raw_get_average(MEAS_NUM_AVG_DEFAULT);
  raw_curr = daq_curr_raw_get_average(MEAS_NUM_AVG_DEFAULT);
  //convert to volts and amps
  convd_volt = daq_raw_to_volt(raw_volt);
  convd_curr = daq_raw_to_curr(raw_curr);
  //check for overrange
  meas_check_out_of_rng_volt(convd_volt, channel);
  meas_check_out_of_rng_curr(convd_curr, channel);


  //print voltage and current
  prv_meas_print_data_ident_IV_point();
  prv_meas_print_ch_ident(channel);
  prv_meas_print_timestamp(convd_volt.timestamp);
  prv_meas_print_IV_point(convd_volt, convd_curr, channel);

  t2 = usec_get_timestamp();

  dbg(Debug, "meas_get_voltage_and_current() took: %lu usec\n", t2-t1);
}

/**
 * @brief measures a point of IV curve. Prints to main serial
 * - prints voltage and current measurement. Actual voltage might not be exactly the same as setpoint
 * - !! only one channel at a time
 * - During IV curve meassurement, usefull to leave current on to reduce settling time
 * @param channel channel to sample.
 * @param voltage voltage to force
 * @param disable_current_when_finished if 1, disables current when finished.
 * @param noident if 1, does not print identification string and timestamp. usefull if called from iv characteristic funct
 */
void meas_get_IV_point(uint8_t channel, float voltage, uint8_t disable_current_when_finished, uint8_t noident){
  //todo: change delays to RTOS delays

  t_daq_sample_raw raw_volt, raw_curr;
  t_daq_sample_convd convd_volt, convd_curr;
  float ch_volt, ch_curr, volt_cmd, min_curr_shntrng;
  uint32_t t1, t2;
  uint8_t iter_cnt;

  dbg(Debug, "MEAS:meas_get_IV_point()\n");
  assert_param(channel <= 6 && channel != 0);

  t1 = usec_get_timestamp();

  //set force voltage
  volt_cmd = voltage;
  //set force voltage
  fec_set_force_voltage(channel, volt_cmd);
  dbg(Debug, "force: %f\n", volt_cmd);
  //connect current stuff to DUT
  fec_enable_current(channel);


  //try to approach voltage first at 1x shunt, then 10x, 100x and 1000x
  for(uint8_t i=0 ; i<=3 ; i++){
    //start with requested voltage as setpoint
    //reset force setp
    volt_cmd = voltage;
    fec_set_force_voltage(channel, volt_cmd);
    //set shunt depending in which phase we are
    switch(i){
      case 0:
        //set shunt to 1x
        fec_set_shunt_1x(channel);
        min_curr_shntrng = FEC_SHNT_1X_LOWTHR;
        break;
      case 1:
        //set shunt to 10x
        fec_set_shunt_10x(channel);
        min_curr_shntrng = FEC_SHNT_10X_LOWTHR;
        break;
      case 2:
        //set shunt to 100x
        fec_set_shunt_100x(channel);
        min_curr_shntrng = FEC_SHNT_100X_LOWTHR;
        break;
      case 3:
        //set shunt to 1000x
        fec_set_shunt_1000x(channel);
        min_curr_shntrng = 0.0f;
        break;
      default:
        //never happens
        break;
    }
    //wait to settle
    HAL_Delay(MEAS_DUT_SETTLING_TIME_MS);
    //approach voltage, do not change shunt
    iter_cnt = 0;
    while(1){
      //prepare for sampling
      daq_prepare_for_sampling(MEAS_NUM_AVG_DEFAULT);
      //start sampling
      daq_start_sampling();
      //wait for sampling to finish
      while(!daq_is_sampling_done());
      //get raw averages from buffer
      raw_volt = daq_volt_raw_get_average(MEAS_NUM_AVG_DEFAULT);
      raw_curr = daq_curr_raw_get_average(MEAS_NUM_AVG_DEFAULT);

      //convert to volts and amps
      convd_volt = daq_raw_to_volt(raw_volt);
      convd_curr = daq_raw_to_curr(raw_curr);

      //get voltage of channel
      ch_volt = daq_get_from_sample_convd_by_index(convd_volt, channel);
      ch_curr = daq_get_from_sample_convd_by_index(convd_curr, channel);

      dbg(Debug, "vmeas: %f\n", ch_volt);
      dbg(Debug, "imeas: %f\n", ch_curr);

      //check if actual DUT voltage is close enough to setpoint
      if(ch_volt > voltage - MEAS_FORCE_VOLT_CLOSE_ENOUGH && ch_volt < voltage + MEAS_FORCE_VOLT_CLOSE_ENOUGH){
        //cool, we are close enough. report uter_cnt and break from for loop
        dbg(Debug, "Voltage approach converged with %d iterations\n", iter_cnt);
        break;
      }
      //not yet :/
      if(iter_cnt > MEAS_FORCE_VOLT_ITER_MAX){
        //approach iteration limit reached
        dbg(Warning, "Voltage approach did not converge!!\n");
        SCI_printf("NOCONVERGE\n");
        //break out of for loop
        break;
      }
        //compensate and try again
      else{
        //approximate drop across shunt and compensate.
        //this will not get us to the exact setpoint, but it will get us close enough
        //because current is dependent on voltage on DUT
        dbg(Debug, "Not close enough...:\n");

        float diff = ch_volt-voltage;
        dbg(Debug, "error: %f\n", diff);
        //calculate new voltage to force
        volt_cmd = volt_cmd - diff;
        //set new voltage
        fec_set_force_voltage(channel, volt_cmd);
        dbg(Debug, "force: %f\n", volt_cmd);
        //wait for DUT to settle
        HAL_Delay(MEAS_DUT_SETTLING_TIME_MS);
      }
      //increment iteration counter
      iter_cnt++;
    }
    //check if current is in current shunt range
    if(ch_curr > min_curr_shntrng){
      //cool, we are in range. break from for loop
      dbg(Debug, "Current in shnt range. Doing final measure...\n");
      break;
    }
    //current is below shunt range
    else{
      //increase shunt and try again
      dbg(Debug, "Current below shnt range. Trying next...\n");
    }

  }


  //measure and print data

  //report shunt ranges
  fec_report_shunt_ranges_dbg();

  //prepare for sampling
  daq_prepare_for_sampling(MEAS_NUM_AVG_DEFAULT);
  //start sampling
  daq_start_sampling();
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //get raw averages from buffer
  raw_volt = daq_volt_raw_get_average(MEAS_NUM_AVG_DEFAULT);
  raw_curr = daq_curr_raw_get_average(MEAS_NUM_AVG_DEFAULT);
  //convert to volts and amps
  convd_volt = daq_raw_to_volt(raw_volt);
  convd_curr = daq_raw_to_curr(raw_curr);

  //print to main serial
  if(!noident){
    //print data identification. usefull if called from iv characteristic neas
    prv_meas_print_data_ident_IV_point();
    prv_meas_print_ch_ident(channel);
    prv_meas_print_timestamp(convd_volt.timestamp);
  }
  prv_meas_print_IV_point(convd_volt, convd_curr, channel);

  //turn off current
  if(disable_current_when_finished){
    fec_disable_current(channel);
    fec_set_force_voltage(channel, 0.0f);
    fec_set_shunt_1000x(channel);
    dbg(Debug, "Disabled current\n");
  }
  else{
    dbg(Debug, "Kept current at last used value.\n");
  }



  //evaluate time and print
  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:meas_get_IV_point() took: %lu usec\n", t2-t1);

}

/**
 * @brief Dumps voltage bufer (converted to V) (num_samples points starting at 0) to main serial in human readable format
 * - call with channel = 0 to dump all channels
 * @param channel channel to dump
 * @param num_samples number of samples to dump
 *
 */
void prv_meas_dump_from_buffer_human_readable_volt(uint8_t channel, uint32_t num_samples){
  uint32_t t1, t2;
  t_daq_sample_raw sample_raw;
  t_daq_sample_convd sample_convd;

  t1 = usec_get_timestamp();

  dbg(Debug, "prv_meas_dump_from_buffer_human_readable_volt()\n");
  //check channel number
  assert_param(channel <= 6);

  //print ident
  prv_meas_print_data_ident_dump_text_volt();
  //print timestamp (of first element
  sample_raw = daq_get_from_buffer_volt(0);
  prv_meas_print_timestamp(sample_raw.timestamp);
  //print sample time
  SCI_printf("TS[us]:%f\n", (float)DAQ_SAMPLE_TIME_100KSPS);
  //print channel map
  prv_meas_print_ch_ident(channel);

  for(uint32_t n = 0 ; n< num_samples ; n++){
    //get sample from buffer (all channels)
    sample_raw = daq_get_from_buffer_volt(n);
    //convert to volts
    sample_convd = daq_raw_to_volt(sample_raw);
    //wait for some space in TX buffer
    while(SCI_get_tx_buffer_remaining() < 128);
    //print to serial
    prv_meas_print_sample(sample_convd, channel);
  }

  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:prv_meas_dump_from_buffer_human_readable_volt() took: %lu usec\n", t2-t1);
}

/**
 * @brief Dumps current buffer (converted to mA) (num_samples points starting at 0) to main serial in human readable format
 * - call with channel = 0 to dump all channels
 * @param channel channel to dump
 * @param num_samples number of samples to dump
 *
 */
void prv_meas_dump_from_buffer_human_readable_curr(uint8_t channel, uint32_t num_samples){
  uint32_t t1, t2;
  t_daq_sample_raw sample_raw;
  t_daq_sample_convd sample_convd;

  t1 = usec_get_timestamp();

  dbg(Debug, "prv_meas_dump_from_buffer_human_readable_curr()\n");
  //check channel number
  assert_param(channel <= 6);

  //print ident
  prv_meas_print_data_ident_dump_text_curr();
  //print timestamp (of first element
  sample_raw = daq_get_from_buffer_curr(0);
  prv_meas_print_timestamp(sample_raw.timestamp);
  //print sample time
  SCI_printf("TS[us]:%f\n", (float)DAQ_SAMPLE_TIME_100KSPS);
  //print channel map
  prv_meas_print_ch_ident(channel);

  for(uint32_t n = 0 ; n< num_samples ; n++){
    //get sample from buffer (all channels)
    sample_raw = daq_get_from_buffer_curr(n);
    //convert to current mA
    sample_convd = daq_raw_to_curr(sample_raw);
    //wait for some space in TX buffer
    while(SCI_get_tx_buffer_remaining() < 128);
    //print to serial
    prv_meas_print_sample(sample_convd, channel);
  }

  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:prv_meas_dump_from_buffer_human_readable_curr() took: %lu usec\n", t2-t1);
}

/**
 * @brief Dumps current buffer (converted to mA) (num_samples points starting at 0) to main serial in human readable format
 * - call with channel = 0 to dump all channels
 * @param channel channel to dump
 * @param num_samples number of samples to dump
 *
 */
void prv_meas_dump_from_buffer_human_readable_IV(uint8_t channel, uint32_t num_samples){
  uint32_t t1, t2;
  t_daq_sample_raw sample_raw_volt, sample_raw_curr;
  t_daq_sample_convd sample_convd_volt, sample_convd_curr;
  float ch_volt;

  t1 = usec_get_timestamp();

  dbg(Debug, "prv_meas_dump_from_buffer_human_readable_curr()\n");
  //check channel number
  assert_param(channel <= 6);

  //print ident
  prv_meas_print_data_ident_dump_text_IV();
  //print timestamp (of first element
  sample_raw_volt = daq_get_from_buffer_volt(0);
  prv_meas_print_timestamp(sample_raw_volt.timestamp);
  //print sample time
  SCI_printf("TS[us]:%f\n", (float)DAQ_SAMPLE_TIME_100KSPS);
  //print channel map
  prv_meas_print_ch_ident(channel);

  for(uint32_t n = 0 ; n< num_samples ; n++){
    //get sample from buffer (all channels)
    sample_raw_volt = daq_get_from_buffer_volt(n);
    sample_raw_curr = daq_get_from_buffer_curr(n);
    //convert to current V/mA
    sample_convd_volt = daq_raw_to_volt(sample_raw_volt);
    sample_convd_curr = daq_raw_to_curr(sample_raw_curr);
    //wait for some space in TX buffer
    while(SCI_get_tx_buffer_remaining() < 128);
    //print to serial
    prv_meas_print_IV_point(sample_convd_volt, sample_convd_curr, channel);
  }

  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:prv_meas_dump_from_buffer_human_readable_curr() took: %lu usec\n", t2-t1);
}

/**
 * @brief prints timestamp
 * @param timestamp timestamp to print
 */
void prv_meas_print_timestamp(uint64_t timestamp){
  SCI_printf("TIME:%llu\n", timestamp);
}

/**
 * @brief prints channel identification
 * - CH1 if single or CH1:CH2:CH3:CH4:CH5:CH6 if all
 * @param timestamp timestamp to print
 */
void prv_meas_print_ch_ident(uint8_t channel){
  if(channel== 0){
    SCI_printf("CH1:CH2:CH3:CH4:CH5:CH6\n");
  }
  else{
    SCI_printf("CH%u\n", channel);
  }

}

/**
 * @brief prints data identification VOLT
 */
void prv_meas_print_data_ident_voltage(void){
  SCI_printf("VOLT[V]:\n");
}

/**
 * @brief prints data identification VOLT
 */
void prv_meas_print_data_ident_current(void){
  SCI_printf("CURR[mA]:\n");
}

/**
 * @brief prints data identification IV point
 */
void prv_meas_print_data_ident_IV_point(void){
  SCI_printf("IV[mA__V]:\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_dump_text_volt(void){
  SCI_printf("DUMPVOLT[V]:\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_dump_text_curr(void){
  SCI_printf("DUMPCURR[V]:\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_dump_text_IV(void){
  SCI_printf("DUMPIVPT[V]:\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_IV_characteristic(void){
  SCI_printf("IVCHAR[mA__V]:\n");
}

/**
 * @brief Measure IV characteristic of DUT. Prints results to main serial
 * - !!! this is limited to single channel measurements !!!
 * @param channel channel to measure
 * @param start_volt start (low) voltage
 * @param end_volt end (high) voltage
 * @param step_volt step voltage
 */
void meas_get_iv_characteristic(uint8_t channel, float start_volt, float end_volt, float step_volt){
  uint32_t t1, t2;
  uint32_t num_iv_points;
  float setp;

  dbg(Debug, "MEAS:meas_get_iv_characteristic()\n");
  assert_param(channel <= 6 && channel != 0);

  t1 = usec_get_timestamp();

  //print ident
  //timestamp is just start of measurements, not for sample points
  prv_meas_print_data_ident_IV_characteristic();
  prv_meas_print_timestamp(usec_get_timestamp_64());
  prv_meas_print_ch_ident(channel);

  //calculate number of points we need to take
  //divide range by step_volt, add 1 to get all points, add 1 to get precise last point at end_volt
  num_iv_points = (uint32_t)(((end_volt - start_volt)/step_volt) + 2);
  dbg(Debug, "IV characteristic num points: %lu\n", num_iv_points);

  for(uint32_t n = 0 ; n < num_iv_points ; n++){

    //determine setpoint
    if(n == num_iv_points-1){
      //last point
      setp = end_volt;
    }
    else{
      //not last point
      setp = start_volt + n*step_volt;
    }
    meas_get_IV_point(channel, setp, 0, 1);

    //todo: maybe turn off current?

    t2 = usec_get_timestamp();
    dbg(Debug, "MEAS:prv_meas_dump_from_buffer_human_readable_curr() took: %lu usec\n", t2-t1);

  }
}