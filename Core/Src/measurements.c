//
// Created by Matej Planinšek on 01/08/2023.
//
#include "measurements.h"
#include "UserGPIO.h"
#include <math.h>

/**
 * @brief measures num_samples on channel, sends data to UART
 * Implemented for testing purposes as a first measure&send function
 * @param channel channel to measure
 * @param num_samples number of samples to measure

 */

//define number of averaged samples for all measurements here
//todo: do value setting in init
uint32_t prv_meas_num_avg = MEAS_NUM_AVG_DEFAULT;
uint32_t prv_meas_dut_settling_time_ms = MEAS_DUT_SETTLING_TIME_DEFAULT_MS;

//start timestamp of any measurement, that requires relative timestamps within a measurement (starting at zero)
uint64_t prv_meas_start_timestamp;


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

/**
 * @brief Sets DUT settling time
 * @param stltm settling time in ms
 */
void meas_set_settling_time(uint32_t stltm){
  prv_meas_dut_settling_time_ms = stltm;
}

/**
 * @brief Gets time to wait for DUT settling after changing force voltage
 * @return settling time set currently
 */
uint32_t meas_get_settling_time(void){
  return prv_meas_dut_settling_time_ms;
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
      mainser_printf("%f:%f:%f:%f:%f:%f\r\n", sample.ch1,sample.ch2, sample.ch3, sample.ch4, sample.ch5, sample.ch6);
      break;
    case 1:
      mainser_printf("%f\r\n", sample.ch1);
      break;
    case 2:
      mainser_printf("%f\r\n", sample.ch2);
      break;
    case 3:
      mainser_printf("%f\r\n", sample.ch3);
      break;
    case 4:
      mainser_printf("%f\r\n", sample.ch4);
      break;
    case 5:
      mainser_printf("%f\r\n", sample.ch5);
      break;
    case 6:
      mainser_printf("%f\r\n", sample.ch6);
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
    dbg(Warning,"prv_meas_print_volt_and_curr(): Volt/Curr timestamps not equal!\r\n");
  }

  switch(channel){
    case 0:
      mainser_printf("VOLT[V]:\r\n");
      mainser_printf("CH1:%f\r\n", sample_volt.ch1);
      mainser_printf("CH2:%f\r\n", sample_volt.ch2);
      mainser_printf("CH3:%f\r\n", sample_volt.ch3);
      mainser_printf("CH4:%f\r\n", sample_volt.ch4);
      mainser_printf("CH5:%f\r\n", sample_volt.ch5);
      mainser_printf("CH6:%f\r\n", sample_volt.ch6);
      mainser_printf("CURR[uA]:\r\n");
      mainser_printf("CH1:%f\r\n", sample_curr.ch1);
      mainser_printf("CH2:%f\r\n", sample_curr.ch2);
      mainser_printf("CH3:%f\r\n", sample_curr.ch3);
      mainser_printf("CH4:%f\r\n", sample_curr.ch4);
      mainser_printf("CH5:%f\r\n", sample_curr.ch5);
      mainser_printf("CH6:%f\r\n", sample_curr.ch6);
      mainser_printf("TIME:%llu\r\n", sample_volt.timestamp);
      break;
    case 1:
      mainser_printf("VOLT[V]:\r\nCH1:%f\r\n", sample_volt.ch1);
      mainser_printf("CURR[uA]:\r\nCH1:%f\r\n", sample_curr.ch1);
      mainser_printf("TIME:%llu\r\n", sample_volt.timestamp);
      break;
    case 2:
      mainser_printf("VOLT[V]:\r\nCH2:%f\r\n", sample_volt.ch2);
      mainser_printf("CURR[uA]:\r\nCH2:%f\r\n", sample_curr.ch2);
      mainser_printf("TIME:%llu\r\n", sample_volt.timestamp);
      break;
    case 3:
      mainser_printf("VOLT[V]:\r\nCH3:%f\r\n", sample_volt.ch3);
      mainser_printf("CURR[uA]:\r\nCH3:%f\r\n", sample_curr.ch3);
      mainser_printf("TIME:%llu\r\n", sample_volt.timestamp);
      break;
    case 4:
      mainser_printf("VOLT[V]:\r\nCH4:%f\r\n", sample_volt.ch4);
      mainser_printf("CURR[uA]:\r\nCH4:%f\r\n", sample_curr.ch4);
      mainser_printf("TIME:%llu\r\n", sample_volt.timestamp);
      break;
    case 5:
      mainser_printf("VOLT[V]:\r\nCH5:%f\r\n", sample_volt.ch5);
      mainser_printf("CURR[uA]:\r\nCH5:%f\r\n", sample_curr.ch5);
      mainser_printf("TIME:%llu\r\n", sample_volt.timestamp);
      break;
    case 6:
      mainser_printf("VOLT[V]:\r\nCH6:%f\r\n", sample_volt.ch6);
      mainser_printf("CURR[uA]:\r\nCH6:%f\r\n", sample_curr.ch6);
      mainser_printf("TIME:%llu\r\n", sample_volt.timestamp);
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
void prv_meas_print_IV_point_ts(t_daq_sample_convd sample_volt, t_daq_sample_convd sample_curr, uint8_t channel, uint8_t channel_mask){
  uint64_t sample_time;
  //voltage and current should have same timestamp if we want to print them in one go. Timestamp for voltage is used.
  //warning if timestamps not equal
  if(sample_volt.timestamp != sample_curr.timestamp){
    dbg(Warning,"prv_meas_print_volt_and_curr(): Volt/Curr timestamps not equal!\r\n");
    //I am guessing this can happen on occasion, if the timer
  }
  sample_time = sample_volt.timestamp-prv_meas_start_timestamp;

  switch(channel){
    case 0:
      (channel_mask & (1<<0)) ? mainser_printf("%f_%f:", sample_curr.ch1, sample_volt.ch1) : mainser_printf("NaN_NaN:");
      (channel_mask & (1<<1)) ? mainser_printf("%f_%f:", sample_curr.ch2, sample_volt.ch2) : mainser_printf("NaN_NaN:");
      (channel_mask & (1<<2)) ? mainser_printf("%f_%f:", sample_curr.ch3, sample_volt.ch3) : mainser_printf("NaN_NaN:");
      (channel_mask & (1<<3)) ? mainser_printf("%f_%f:", sample_curr.ch4, sample_volt.ch4) : mainser_printf("NaN_NaN:");
      (channel_mask & (1<<4)) ? mainser_printf("%f_%f:", sample_curr.ch5, sample_volt.ch5) : mainser_printf("NaN_NaN:");
      (channel_mask & (1<<5)) ? mainser_printf("%f_%f:", sample_curr.ch6, sample_volt.ch6) : mainser_printf("NaN_NaN:");
      mainser_printf("%llu\r\n", sample_time);
      break;
    case 1:
      mainser_printf("%f_%f:", sample_curr.ch1, sample_volt.ch1);
      mainser_printf("%llu\r\n", sample_time);
      break;
    case 2:
      mainser_printf("%f_%f:", sample_curr.ch2, sample_volt.ch2);
      mainser_printf("%llu\r\n", sample_time);
      break;
    case 3:
      mainser_printf("%f_%f:", sample_curr.ch3, sample_volt.ch3);
      mainser_printf("%llu\r\n", sample_time);
      break;
    case 4:
      mainser_printf("%f_%f:", sample_curr.ch4, sample_volt.ch4);
      mainser_printf("%llu\r\n", sample_time);
      break;
    case 5:
      mainser_printf("%f_%f:", sample_curr.ch5, sample_volt.ch5);
      mainser_printf("%llu\r\n", sample_time);
      break;
    case 6:
      mainser_printf("%f_%f:", sample_curr.ch6, sample_volt.ch6);
      mainser_printf("%llu\r\n", sample_time);
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
    dbg(Warning,"prv_meas_print_volt_and_curr(): Volt/Curr timestamps not equal!\r\n");

  }

  switch(channel){
    case 0:
      mainser_printf("%f_%f:", sample_curr.ch1, sample_volt.ch1);
      mainser_printf("%f_%f:", sample_curr.ch2, sample_volt.ch2);
      mainser_printf("%f_%f:", sample_curr.ch3, sample_volt.ch3);
      mainser_printf("%f_%f:", sample_curr.ch4, sample_volt.ch4);
      mainser_printf("%f_%f:", sample_curr.ch5, sample_volt.ch5);
      mainser_printf("%f_%f\r\n", sample_curr.ch6, sample_volt.ch6);
      break;
    case 1:
      mainser_printf("%f_%f\r\n", sample_curr.ch1, sample_volt.ch1);
      break;
    case 2:
      mainser_printf("%f_%f\r\n", sample_curr.ch2, sample_volt.ch2);
      break;
    case 3:
      mainser_printf("%f_%f\r\n", sample_curr.ch3, sample_volt.ch3);
      break;
    case 4:
      mainser_printf("%f_%f\r\n", sample_curr.ch4, sample_volt.ch4);
      break;
    case 5:
      mainser_printf("%f_%f\r\n", sample_curr.ch5, sample_volt.ch5);
      break;
    case 6:
      mainser_printf("%f_%f\r\n", sample_curr.ch6, sample_volt.ch6);
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
  dbg(Debug, "MEAS:get_voltage()\r\n");

  assert_param(channel <= 6);
  meas = daq_single_shot_volt(prv_meas_num_avg);
  //check if out of range
  meas_check_out_of_rng_volt(meas, channel);
  //print sample
  prv_meas_print_data_ident_voltage();
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(meas.timestamp);
  prv_meas_print_sample(meas, channel);

  t2 = usec_get_timestamp();
  dbg(Debug, "meas_get_voltage() took: %lu usec\r\n", t2-t1);
}

/**
 * @brief Measures RMS noise level on one or all (param=0) voltage channels. Prints to main serial
 * @param channel channel to measure
 */
void meas_get_noise_volt(uint8_t channel){
  t_daq_sample_raw avg;
  uint32_t t1, t2;
  int32_t sample_noise[6];
  uint32_t sum_squares[6];
  double snr[6];
  t_daq_sample_raw rms_noise_raw;
  t_daq_sample_convd rms_noise_convd;
  t1 = usec_get_timestamp();
  rms_noise_raw.timestamp = t1;
  dbg(Debug, "MEAS:meas_get_noise_volt()\r\n");
  assert_param(channel <= 6);
  daq_single_shot_volt(NOISE_MEASURE_NUMSAMPLES);
  avg = daq_volt_raw_get_average(NOISE_MEASURE_NUMSAMPLES);

  sum_squares[0] = 0;
  sum_squares[1] = 0;
  sum_squares[2] = 0;
  sum_squares[3] = 0;
  sum_squares[4] = 0;
  sum_squares[5] = 0;
  for(uint32_t i = 0; i < NOISE_MEASURE_NUMSAMPLES; i++){
    sample_noise[0] = daq_get_from_buffer_volt(i).ch1 - avg.ch1;
    sample_noise[1] = daq_get_from_buffer_volt(i).ch2 - avg.ch2;
    sample_noise[2] = daq_get_from_buffer_volt(i).ch3 - avg.ch3;
    sample_noise[3] = daq_get_from_buffer_volt(i).ch4 - avg.ch4;
    sample_noise[4] = daq_get_from_buffer_volt(i).ch5 - avg.ch5;
    sample_noise[5] = daq_get_from_buffer_volt(i).ch6 - avg.ch6;
    for(uint8_t n = 0 ; n<6 ; n++){
      sum_squares[n] += sample_noise[n] * sample_noise[n];  // Square the noise
    }
  }
  rms_noise_raw.ch1 = sqrt((double)sum_squares[0] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch2 = sqrt((double)sum_squares[1] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch3 = sqrt((double)sum_squares[2] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch4 = sqrt((double)sum_squares[3] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch5 = sqrt((double)sum_squares[4] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch6 = sqrt((double)sum_squares[5] / (double)NOISE_MEASURE_NUMSAMPLES);

  snr[0] = 20*log(avg.ch1 / rms_noise_raw.ch1);
  snr[1] = 20*log(avg.ch2 / rms_noise_raw.ch2);
  snr[2] = 20*log(avg.ch3 / rms_noise_raw.ch3);
  snr[3] = 20*log(avg.ch4 / rms_noise_raw.ch4);
  snr[4] = 20*log(avg.ch5 / rms_noise_raw.ch5);
  snr[5] = 20*log(avg.ch6 / rms_noise_raw.ch6);

  rms_noise_convd = daq_raw_to_volt(rms_noise_raw);
  rms_noise_convd.timestamp = rms_noise_raw.timestamp;
  rms_noise_convd.ch1 = (((float)rms_noise_raw.ch1 / DAQ_MAX_ADC_VAL) * DAQ_VREF)*DAQ_VOLT_AMP_GAIN_CH1;
  rms_noise_convd.ch2 = (((float)rms_noise_raw.ch2 / DAQ_MAX_ADC_VAL) * DAQ_VREF)*DAQ_VOLT_AMP_GAIN_CH2;
  rms_noise_convd.ch3 = (((float)rms_noise_raw.ch3 / DAQ_MAX_ADC_VAL) * DAQ_VREF)*DAQ_VOLT_AMP_GAIN_CH3;
  rms_noise_convd.ch4 = (((float)rms_noise_raw.ch4 / DAQ_MAX_ADC_VAL) * DAQ_VREF)*DAQ_VOLT_AMP_GAIN_CH4;
  rms_noise_convd.ch5 = (((float)rms_noise_raw.ch5 / DAQ_MAX_ADC_VAL) * DAQ_VREF)*DAQ_VOLT_AMP_GAIN_CH5;
  rms_noise_convd.ch6 = (((float)rms_noise_raw.ch6 / DAQ_MAX_ADC_VAL) * DAQ_VREF)*DAQ_VOLT_AMP_GAIN_CH6;

  mainser_printf("RMS_VOLTNOISE[mV]:\r\n");
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(daq_get_sampling_start_timestamp());

  switch(channel){
    case 1:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch1 * 1000));
      break;
    case 2:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch2 * 1000));
      break;
    case 3:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch3 * 1000));

      break;
    case 4:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch4 * 1000));
      break;
    case 5:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch5 * 1000));
      break;
    case 6:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch6 * 1000));
      break;
    case 0:
      mainser_printf("%f", (float)(rms_noise_convd.ch1 * 1000));
      mainser_printf(":%f", (float)(rms_noise_convd.ch2 * 1000));
      mainser_printf(":%f", (float)(rms_noise_convd.ch3 * 1000));
      mainser_printf(":%f", (float)(rms_noise_convd.ch4 * 1000));
      mainser_printf(":%f", (float)(rms_noise_convd.ch5 * 1000));
      mainser_printf(":%f\r\n", (float)(rms_noise_convd.ch6 * 1000));
  }

  mainser_printf("SNR_VOLTNOISE[dB]:\r\n");
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(daq_get_sampling_start_timestamp());

  switch(channel){
    case 1:
      mainser_printf("%f\r\n", (float)snr[0]);
      break;
    case 2:
      mainser_printf("%f\r\n", (float)snr[1]);
      break;
    case 3:
      mainser_printf("%f\r\n", (float)snr[2]);
      break;
    case 4:
      mainser_printf("%f\r\n", (float)snr[3]);
      break;
    case 5:
      mainser_printf("%f\r\n", (float)snr[4]);
      break;
    case 6:
      mainser_printf("%f\r\n", (float)snr[5]);
      break;
    case 0:
      mainser_printf("%f", (float)snr[0]);
      mainser_printf(":%f", (float)snr[1]);
      mainser_printf(":%f", (float)snr[2]);
      mainser_printf(":%f", (float)snr[3]);
      mainser_printf(":%f", (float)snr[4]);
      mainser_printf(":%f\r\n", (float)snr[5]);
  }


  t2 = usec_get_timestamp();
  dbg(Debug, "meas_get_noise_volt() took: %lu usec\r\n", t2-t1);

}


/**
 * @brief Measures RMS noise level on one or all (param=0) current channels. Prints to main serial
 * @param channel channel to measure
 */
void meas_get_noise_curr(uint8_t channel){
  t_daq_sample_raw avg;
  uint32_t t1, t2;
  int32_t sample_noise[6];
  uint32_t sum_squares[6];
  double snr[6];
  t_daq_sample_raw rms_noise_raw;
  t_daq_sample_convd rms_noise_convd;
  t1 = usec_get_timestamp();
  dbg(Debug, "MEAS:meas_get_noise_curr()\r\n");
  assert_param(channel <= 6);
  daq_single_shot_volt(NOISE_MEASURE_NUMSAMPLES);
  avg = daq_curr_raw_get_average(NOISE_MEASURE_NUMSAMPLES);

  sum_squares[0] = 0;
  sum_squares[1] = 0;
  sum_squares[2] = 0;
  sum_squares[3] = 0;
  sum_squares[4] = 0;
  sum_squares[5] = 0;
  for(uint32_t i = 0; i < NOISE_MEASURE_NUMSAMPLES; i++){
    sample_noise[0] = daq_get_from_buffer_curr(i).ch1 - avg.ch1;
    sample_noise[1] = daq_get_from_buffer_curr(i).ch2 - avg.ch2;
    sample_noise[2] = daq_get_from_buffer_curr(i).ch3 - avg.ch3;
    sample_noise[3] = daq_get_from_buffer_curr(i).ch4 - avg.ch4;
    sample_noise[4] = daq_get_from_buffer_curr(i).ch5 - avg.ch5;
    sample_noise[5] = daq_get_from_buffer_curr(i).ch6 - avg.ch6;
    for(uint8_t n = 0 ; n<6 ; n++){
      sum_squares[n] += sample_noise[n] * sample_noise[n];  // Square the noise
    }
  }
  rms_noise_raw.ch1 = sqrt((double)sum_squares[0] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch2 = sqrt((double)sum_squares[1] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch3 = sqrt((double)sum_squares[2] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch4 = sqrt((double)sum_squares[3] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch5 = sqrt((double)sum_squares[4] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch6 = sqrt((double)sum_squares[5] / (double)NOISE_MEASURE_NUMSAMPLES);

  snr[0] = 20*log(avg.ch1 / rms_noise_raw.ch1);
  snr[1] = 20*log(avg.ch2 / rms_noise_raw.ch2);
  snr[2] = 20*log(avg.ch3 / rms_noise_raw.ch3);
  snr[3] = 20*log(avg.ch4 / rms_noise_raw.ch4);
  snr[4] = 20*log(avg.ch5 / rms_noise_raw.ch5);
  snr[5] = 20*log(avg.ch6 / rms_noise_raw.ch6);

  rms_noise_raw.ch1 = sqrt((double)sum_squares[0] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch2 = sqrt((double)sum_squares[1] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch3 = sqrt((double)sum_squares[2] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch4 = sqrt((double)sum_squares[3] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch5 = sqrt((double)sum_squares[4] / (double)NOISE_MEASURE_NUMSAMPLES);
  rms_noise_raw.ch6 = sqrt((double)sum_squares[5] / (double)NOISE_MEASURE_NUMSAMPLES);

  float shunt_volt;
  // conversion is kinda hacky, but we don't want to compensate offset here, just differential and so can not use function...
  shunt_volt = ((float)rms_noise_raw.ch1 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  shunt_volt *= 1000000UL;
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN_CH1;
  rms_noise_convd.ch1 = shunt_volt / fec_get_shunt_resistance(1);

  shunt_volt = ((float)rms_noise_raw.ch2 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  shunt_volt *= 1000000UL;
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN_CH2;
  rms_noise_convd.ch2 = shunt_volt / fec_get_shunt_resistance(2);


  shunt_volt = ((float)rms_noise_raw.ch3 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  shunt_volt *= 1000000UL;
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN_CH3;
  rms_noise_convd.ch3 = shunt_volt / fec_get_shunt_resistance(3);

  shunt_volt = ((float)rms_noise_raw.ch4 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  shunt_volt *= 1000000UL;
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN_CH4;
  rms_noise_convd.ch4 = shunt_volt / fec_get_shunt_resistance(4);

  shunt_volt = ((float)rms_noise_raw.ch5 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  shunt_volt *= 1000000UL;
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN_CH5;
  rms_noise_convd.ch5 = shunt_volt / fec_get_shunt_resistance(5);

  shunt_volt = ((float)rms_noise_raw.ch6 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  shunt_volt *= 1000000UL;
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN_CH6;
  rms_noise_convd.ch6 = shunt_volt / fec_get_shunt_resistance(6);

  //print to mainser

  mainser_printf("RMS_CURRNOISE[uA]:\r\n");
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(daq_get_sampling_start_timestamp());

  switch(channel){
    case 1:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch1));
      break;
    case 2:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch2));
      break;
    case 3:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch3));
      break;
    case 4:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch4));
      break;
    case 5:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch5));
      break;
    case 6:
      mainser_printf("%f\r\n", (float)(rms_noise_convd.ch6));
      break;
    case 0:
      mainser_printf("%f", (float)(rms_noise_convd.ch1));
      mainser_printf(":%f", (float)(rms_noise_convd.ch2));
      mainser_printf(":%f", (float)(rms_noise_convd.ch3));
      mainser_printf(":%f", (float)(rms_noise_convd.ch4));
      mainser_printf(":%f", (float)(rms_noise_convd.ch5));
      mainser_printf(":%f\r\n", (float)(rms_noise_convd.ch6));
  }

  mainser_printf("SNR_CURRNOISE[dB]:\r\n");
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(daq_get_sampling_start_timestamp());

  switch(channel){
    case 1:
      mainser_printf("%f\r\n", (float)snr[0]);
      break;
    case 2:
      mainser_printf("%f\r\n", (float)snr[1]);
      break;
    case 3:
      mainser_printf("%f\r\n", (float)snr[2]);
      break;
    case 4:
      mainser_printf("%f\r\n", (float)snr[3]);
      break;
    case 5:
      mainser_printf("%f\r\n", (float)snr[4]);
      break;
    case 6:
      mainser_printf("%f\r\n", (float)snr[5]);
      break;
    case 0:
      mainser_printf("%f", (float)snr[0]);
      mainser_printf(":%f", (float)snr[1]);
      mainser_printf(":%f", (float)snr[2]);
      mainser_printf(":%f", (float)snr[3]);
      mainser_printf(":%f", (float)snr[4]);
      mainser_printf(":%f\r\n", (float)snr[5]);
  }


  t2 = usec_get_timestamp();
  dbg(Debug, "meas_get_noise_curr() took: %lu usec\r\n", t2-t1);

}

/**
 * @brief Measures current on one or all (param=0) channels. Prints to main serial
 * Not that usefull to measure jut current - more for testing
 * !!! Does not change current enable, shunts, force voltage or do any autoranging !!!
 * - Enabling current and setting the voltage setpoint should be done before this.
 * This function just autoranges and measures current
 * @param channel channel to measure
 */
void meas_get_current(uint8_t channel){
  t_daq_sample_convd meas;
  uint32_t t1, t2;

  t1= usec_get_timestamp();
  dbg(Debug, "MEAS:meas_get_current()\r\n");
  //measure and print
  meas = daq_single_shot_curr_no_autorng(prv_meas_num_avg);
  meas_check_out_of_rng_curr(meas, channel);


  //print sample
  prv_meas_print_data_ident_current();
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(meas.timestamp);
  prv_meas_print_sample(meas, channel);

  //report shunt config to debug serial
  t2 = usec_get_timestamp();
  dbg(Debug, "meas_get_current() took: %lu usec\r\n", t2-t1);

}

/**
 * @brief checks if voltage sample (per channel check) is out of range
 * @param sample sample to check
 */
void meas_check_out_of_rng_volt(t_daq_sample_convd sample, uint8_t channel){

  switch(channel){
    case 0:
      if(sample.ch1 > FEC_VOLT_OVRNG || sample.ch1 < FEC_VOLT_UNDRNG){
        mainser_printf("CH1:VOLT:OUT_OF_RANGE\r\n");
      }

      if(sample.ch2 > FEC_VOLT_OVRNG || sample.ch2 < FEC_VOLT_UNDRNG){
        mainser_printf("CH2:VOLT:OUT_OF_RANGE\r\n");
      }

      if(sample.ch3 > FEC_VOLT_OVRNG || sample.ch3 < FEC_VOLT_UNDRNG){
        mainser_printf("CH3:VOLT:OUT_OF_RANGE\r\n");
      }

      if(sample.ch4 > FEC_VOLT_OVRNG || sample.ch4 < FEC_VOLT_UNDRNG){
        mainser_printf("CH4:VOLT:OUT_OF_RANGE\r\n");
      }

      if(sample.ch5 > FEC_VOLT_OVRNG || sample.ch5 < FEC_VOLT_UNDRNG){
        mainser_printf("CH5:VOLT:OUT_OF_RANGE\r\n");
      }

      if(sample.ch6 > FEC_VOLT_OVRNG || sample.ch6 < FEC_VOLT_UNDRNG){
        mainser_printf("CH6:VOLT:OUT_OF_RANGE\r\n");
      }
      break;

    case 1:
      if(sample.ch1 > FEC_VOLT_OVRNG || sample.ch1 < FEC_VOLT_UNDRNG){
        mainser_printf("CH1:VOLT:OUT_OF_RANGE\r\n");
      }
      break;

    case 2:
      if(sample.ch2 > FEC_VOLT_OVRNG || sample.ch2 < FEC_VOLT_UNDRNG){
        mainser_printf("CH2:VOLT:OUT_OF_RANGE\r\n");
      }
      break;

    case 3:
      if(sample.ch3 > FEC_VOLT_OVRNG || sample.ch3 < FEC_VOLT_UNDRNG){
        mainser_printf("CH3:VOLT:OUT_OF_RANGE\r\n");
      }
      break;

    case 4:
      if(sample.ch4 > FEC_VOLT_OVRNG || sample.ch4 < FEC_VOLT_UNDRNG){
        mainser_printf("CH4:VOLT:OUT_OF_RANGE\r\n");
      }
      break;

    case 5:
      if(sample.ch5 > FEC_VOLT_OVRNG || sample.ch5 < FEC_VOLT_UNDRNG){
        mainser_printf("CH5:VOLT:OUT_OF_RANGE\r\n");
      }
      break;

    case 6:
      if(sample.ch6 > FEC_VOLT_OVRNG || sample.ch6 < FEC_VOLT_UNDRNG){
        mainser_printf("CH6:VOLT:OUT_OF_RANGE\r\n");
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
        mainser_printf("CH1:CURR:OUT_OF_RANGE\r\n");
      }

      if(sample.ch2 > FEC_CURR_OVRNG || sample.ch2 < FEC_CURR_UNDRNG){
        mainser_printf("CH2:CURR:OUT_OF_RANGE\r\n");
      }

      if(sample.ch3 > FEC_CURR_OVRNG || sample.ch3 < FEC_CURR_UNDRNG){
        mainser_printf("CH3:CURR:OUT_OF_RANGE\r\n");
      }

      if(sample.ch4 > FEC_CURR_OVRNG || sample.ch4 < FEC_CURR_UNDRNG){
        mainser_printf("CH4:CURR:OUT_OF_RANGE\r\n");
      }

      if(sample.ch5 > FEC_CURR_OVRNG || sample.ch5 < FEC_CURR_UNDRNG){
        mainser_printf("CH5:CURR:OUT_OF_RANGE\r\n");
      }

      if(sample.ch6 > FEC_CURR_OVRNG || sample.ch6 < FEC_CURR_UNDRNG){
        mainser_printf("CH6:CURR:OUT_OF_RANGE\r\n");
      }
      break;

    case 1:
      if(sample.ch1 > FEC_CURR_OVRNG || sample.ch1 < FEC_CURR_UNDRNG){
        mainser_printf("CH1:CURR:OUT_OF_RANGE\r\n");
      }
      break;

    case 2:
      if(sample.ch2 > FEC_CURR_OVRNG || sample.ch2 < FEC_CURR_UNDRNG){
        mainser_printf("CH2:CURR:OUT_OF_RANGE\r\n");
      }
      break;

    case 3:
      if(sample.ch3 > FEC_CURR_OVRNG || sample.ch3 < FEC_CURR_UNDRNG){
        mainser_printf("CH3:CURR:OUT_OF_RANGE\r\n");
      }
      break;

    case 4:
      if(sample.ch4 > FEC_CURR_OVRNG || sample.ch4 < FEC_CURR_UNDRNG){
        mainser_printf("CH4:CURR:OUT_OF_RANGE\r\n");
      }
      break;

    case 5:
      if(sample.ch5 > FEC_CURR_OVRNG || sample.ch5 < FEC_CURR_UNDRNG){
        mainser_printf("CH5:CURR:OUT_OF_RANGE\r\n");
      }
      break;

    case 6:
      if(sample.ch6 > FEC_CURR_OVRNG || sample.ch6 < FEC_CURR_UNDRNG){
        mainser_printf("CH6:CURR:OUT_OF_RANGE\r\n");
      }
      break;

    default:
      break;


  }
}

/**
 * @brief Measures current and voltage at the exact same time. on one or all (param=0) channels. Prints to main serial
 * !!! Does not change current enable, shunts, force voltage or do any autoranging !!!
 * - does not manipulate enable current and voltage force setpoint
 * does not check for propperly selected shunt !!!
 * @param channel channel to sample. 0 for all channels
 */
void meas_get_voltage_and_current(uint8_t channel){
  t_daq_sample_raw raw_volt, raw_curr;
  t_daq_sample_convd convd_volt, convd_curr;
  uint32_t t1, t2;

  t1 = usec_get_timestamp();

  dbg(Debug, "MEAS:meas_get_voltage_and_current()\r\n");

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
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(convd_volt.timestamp);
  prv_meas_print_IV_point(convd_volt, convd_curr, channel);

  t2 = usec_get_timestamp();

  dbg(Debug, "meas_get_voltage_and_current() took: %lu usec\r\n", t2-t1);
}

/**
 * @brief measures a point of IV curve at the exact selected voltage. Prints to main serial
 * - prints voltage and current measurement. Actual voltage might not be exactly the same as setpoint
 * - !! only one channel at a time
 * - During IV curve meassurement, usefull to leave current on to reduce settling time
 * @param channel channel to sample.
 * @param voltage voltage to force
 * @param disable_current_when_finished if 1, disables current when finished.
 * @param noident if 1, does not print identification string and timestamp. usefull if called from iv characteristic funct
 * @return current measured. used to terminate IV curve mesurements when current reaches 0
 */
float meas_get_exact_IV_point(uint8_t channel, float voltage, uint8_t disable_current_when_finished, uint8_t noident){
  //todo: change delays to RTOS delays

  t_daq_sample_raw raw_volt, raw_curr;
  t_daq_sample_convd convd_volt, convd_curr;
  float ch_volt, ch_curr, volt_cmd, min_curr_shntrng;
  uint32_t t1, t2;
  uint8_t iter_cnt;

  dbg(Debug, "MEAS:meas_get_exact_IV_point()\r\n");
  assert_param(channel <= 6 && channel != 0);

  t1 = usec_get_timestamp();

  //set force voltage
  volt_cmd = voltage;
  //set force voltage
  fec_set_force_voltage(channel, volt_cmd);
  dbg(Debug, "force: %f\r\n", volt_cmd);
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
    HAL_Delay(prv_meas_dut_settling_time_ms);
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

      dbg(Debug, "vmeas: %f\r\n", ch_volt);
      dbg(Debug, "imeas: %f\r\n", ch_curr);

      //check if actual DUT voltage is close enough to setpoint
      if(ch_volt > voltage - MEAS_FORCE_VOLT_CLOSE_ENOUGH && ch_volt < voltage + MEAS_FORCE_VOLT_CLOSE_ENOUGH){
        //cool, we are close enough. report uter_cnt and break from for loop
        dbg(Debug, "Voltage approach converged with %d iterations\r\n", iter_cnt);
        break;
      }
      //not yet :/
      if(iter_cnt > MEAS_FORCE_VOLT_ITER_MAX){
        //approach iteration limit reached
        dbg(Warning, "Voltage approach did not converge!!\r\n");
        if(MEAS_NOCONVERGE_REPORT){
          //report to main serial
          mainser_printf("NOCONVERGE\r\n");
        }
        //break out of for loop
        break;
      }
        //compensate and try again
      else{
        //approximate drop across shunt and compensate.
        //this will not get us to the exact setpoint, but it will get us close enough
        //because current is dependent on voltage on DUT
        dbg(Debug, "Not close enough...:\r\n");

        float diff = ch_volt-voltage;
        dbg(Debug, "error: %f\r\n", diff);
        //calculate new voltage to force
        volt_cmd = volt_cmd - diff;
        //set new voltage
        fec_set_force_voltage(channel, volt_cmd);
        dbg(Debug, "force: %f\r\n", volt_cmd);
        //wait for DUT to settle
        HAL_Delay(prv_meas_dut_settling_time_ms);
      }
      //increment iteration counter
      iter_cnt++;
    }
    //check if current is in current shunt range
    if(ch_curr > min_curr_shntrng){
      //cool, we are in range. break from for loop
      dbg(Debug, "Current in shnt range. Doing final measure...\r\n");
      break;
    }
    //current is below shunt range
    else{
      //increase shunt and try again
      dbg(Debug, "Current below shnt range. Trying next...\r\n");
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
    prv_meas_print_ch_ident(channel,0);
    prv_meas_print_timestamp(convd_volt.timestamp);
  }
  prv_meas_print_IV_point(convd_volt, convd_curr, channel);

  //turn off current
  if(disable_current_when_finished){
    fec_disable_current(channel);
    fec_set_force_voltage(channel, 0.0f);
    fec_set_shunt_1000x(channel);
    dbg(Debug, "Disabled current\r\n");
  }
  else{
    dbg(Debug, "Kept current at last used value.\r\n");
  }



  //evaluate time and print
  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:meas_get_IV_point() took: %lu usec\r\n", t2-t1);

  //return current measured (for terminating IV curve measurements when current reaches 0)

  float retcur;
  switch (channel) {
    case 1:
      retcur = convd_curr.ch1;
      break;
    case 2:
      retcur = convd_curr.ch2;
      break;
    case 3:
      retcur = convd_curr.ch3;
      break;
    case 4:
      retcur = convd_curr.ch4;
      break;
    case 5:
      retcur = convd_curr.ch5;
      break;
    case 6:
      retcur = convd_curr.ch6;
      break;
    default:
      retcur = 0.0f;
      break;
  }

  return retcur;

}


/**
 * @brief finds the required range for current measurement range for IV curve scanning at the selected voltage.
 * @param channel channel to sample.
 * @param voltage voltage to force
 * @return current measured. used to terminate IV curve mesurements when current reaches 0
 */
enum shntEnum SelectedRange[6]={shnt_10X,shnt_10X,shnt_10X,shnt_10X,shnt_10X,shnt_10X};
void autorange_IV_point(uint8_t channel, float voltage, uint32_t settling_time, t_daq_sample_convd* convd_volt, t_daq_sample_convd* convd_curr){

  t_daq_sample_raw raw_volt, raw_curr;
  float ch_curr, ch_volt, volt_cmd;
  uint32_t t1, t2;

  dbg(Debug, "MEAS:meas_get_IV_point()\r\n");

  t1 = usec_get_timestamp();

  volt_cmd = voltage;

  dbg(Debug, "force: %f\r\n", voltage);
  fec_set_force_voltage(channel, volt_cmd);  //set force voltage
  fec_set_shunt_10x(channel);                //start with 10x range
  fec_enable_current(channel);               //connect current stuff to DUT

  for (int i=0; i<2; i++)
  {
    usec_delay(settling_time);

    daq_prepare_for_sampling(1);  //1 sample per measurement should be enough to determine the range
    daq_start_sampling();         //Measures all channels always
    while(!daq_is_sampling_done());
    //get raw averages from buffer
    raw_curr = daq_curr_raw_get_average(1);
    raw_volt = daq_volt_raw_get_average(1);
    //convert to volts and amps
    *convd_curr = daq_raw_to_curr(raw_curr);
    *convd_volt = daq_raw_to_volt(raw_volt);

    for (int ch=0; ch < FEC_NUM_CHANNELS; ch++)
    {
      if ( (channel == 0) || (channel == ch+1) )
      {
        //get current of channel
        ch_curr = daq_get_from_sample_convd_by_index(*convd_curr, ch+1);
        ch_volt = daq_get_from_sample_convd_by_index(*convd_volt, ch+1);

        if (ch_curr < FEC_SHNT_100X_LOWTHR)
        {
          fec_set_shunt_1000x(ch+1);
          SelectedRange[ch] = shnt_1000X;
          volt_cmd = -22000.0e-6*ch_curr + voltage; //current is in uA
          dbg(Debug, "CH%u: V=%f, I=%f Rs=22k V2=%f\r\n", ch+1, ch_volt,ch_curr,volt_cmd);
        }
        else if (ch_curr < FEC_SHNT_10X_LOWTHR)
        {
          fec_set_shunt_100x(ch+1);
          SelectedRange[ch] = shnt_100X;
          volt_cmd = -2200.0e-6*ch_curr + voltage; //current is in uA
          dbg(Debug, "CH%u: V=%f, I=%f Rs=2k2 V2=%f\r\n", ch+1, ch_volt,ch_curr,volt_cmd);
        }
        else if (ch_curr < FEC_SHNT_1X_LOWTHR)
        {
          fec_set_shunt_10x(ch+1);
          SelectedRange[ch] = shnt_10X;
          volt_cmd = -220.0e-6*ch_curr + voltage; //current is in uA
          dbg(Debug, "CH%u: V=%f, I=%f Rs=220 V2=%f\r\n", ch+1, ch_volt,ch_curr,volt_cmd);
        }
        else
        {
          fec_set_shunt_1x(ch+1);
          SelectedRange[ch] = shnt_1X;
          volt_cmd = -22.0e-6*ch_curr + voltage; //current is in uA
          dbg(Debug, "CH%u: V=%f, I=%f Rs=22  V2=%f\r\n", ch+1, ch_volt,ch_curr,volt_cmd);
        }
        fec_set_force_voltage(ch+1, volt_cmd);  //set force voltage
      }
    }
  }
  //report shunt ranges
  fec_report_shunt_ranges_dbg();

  //evaluate time and print
  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:autorange took: %lu usec\r\n", t2-t1);

}

void adjust_range_IV_point(uint8_t channel, uint8_t channel_mask, t_daq_sample_convd* convd_curr)
{
  float ch_curr;

  for (int ch=0; ch < FEC_NUM_CHANNELS; ch++)
  {
    if ( (channel_mask & (1<<ch)) != 0)
    {
      //get current of channel
      ch_curr = daq_get_from_sample_convd_by_index(*convd_curr, ch+1);

      if (ch_curr < FEC_SHNT_100X_LOWTHR)
      {
        fec_set_shunt_1000x(ch+1);
        SelectedRange[ch] = shnt_1000X;
      }
      else if (ch_curr < FEC_SHNT_10X_LOWTHR)
      {
        fec_set_shunt_100x(ch+1);
        SelectedRange[ch] = shnt_100X;
      }
      else if (ch_curr < FEC_SHNT_1X_LOWTHR)
      {
        fec_set_shunt_10x(ch+1);
        SelectedRange[ch] = shnt_10X;
      }
      else
      {
        fec_set_shunt_1x(ch+1);
        SelectedRange[ch] = shnt_1X;
      }
    }
  }
}

/**
 * @brief Sets the voltage for the next point in the IV scan.
 * Actual voltage will not be exactly the same as the desired voltage
 * @param channel channel to sample.
 * @param channel_mask channels that are still active (bitmask: bit 0 = ch 1, bit 5 = ch 6)
 * @param voltage voltage to force
 */
void meas_stepV_for_IV_point(uint8_t channel, uint8_t channel_mask, float voltage, t_daq_sample_convd* convd_volt, t_daq_sample_convd* convd_curr)
{
  float ch_curr, volt_cmd;
  float Rshunt=22000.0e-6;  //current is in uA

  volt_cmd = voltage;

  dbg(Debug, "force: %f\r\n", voltage);
  for (int ch = 0; ch < FEC_NUM_CHANNELS; ch++) //don't rely on the built-in  ability of functions
  {                                             //to work with 1 or all channels, because we need to control an arbitrary number of channels
    if ((channel_mask & (1<<ch)) != 0)
    {
      if      (SelectedRange[ch] == shnt_1000X){Rshunt = 22000.0e-6;}
      else if (SelectedRange[ch] == shnt_100X) {Rshunt = 2200.0e-6;}
      else if (SelectedRange[ch] == shnt_10X)  {Rshunt = 220.0e-6;}
      else                                 {Rshunt = 22.0e-6;}
      ch_curr = daq_get_from_sample_convd_by_index(*convd_curr, ch+1);  //use previously measured current to adjust set-point voltage
      volt_cmd = - Rshunt * ch_curr + voltage;
      fec_set_force_voltage(ch+1, volt_cmd);  //set force voltage
      fec_enable_current(ch+1);               //connect current stuff to DUT
      dbg(Debug, "CH%u: Corrected V: %f\r\n", ch+1, volt_cmd);
    }
  }
}
/**
 * @brief measures a point of IV curve at an approximate voltage. Prints to main serial
 * - prints voltage and current measurement. Actual voltage will not be exactly the same as setpoint
 * @param	channel	channel to sample.
 * @param	voltage	voltage to force
 * @param	find_range	0 - try with previously used range first and adjust if necessary, 1 - find range first, then measure
 * @return current measured. used to terminate IV curve mesurements when current reaches 0
 */

void meas_get_IV_point(uint8_t channel, uint8_t channel_mask, uint32_t next_trigger_us, t_daq_sample_convd* convd_volt, t_daq_sample_convd* convd_curr){

  t_daq_sample_raw raw_volt, raw_curr;
  float ch_curr, ch_volt;
  uint32_t t1, t2;

  dbg(Debug, "MEAS:meas_get_IV_point()\r\n");

  t1 = usec_get_timestamp();

  //perform the measurement
  daq_prepare_for_sampling(prv_meas_num_avg);
  //start sampling
  daq_start_sampling();			//Measures all channels in any case
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //get raw averages from buffer
  raw_volt = daq_volt_raw_get_average(prv_meas_num_avg);
  raw_curr = daq_curr_raw_get_average(prv_meas_num_avg);
  //convert to volts and amps
  *convd_volt = daq_raw_to_volt(raw_volt);
  *convd_curr = daq_raw_to_curr(raw_curr);
  prv_meas_print_IV_point_ts(*convd_volt, *convd_curr, channel, channel_mask);

  //debug print
  for (int ch = 0; ch < FEC_NUM_CHANNELS; ch++) //don't rely on the built-in  ability of functions
  {                                             //to work with 1 or all channels, because we need to control an arbitrary number of channels
    if ((channel_mask & (1<<ch)) != 0)
    {
      ch_curr = daq_get_from_sample_convd_by_index(*convd_curr, ch+1);  //use previously measured current to adjust set-point voltage
      ch_volt = daq_get_from_sample_convd_by_index(*convd_volt, ch+1);  //use previously measured current to adjust set-point voltage
      dbg(Debug, "CH%u: measured V=%f I=%f\r\n", ch+1, ch_volt, ch_curr);
    }
  }
  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:meas_get_IV_point() took: %lu usec\r\n", t2-t1);

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

  dbg(Debug, "prv_meas_dump_from_buffer_human_readable_volt()\r\n");
  //check channel number
  assert_param(channel <= 6);

  //print ident
  prv_meas_print_data_ident_dump_text_volt();
  //print timestamp (of first element
  sample_raw = daq_get_from_buffer_volt(0);
  prv_meas_print_timestamp(sample_raw.timestamp);
  //print sample time
  mainser_printf("TS[us]:%f\r\n", (float)DAQ_SAMPLE_TIME_100KSPS);
  //print channel map
  prv_meas_print_ch_ident(channel,0);

  for(uint32_t n = 0 ; n< num_samples ; n++){
    //get sample from buffer (all channels)
    sample_raw = daq_get_from_buffer_volt(n);
    //convert to volts
    sample_convd = daq_raw_to_volt(sample_raw);
    //print to serial
    mainser_printf("[%lu]", n);
    prv_meas_print_sample(sample_convd, channel);

  }
  //print END_DUMP
  prv_meas_print_dump_end();

  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:prv_meas_dump_from_buffer_human_readable_volt() took: %lu usec\r\n", t2-t1);
}

/**
 * @brief Dumps current buffer (converted to uA) (num_samples points starting at 0) to main serial in human readable format
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

  dbg(Debug, "prv_meas_dump_from_buffer_human_readable_curr()\r\n");
  //check channel number
  assert_param(channel <= 6);

  //print ident
  prv_meas_print_data_ident_dump_text_curr();
  //print timestamp (of first element
  sample_raw = daq_get_from_buffer_curr(0);
  prv_meas_print_timestamp(sample_raw.timestamp);
  //print sample time
  mainser_printf("TS[us]:%f\r\n", (float)DAQ_SAMPLE_TIME_100KSPS);
  //print channel map
  prv_meas_print_ch_ident(channel,0);

  for(uint32_t n = 0 ; n< num_samples ; n++){
    //get sample from buffer (all channels)
    sample_raw = daq_get_from_buffer_curr(n);
    //convert to current uA
    sample_convd = daq_raw_to_curr(sample_raw);
    //print to serial
    mainser_printf("[%lu]", n);
    prv_meas_print_sample(sample_convd, channel);
  }
  //print END_DUMP
  prv_meas_print_dump_end();

  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:prv_meas_dump_from_buffer_human_readable_curr() took: %lu usec\r\n", t2-t1);
}

/**
 * @brief Dumps current buffer (converted to uA) (num_samples points starting at 0) to main serial in human readable format
 * - call with channel = 0 to dump all channels
 * @param channel channel to dump
 * @param num_samples number of samples to dump
 *
 */
void prv_meas_dump_from_buffer_human_readable_iv(uint8_t channel, uint32_t num_samples){
  uint32_t t1, t2;
  t_daq_sample_raw sample_raw_volt, sample_raw_curr;
  t_daq_sample_convd sample_convd_volt, sample_convd_curr;
  //float ch_volt;

  t1 = usec_get_timestamp();

  dbg(Debug, "prv_meas_dump_from_buffer_human_readable_curr()\r\n");
  //check channel number
  assert_param(channel <= 6);

  //print ident
  prv_meas_print_data_ident_dump_text_IV();
  //print timestamp (of first element
  sample_raw_volt = daq_get_from_buffer_volt(0);
  prv_meas_print_timestamp(sample_raw_volt.timestamp);
  //print sample time
  mainser_printf("TS[us]:%f\r\n", (float)DAQ_SAMPLE_TIME_100KSPS);
  //print channel map
  prv_meas_print_ch_ident(channel,0);

  for(uint32_t n = 0 ; n< num_samples ; n++){
    //get sample from buffer (all channels)
    sample_raw_volt = daq_get_from_buffer_volt(n);
    sample_raw_curr = daq_get_from_buffer_curr(n);
    //convert to current V/uA
    sample_convd_volt = daq_raw_to_volt(sample_raw_volt);
    sample_convd_curr = daq_raw_to_curr(sample_raw_curr);
    //print to serial
    mainser_printf("[%lu]", n);
    prv_meas_print_IV_point(sample_convd_volt, sample_convd_curr, channel);
  }
  //print END_DUMP
  prv_meas_print_dump_end();

  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:prv_meas_dump_from_buffer_human_readable_curr() took: %lu usec\r\n", t2-t1);
}

/**
 * @brief prints timestamp
 * @param timestamp timestamp to print
 */
void prv_meas_print_timestamp(uint64_t timestamp){
  mainser_printf("TIME:%llu\r\n", timestamp);
}

/**
 * @brief prints channel identification
 * - CH1 if single or CH1:CH2:CH3:CH4:CH5:CH6 if all
 * @param timestamp timestamp to print
 */
void prv_meas_print_ch_ident(uint8_t channel, uint8_t sample_timestamp){
  if (sample_timestamp == 0)
  {
    if(channel== 0){
    mainser_printf("CH1:CH2:CH3:CH4:CH5:CH6\r\n");
    }
    else{
    mainser_printf("CH%u\r\n", channel);
    }
  }
  else
  {
    if(channel== 0){
    mainser_printf("CH1:CH2:CH3:CH4:CH5:CH6:t\r\n");
    }
    else{
    mainser_printf("CH%u:t\r\n", channel);
    }
  }
}

/**
 * @brief prints channel identification as per channel mask (any number of channels from 1 to 6)
 * - CH1 if single or CH1:CH2:CH3:CH4:CH5:CH6 if all
 * @param timestamp timestamp to print
 */
void prv_meas_print_ch_ident_by_mask(uint8_t channel_mask, uint8_t sample_timestamp){
  char str[30];
  int str_idx=0;
  for (int ch=0; ch<FEC_NUM_CHANNELS; ch++)
  {
    if ( (channel_mask & (1<<ch)) != 0 )
    {
      str_idx += sprintf(&str[str_idx],"CH%u:",ch+1);
    }
  }
  if (sample_timestamp != 0)
  {
    sprintf(&str[str_idx],"t\r\n");
  }
  else
  {
    sprintf(&str[str_idx-1],"\r\n");
  }
  mainser_printf(str);
}

/**
 * @brief prints data identification VOLT
 */
void prv_meas_print_data_ident_voltage(void){
  mainser_printf("VOLT[V]:\r\n");
}

/**
 * @brief prints data identification VOLT
 */
void prv_meas_print_data_ident_current(void){
  mainser_printf("CURR[uA]:\r\n");
}

/**
 * @brief prints data identification IV point
 */
void prv_meas_print_data_ident_IV_point(void){
  mainser_printf("IV[uA__V]:\r\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_dump_text_volt(void){
  mainser_printf("DUMPVOLT[V]:\r\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_dump_text_curr(void){
  mainser_printf("DUMPCURR[uA]:\r\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_dump_text_IV(void){
  mainser_printf("DUMPIVPT[uA__V]:\r\n");
}

/**
 * @brief prints data identification human readable text buffer dump
 */
void prv_meas_print_data_ident_IV_characteristic(void){
  mainser_printf("IVCHAR[uA__V]:\r\n");
}

/**
 * @brief prints data identification human readable MPP point
 */
void prv_meas_print_data_ident_MPP(void){
  mainser_printf("MPPT[uA__V]:\r\n");
}

/**
 * @brief prints data identification
 */
void prv_meas_print_data_ident_flashmeasure_single(void){
  mainser_printf("FLASHMEAS_SINGLE:\r\n");
}

/**
 * @brief prints data identification
 */
void prv_meas_print_data_ident_flashmeasure_dump(void){
  mainser_printf("FLASHMEAS_DUMP:\r\n");
}

/**
 * @brief Measure IV characteristic of DUT. Prints results to main serial
 * - !!! this is limited to single channel measurements !!!
 * @param channel channel to measure
 * @param start_volt start (low) voltage
 * @param end_volt end (high) voltage
 * @param step_volt step voltage
 */
void meas_get_iv_characteristic(uint8_t channel, float start_volt, float end_volt, float step_volt, uint32_t step_time, uint32_t Npoints_per_step){
  uint32_t t1, t2;
  uint32_t max_num_iv_points;
  uint32_t micro_step_time_us;
  uint64_t next_trigger_us;
  uint8_t inProgress;
  float voltHistory[6][3];
  float setp;
  float curr;
  float volt;
  t_daq_sample_convd convd_volt, convd_curr;

  memset(voltHistory,0,sizeof(voltHistory));
  dbg(Debug, "MEAS:meas_get_iv_characteristic()\r\n");
  assert_param(channel <= FEC_NUM_CHANNELS && channel >= 0);

  if (channel == 0) inProgress = 0x3F;
  else inProgress = 1<<(channel-1);
  t1 = usec_get_timestamp();

  autorange_IV_point(channel, start_volt, step_time, &convd_volt, &convd_curr);  //autorange first, then mark the beginning of the IV scan

  micro_step_time_us = (step_time*1000) / Npoints_per_step;
  prv_meas_start_timestamp = usec_get_timestamp_64() + micro_step_time_us;
  next_trigger_us = prv_meas_start_timestamp;
  //print ident
  //timestamp is just start of measurements, not for sample points
  prv_meas_print_data_ident_IV_characteristic();
  prv_meas_print_timestamp(prv_meas_start_timestamp);
  prv_meas_print_ch_ident(channel,1);

  //calculate number of points we need to take and double them to set the maximum number of steps to be taken
  max_num_iv_points = (uint32_t)((end_volt - start_volt)/step_volt) * 2;
  dbg(Debug, "IV characteristic max num of points: %lu\r\n", max_num_iv_points);

  //max number of points should never be reached
  //other conditions should finish the scan before that (OC, or voltage not changing anymore)
  for(uint32_t n = 0 ; n < max_num_iv_points ; n++)
  {
    setp = start_volt + n*step_volt;
    meas_stepV_for_IV_point(channel,  inProgress, setp, &convd_volt, &convd_curr);

    for (int m = 0; m < Npoints_per_step; m++)
    {
      mainser_printf("[%lu]", n*Npoints_per_step+m);
      meas_get_IV_point(channel, inProgress, next_trigger_us, &convd_volt, &convd_curr);
      next_trigger_us += micro_step_time_us;
    }
    //adjust_range_IV_point(channel, inProgress, &convd_curr);

    //after each burst of micro-steps, check if any of the channels has finished the scan
    for (int ch=0; ch < FEC_NUM_CHANNELS; ch++)
    {
    	if ((inProgress & (1<<ch)) == 0) continue;	//skip finished channels

    	curr = daq_get_from_sample_convd_by_index(convd_curr, ch+1);
    	volt = daq_get_from_sample_convd_by_index(convd_volt, ch+1);

    	//is current (practically) zero? (OC condition reached)
    	if(curr < MEAS_IV_CHAR_MIN_CURR_THR){
        dbg(Debug, "CH%u: Current below threshold. Stopping IV char measurements\r\n",ch+1);
        inProgress &= ~(1<<ch);	//clear appropriate in-progress flag
        fec_disable_current(ch+1);
        fec_set_force_voltage(ch+1, 0);
        fec_set_shunt_1000x(ch+1);
        continue;
      }
    	//has the maximum voltage been reached?
    	if (volt >= end_volt) {
        dbg(Debug, "CH%u: End voltage reached. Stopping IV char measurements\r\n",ch+1);
        inProgress &= ~(1<<ch); //clear appropriate in-progress flag
        fec_disable_current(ch+1);
        fec_set_force_voltage(ch+1, 0);
        fec_set_shunt_1000x(ch+1);
        continue;
    	}
#if 0 /* This type of detection does not work here, because the voltage source is not the solar cell
       * but an active circuit that forces the desired voltage onto the cell */
    	//is voltage still changing? Check last 3 voltages compared to the current voltage
    	int VoltageStillChanging=1;
    	if (n > MEAS_IV_CHAR_MIN_STEPS_THR)	//only check for voltage not changing after a minimum number of steps
    	{
    		VoltageStillChanging=0;
    		for (int i=0; i<3; i++)
    		{
    			if (fabsf(voltHistory[ch][i]-volt) > MEAS_IV_CHAR_MIN_DELTA_V)
    				VoltageStillChanging = 1;
    		}
    	}
    	voltHistory[ch][n%3] = volt;
    	if (!VoltageStillChanging) inProgress &= ~(1<<ch);	//clear appropriate in-progress flag
#endif
    }
    if (inProgress == 0) break;
  }

  mainser_printf("END_IVCHAR\r\n");

  prv_meas_start_timestamp = 0;
  t2 = usec_get_timestamp();
  dbg(Debug, "MEAS:meas_get_iv_characteristic() took: %lu usec\r\n", t2-t1);
}

/**
 * @brief prints end dump identificator
 */
void prv_meas_print_dump_end(void){
  mainser_printf("END_DUMP\r\n");
}

/**
 * @brief measures num_samples samples of voltage and dumps in human-readable format to main serial
 * call with channel=0 for all channels at once
 * !!! Does not change current enable, shunts, force voltage or do any autoranging !!!
 * @param channel channel to measure
 * @param num_samples number of samples to measure
 */
void meas_volt_sample_and_dump(uint8_t channel, uint32_t num_samples){
  //prepare for sampling
  daq_prepare_for_sampling(num_samples);
  //start sampling
  daq_start_sampling();
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //dump data
  prv_meas_dump_from_buffer_human_readable_volt(channel, num_samples);
}

/**
 * @brief measures num_samples samples of current and dumps in human-readable format to main serial
 * call with channel=0 for all channels at once
 * !!! Does not change current enable, shunts, force voltage or do any autoranging !!!
 * @param channel channel to measure
 * @param num_samples number of samples to measure
 */
void meas_curr_sample_and_dump(uint8_t channel, uint32_t num_samples){
  //prepare for sampling
  daq_prepare_for_sampling(num_samples);
  //start sampling
  daq_start_sampling();
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //dump data
  prv_meas_dump_from_buffer_human_readable_curr(channel, num_samples);
}

/**
 * @brief measures num_samples samples of current-voltage point and dumps in human-readable format to main serial
 * call with channel=0 for all channels at once
 * !!! Does not change current enable, shunts, force voltage or do any autoranging !!!
 * @param channel channel to measure
 * @param num_samples number of samples to measure
 */
void meas_iv_sample_and_dump(uint8_t channel, uint32_t num_samples){
  //prepare for sampling
  daq_prepare_for_sampling(num_samples);
  //start sampling
  daq_start_sampling();
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //dump data
  prv_meas_dump_from_buffer_human_readable_iv(channel, num_samples);
}

/**
 * @brief Does a flash measurement: LED on, wait to settle, measure DUT voltage, LED off. Should take max a couple of ms
 * Warning: take care that sampling does not take longer than flash duration
 * @param channel channel to measure
 * @param illum number of samples to measure
 * @param flash_dur_us duration of flash in us
 * @param measure_at_us time in us to measure after flash start
 * @param numavg number of samples to take and average
 */
void meas_flashmeasure_singlesample(uint8_t channel, float illum, uint32_t flash_dur_us, uint32_t measure_at_us, uint32_t numavg){
  float curr_set;
  uint32_t ton, tmeas, toff;
  //get current for specified illumination
  curr_set = ledctrl_illumination_to_current(illum);
  //compensate current for temperature
  curr_set = ledctrl_compensate_current_for_temp(curr_set);
  //prepare for sampling
  daq_prepare_for_sampling(numavg);
  //set current. LED is now on
  ledctrl_set_current_tempcomp(curr_set);
  //save LED on time
  ton = usec_get_timestamp();
  //calculate at what time to measure and turn-off time
  tmeas = ton + measure_at_us;
  toff = ton + flash_dur_us;
  //wait for measurement time
  while(usec_get_timestamp() < tmeas);
  //measure
  daq_start_sampling();
  //wait off time
  while(usec_get_timestamp() < toff);
  //turn off LED. LED is now off
  ledctrl_set_current_tempcomp(0.0f);

  //check if sampling is finished
  if(!daq_is_sampling_done()){
    dbg(Error, "MEAS:meas_flashmeasure_singlesample(): LED off before sampling finished!\r\n");
    //wait to finish sampling
    while(!daq_is_sampling_done());
  }

  t_daq_sample_raw avg_raw = daq_volt_raw_get_average(numavg);
  t_daq_sample_convd avg_convd = daq_raw_to_volt(avg_raw);

  //print out sample
  //check if out of range
  meas_check_out_of_rng_volt(avg_convd, channel);
  //print sample
  prv_meas_print_data_ident_flashmeasure_single();
  prv_meas_print_ch_ident(channel,0);
  prv_meas_print_timestamp(avg_convd.timestamp);
  prv_meas_print_sample(avg_convd, channel);
}

/**
 * @brief Flash measurement with constant sampling of voltage
 * Warning: take care that sampling does not take longer than flash duration
 * @param channel channel to measure
 * @param illum number of samples to measure
 * @param flash_dur_us duration of flash in us
 * @param measure_at_us time in us to measure after flash start
 */
void meas_flashmeasure_dumpbuffer(uint8_t channel, float illum, uint32_t flash_dur_us){
  float curr_set;
  uint32_t ton, toff, tmeas_start, num_samples;
  //get current for specified illumination
  curr_set = ledctrl_illumination_to_current(illum);
  //compensate current for temperature
  curr_set = ledctrl_compensate_current_for_temp(curr_set);
  //calculate number of samples
  num_samples = (flash_dur_us+(2*MEAS_FLASH_DUMP_SAMPLEBORDER_US)) / DAQ_SAMPLE_TIME_100KSPS;
  //prepare for sampling
  daq_prepare_for_sampling(num_samples);
  //start sampling
  daq_start_sampling();
  //save start sampling time
  tmeas_start = usec_get_timestamp();
  //calculate LED on and off time
  ton = tmeas_start+MEAS_FLASH_DUMP_SAMPLEBORDER_US;
  toff = ton + flash_dur_us;
  //wait for LED on
  while(usec_get_timestamp() < ton);
  //set current. LED is now on
  ledctrl_set_current_tempcomp(curr_set);
  //wait for LED off
  while(usec_get_timestamp() < toff);
  //turn off LED. LED is now off
  ledctrl_set_current_tempcomp(0.0f);
  //wait for sampling to finish
  while(!daq_is_sampling_done());

  //dump data
  prv_meas_print_data_ident_flashmeasure_dump();
  prv_meas_dump_from_buffer_human_readable_volt(channel, num_samples);
}


/**
 * @brief measures I and V for MPPT
 * @param Navg  Number of measurements to average
 * @param *convd_volt  pointer to where to store voltage results
 * @param *convd_curr  pointer to where to store current results
 */

void meas_mpp_IV_point(uint32_t Navg, t_daq_sample_convd* convd_volt, t_daq_sample_convd* convd_curr){

  t_daq_sample_raw raw_volt, raw_curr;
  uint32_t t1, t2;

  dbg(Debug, "MPPT:meas_mpp_IV_point()\r\n");

  t1 = usec_get_timestamp();

  //perform the measurement
  daq_prepare_for_sampling(Navg);
  //start sampling
  daq_start_sampling();     //Measures all channels in any case
  //wait for sampling to finish
  while(!daq_is_sampling_done());
  //get raw averages from buffer
  raw_volt = daq_volt_raw_get_average(Navg);
  raw_curr = daq_curr_raw_get_average(Navg);
  //convert to volts and amps
  *convd_volt = daq_raw_to_volt(raw_volt);
  *convd_curr = daq_raw_to_curr(raw_curr);

  t2 = usec_get_timestamp();
  dbg(Debug, "MPPT:meas_mpp_IV_point() took: %lu usec\r\n", t2-t1);

}


uint8_t MpptOn = MPPT_ALL_OFF;
uint64_t MpptPeriod = 10000;
uint64_t NextMpptExecutionTime = 0;
uint32_t MpptReportEveryXthPoint = 0;
uint32_t MpptReportCnt;
int8_t Udir = +1; //+1 increasing direction, -1 decreasing direction
float Pold[FEC_NUM_CHANNELS];
float MPPT_VsetPoint[FEC_NUM_CHANNELS];
float MaxMPPVoltage = MPPT_MAX_VOLTAGE_DEFAULT;
enum shntEnum MPPTRange[6]={shnt_10X,shnt_10X,shnt_10X,shnt_10X,shnt_10X,shnt_10X};

void mppt_start(uint8_t channel, uint32_t settling_time, uint32_t report_every_xth_point)
{
  t_daq_sample_convd convd_volt;
  t_daq_sample_convd convd_curr;
  float curr;
  float volt;
  float Pnew;
  float Ithreshold;
  int DirReverseCounter;
  float voltageStep[FEC_NUM_CHANNELS];
  uint8_t MpptFound=0;
  uint32_t t1,t2;

  dbg(Debug, "\r\nMPPT:Starting...\r\n");
  t1 = usec_get_timestamp();
  MpptPeriod = settling_time;
  MpptReportEveryXthPoint = report_every_xth_point;
  MpptReportCnt = 0;
  if (channel == 0) MpptOn = MPPT_ALL_ON;
  else MpptOn = 1<<(channel-1);
  NextMpptExecutionTime = 0;

  //select range for MPP
  autorange_IV_point(channel, 0, settling_time, &convd_volt, &convd_curr);
  for (int ch=0; ch<FEC_NUM_CHANNELS; ch++) MPPTRange[ch] = SelectedRange[ch];

  //Measure Voc
  dbg(Debug, "MPPT:Measuring Voc\r\n");
  fec_disable_current(channel); //0 = All channels
  usec_delay(settling_time);
  meas_mpp_IV_point(1, &convd_volt, &convd_curr);   //Only a single measurement, as this will be used only as a first guess for Vmpp
  for (int ch=0; ch<FEC_NUM_CHANNELS; ch++)
  {
    if ( (MpptOn & (1<<ch)) != 0 )
    {
      volt = daq_get_from_sample_convd_by_index(convd_volt, ch+1);
      MPPT_VsetPoint[ch] = MPPT_FVOC * volt;
      voltageStep[ch] = MPPT_SEARCH_VOLTAGE_STEP;
      dbg(Debug, "MPPT CH%u:Voc=%.3fV,VmppStart=%.3fV\r\n",ch+1, volt, MPPT_VsetPoint[ch]);
      fec_set_force_voltage(ch+1, MPPT_VsetPoint[ch]);  //set force voltage
    }
  }
  fec_enable_current(channel); //0 = All channels

  DirReverseCounter = 0;
  for (int i=0; i<MPPT_MAX_START_STEPS; i++)
  {
    usec_delay(settling_time/10); //step quite quickly (10x faster than normal)
    meas_mpp_IV_point(prv_meas_num_avg, &convd_volt, &convd_curr);
    for (int ch=0; ch < FEC_NUM_CHANNELS; ch++)
    {
      if ( (MpptOn & (1<<ch)) != 0 )
      {
        curr = daq_get_from_sample_convd_by_index(convd_curr, ch+1);
        volt = daq_get_from_sample_convd_by_index(convd_volt, ch+1);

        //Check current. If too low, decrease voltage
        Pnew = curr * volt;
        dbg(Debug, "MPPT step %02d, CH%u:V=%.3fV, I=%.3fuA, P=%.3fuW\r\n",i ,ch+1, volt, curr, Pnew);
        switch (MPPTRange[ch])
        {
          case shnt_1X:    Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_1X_LOWTHR/0.08;   break;
          case shnt_10X:   Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_10X_LOWTHR/0.08;  break;
          case shnt_100X:  Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_100X_LOWTHR/0.08; break;
          case shnt_1000X:
          default:         Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_100X_LOWTHR/0.8;  break;
        }
        //check for minimum voltage (0V)
        if (volt < 0.0)
        {
          dbg(Debug, "V<0\r\n");
          Udir = +1;
        }
        //check for maximum voltage set by the user
        else if (volt > MaxMPPVoltage)
        {
          dbg(Debug, "V>Max\r\n");
          Udir = -1;
        }
        else if (curr < Ithreshold)
        {
          dbg(Debug, "I<min (%.3fuA)\r\n",Ithreshold);
          Udir = -1; //if current is too small, ignore power, reduce voltage
        }
        //*** BASIC MPPT ***
        else if (Pnew < Pold[ch])
        {
          Udir *= -1; //if new power is smaller than previous, reverse direction and reduce voltage step size to half
          voltageStep[ch] /= 2;
          if (voltageStep[ch] < MPPT_VOLTAGE_STEP) voltageStep[ch] = MPPT_VOLTAGE_STEP;
          DirReverseCounter++;
          if (DirReverseCounter > MPPT_DIR_REVERSE_THRESHOLD) MpptFound |= (1<<ch); //No need to stop adjusting this channel. Just flag that MPP has been reached.
          dbg(Debug, "Pnew<Pold: dir=%+d, step=%.3fV, revCnt=%d\r\n",Udir, voltageStep[ch],DirReverseCounter);
        }
        MPPT_VsetPoint[ch] += Udir * voltageStep[ch];
        //*** BASIC MPPT END ***
        //check voltage limits:
        if (MPPT_VsetPoint[ch] > MPPT_VMAX) MPPT_VsetPoint[ch] = MPPT_VMAX;
        if (MPPT_VsetPoint[ch] < MPPT_VMIN) MPPT_VsetPoint[ch] = MPPT_VMIN;

        //Apply new voltage
        fec_set_force_voltage(ch+1, MPPT_VsetPoint[ch]);  //set force voltage
        dbg(Debug, "New SetPointV=%.3f\r\n",MPPT_VsetPoint[ch]);
        Pold[ch] = Pnew;
      }
    }
    if ((MpptFound & MpptOn) == MpptOn) break;
  }
  t2 = usec_get_timestamp();
  dbg(Debug, "MPPT first MPP finding took: %lu usec\r\n", t2-t1);
}

void mppt_resume()
{
  MpptOn = MPPT_ALL_ON;
  for (int ch=0; ch<FEC_NUM_CHANNELS; ch++)
  {
    switch (MPPTRange[ch])
    {
      case shnt_1X:    fec_set_shunt_1x(ch+1); break;
      case shnt_10X:   fec_set_shunt_10x(ch+1); break;
      case shnt_100X:  fec_set_shunt_100x(ch+1); break;
      case shnt_1000X: fec_set_shunt_1000x(ch+1); break;
      default:
        break;
    }
  }
}

void mppt_stop()
{
  MpptOn = MPPT_ALL_OFF;
}

void prv_meas_print_mpp(uint8_t channel_mask, t_daq_sample_convd *sample_volt, t_daq_sample_convd *sample_curr)
{
  int not_first=0;
  if (MpptReportEveryXthPoint == 0) return;
  MpptReportCnt++;
  if (MpptReportCnt >= MpptReportEveryXthPoint)
  {
    MpptReportCnt=0;
    if(sample_volt->timestamp != sample_curr->timestamp){
      dbg(Warning,"prv_meas_print_volt_and_curr(): Volt/Curr timestamps not equal!\r\n");
      //I am guessing this can happen on occasion, if the timer
    }
    prv_meas_print_data_ident_MPP();
    prv_meas_print_timestamp(sample_volt->timestamp);
    prv_meas_print_ch_ident_by_mask(channel_mask,0);

    for (int ch=0; ch < FEC_NUM_CHANNELS; ch++)
    {
      if ( (MpptOn & (1<<ch)) != 0 )
      {
        if (not_first) mainser_printf(":");
        not_first = 1;
        switch(ch+1){
          case 1: mainser_printf("%f_%f", sample_curr->ch1, sample_volt->ch1); break;
          case 2: mainser_printf("%f_%f", sample_curr->ch2, sample_volt->ch2); break;
          case 3: mainser_printf("%f_%f", sample_curr->ch3, sample_volt->ch3); break;
          case 4: mainser_printf("%f_%f", sample_curr->ch4, sample_volt->ch4); break;
          case 5: mainser_printf("%f_%f", sample_curr->ch5, sample_volt->ch5); break;
          case 6: mainser_printf("%f_%f", sample_curr->ch6, sample_volt->ch6); break;
          default:
            break;
        }
      }
    }
    mainser_printf("\r\n");
  }
}

void mppt()
{
  t_daq_sample_convd convd_volt;
  t_daq_sample_convd convd_curr;
  float curr;
  float volt;
  float Pnew;
  float Ithreshold;

  if (MpptOn == MPPT_ALL_OFF) return;
  if (usec_get_timestamp_64() < NextMpptExecutionTime) return;
  NextMpptExecutionTime = usec_get_timestamp_64() + MpptPeriod;

  meas_mpp_IV_point(prv_meas_num_avg, &convd_volt, &convd_curr);
  for (int ch=0; ch < FEC_NUM_CHANNELS; ch++)
  {
    if ( (MpptOn & (1<<ch)) != 0 )
    {
      curr = daq_get_from_sample_convd_by_index(convd_curr, ch+1);
      volt = daq_get_from_sample_convd_by_index(convd_volt, ch+1);
      Pnew = curr * volt;

      dbg(Debug, "MPPT CH%u:V=%.3fV, I=%.3fuA, P=%.3fuW\r\n",ch+1, volt, curr, Pnew);
      switch (MPPTRange[ch])
      {
        case shnt_1X:    Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_1X_LOWTHR/0.08;   break;
        case shnt_10X:   Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_10X_LOWTHR/0.08;  break;
        case shnt_100X:  Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_100X_LOWTHR/0.08; break;
        case shnt_1000X:
        default:         Ithreshold = MPPT_IMIN_OF_RANGE * FEC_SHNT_100X_LOWTHR/0.8;  break;
      }
      //check for minimum voltage (0V)
      if (volt < 0.0)
      {
        dbg(Warning, "V<0\r\n");
        Udir = +1;
      }
      //check for maximum voltage set by the user
      else if (volt > MaxMPPVoltage)
      {
        dbg(Warning, "V>Max\r\n");
        Udir = -1;
      }
      //Check current. If too low, decrease voltage
      else if (curr < Ithreshold)
      {
        dbg(Warning, "I<min (%.3fuA)\r\n",Ithreshold);
        Udir = -1; //if current is too small, ignore power, reduce voltage
      }
      //*** BASIC MPPT ***
      else if (Pnew < Pold[ch])
      {
        Udir *= -1; //if new power is smaller than previous, reverse direction and reduce voltage step size to half
        dbg(Debug, "Pnew<Pold: dir=%+d\r\n",Udir);
      }
      Pold[ch]=Pnew;
      MPPT_VsetPoint[ch] += Udir * MPPT_VOLTAGE_STEP;
      //*** BASIC MPPT END ***
      //check voltage limits:
      if (MPPT_VsetPoint[ch] > MPPT_VMAX) MPPT_VsetPoint[ch] = MPPT_VMAX;
      if (MPPT_VsetPoint[ch] < MPPT_VMIN) MPPT_VsetPoint[ch] = MPPT_VMIN;

      //Apply new voltage
      fec_set_force_voltage(ch+1, MPPT_VsetPoint[ch]);  //set force voltage
      dbg(Debug, "New SetPointV=%.3f\r\n",MPPT_VsetPoint[ch]);

    }
  }
  //Report mppt point
  prv_meas_print_mpp(MpptOn, &convd_volt, &convd_curr);
}

/**
 * @brief end of test sequence
 * todo: implement
 */
void meas_end_of_sequence(void){
  mainser_printf("END_OF_SEQUENCE\r\n");
}
