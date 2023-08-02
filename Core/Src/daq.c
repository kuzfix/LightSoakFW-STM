//
// Created by Matej Planinšek on 30/07/2023.
//

#include "daq.h"

// voltage ADC buffer
volatile uint16_t g_daq_buffer_volt[DAQ_BUFF_SIZE] = {0};
volatile uint16_t g_daq_buffer_curr[DAQ_BUFF_SIZE] = {0};

// sampling done flags
// also indicate if sampling is in progress. If 0, sampling is in progress
volatile uint8_t daq_sampling_done_volt;
volatile uint8_t daq_sampling_done_curr;

//sampling done timestamp
volatile uint64_t daq_sampling_volt_done_timestamp;
volatile uint64_t daq_sampling_curr_done_timestamp;

uint8_t prv_daq_ready_to_sample;

uint32_t prv_daq_num_samples;


/**
 * @brief init data acquisition. Inits vars, timers, etc
 */
void daq_init(void){
  //init vars
  daq_sampling_done_volt = 1;
  daq_sampling_done_curr = 1;
  daq_sampling_volt_done_timestamp = 0;
  daq_sampling_curr_done_timestamp = 0;
  prv_daq_ready_to_sample = 0;
  prv_daq_num_samples = 0;
  //calibrate ADCs
  HAL_ADCEx_Calibration_Start(DAQ_VOLT_ADC_HANDLE, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(DAQ_CURR_ADC_HANDLE, ADC_SINGLE_ENDED);
  dbg(Debug, "DAQ: DAQ init, ADCs calibrated\n");
}

/**
 * @brief Prepares everything for sampling. Starts ADC DMA transfers
 * but sample trigger timer is stopped.
 * Start sampling with daq_start_sampling()
 * @param num_samples number of samples to take
 */
void daq_prepare_for_sampling(uint32_t num_samples){
  //check if sampling is not in progress and if sample number possible
  assert_param(daq_sampling_done_volt && daq_sampling_done_curr);
  assert_param(num_samples*DAQ_NUM_CH <= DAQ_BUFF_SIZE);

  //save how many samples we will take. Needed for timestamp calcs
  prv_daq_num_samples = num_samples;

  //todo: multiple sampling frequencies??

  //stop timer
  HAL_TIM_Base_Stop_IT(DAQ_SAMPLE_TIMER_HANDLE);
  //reset overflow IT flag just in case
  __HAL_TIM_CLEAR_IT(DAQ_SAMPLE_TIMER_HANDLE , TIM_IT_UPDATE);
  //reset tim20 counter to just before overflow
  //todo: maybe better to be without -1 ?
  __HAL_TIM_SET_COUNTER(DAQ_SAMPLE_TIMER_HANDLE, DAQ_SAMPLE_TIMER_PERIOD_100KSPS - 1);
  //start DMA transfers
  HAL_ADC_Start_DMA(DAQ_VOLT_ADC_HANDLE,
                    (uint32_t*)g_daq_buffer_volt,
                    num_samples*DAQ_NUM_CH);
  HAL_ADC_Start_DMA(DAQ_CURR_ADC_HANDLE,
                    (uint32_t*)g_daq_buffer_curr,
                    num_samples*DAQ_NUM_CH);
  //set flag that we are ready to sample
  prv_daq_ready_to_sample = 1;
  // sampling will not start untill daq_start_sampling() is called
}

/**
 * @brief Starts sampling. Timer is started and sampling begins.
 * !!! daq_prepare_for_sampling() MUST be called before this !!!
 */
void daq_start_sampling(void){
  //check if ready to sample
  assert_param(prv_daq_ready_to_sample);

  //reset flag
  prv_daq_ready_to_sample = 0;

  //reset done flags
  daq_sampling_done_volt = 0;
  daq_sampling_done_curr = 0;

  //start timer
  HAL_GPIO_WritePin(DBG_PAD_2_GPIO_Port, DBG_PAD_2_Pin, GPIO_PIN_SET);
  HAL_TIM_Base_Start_IT(DAQ_SAMPLE_TIMER_HANDLE);
  //adc sampling at every update(overflow) of TIM20
}

/**
 * @brief Check if sampling both voltage and current channels is done.
 * (should happen at the same time)
 */
uint8_t daq_is_sampling_done(void){
  return daq_sampling_done_volt && daq_sampling_done_curr;
  //todo: need to add anything to cleanup?
}


/**
 * @brief Calibrate ADCs. To be used if periodic recalibration is needed.
 * checks if no sampling is in progress.
 */
void daq_calibrate_adcs(void){
  if(daq_sampling_done_volt == 0 || daq_sampling_done_curr == 0){
    dbg(Error, "DAQ: Can't calibrate. Sampling in progress\n");
    return;
  }
  //calibrate ADCs
  HAL_ADCEx_Calibration_Start(DAQ_VOLT_ADC_HANDLE, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(DAQ_CURR_ADC_HANDLE, ADC_SINGLE_ENDED);
  dbg(Debug, "DAQ: ADCs calibrated\n");
}

/**
 * @brief Get timestamp of when sampling started.
 * @param num_samples number of samples taken
 * @return timestamp in us
 */
uint64_t daq_get_sampling_start_timestamp(void){
  //sampling time for n samples takes n-1 periods of TIM20 + 1 conversion time
  return daq_sampling_volt_done_timestamp - (uint64_t)DAQ_SAMPLE_TIME_100KSPS*(prv_daq_num_samples-1);
}


/**
 * @brief get a sample of all channels from buffer (voltage)
 * Sample index starts at 0
 * @param sanple_idx sample index to get
 * @return sample structure
 */
t_daq_sample_raw daq_get_from_buffer_volt(uint32_t sample_idx){
  t_daq_sample_raw sample;
  //check if sample_idx makes sense
  if(sample_idx >= prv_daq_num_samples){
    //send warning and return 0
    dbg(Warning, "DAQ: requested not taken sample!\n");
    sample.ch1 = 0;
    sample.ch2 = 0;
    sample.ch3 = 0;
    sample.ch4 = 0;
    sample.ch5 = 0;
    sample.ch6 = 0;
    sample.timestamp = 0;
    return sample;
  }
  sample.ch1 = g_daq_buffer_volt[sample_idx * DAQ_NUM_CH + 0]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch2 = g_daq_buffer_volt[sample_idx * DAQ_NUM_CH + 1]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch3 = g_daq_buffer_volt[sample_idx * DAQ_NUM_CH + 2]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch4 = g_daq_buffer_volt[sample_idx * DAQ_NUM_CH + 3]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch5 = g_daq_buffer_volt[sample_idx * DAQ_NUM_CH + 4]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch6 = g_daq_buffer_volt[sample_idx * DAQ_NUM_CH + 5]<<DAQ_SAMPLE_BITSIHFT;
  sample.timestamp = daq_get_sampling_start_timestamp()+DAQ_SAMPLE_TIME_100KSPS*sample_idx;
  return sample;
}

/**
 * @brief get a sample of all channels from buffer (current)
 * Sample index starts at 0
 * @param sanple_idx sample index to get
 * @return sample structure
 */
t_daq_sample_raw daq_get_from_buffer_curr(uint32_t sample_idx){
  t_daq_sample_raw sample;
  if(sample_idx >= prv_daq_num_samples){
    //send warning and return 0
    dbg(Warning, "DAQ: requested not taken sample!\n");
    sample.ch1 = 0;
    sample.ch2 = 0;
    sample.ch3 = 0;
    sample.ch4 = 0;
    sample.ch5 = 0;
    sample.ch6 = 0;
    sample.timestamp = 0;
    return sample;
  }
  sample.ch1 = g_daq_buffer_curr[sample_idx * DAQ_NUM_CH + 0]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch2 = g_daq_buffer_curr[sample_idx * DAQ_NUM_CH + 1]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch3 = g_daq_buffer_curr[sample_idx * DAQ_NUM_CH + 2]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch4 = g_daq_buffer_curr[sample_idx * DAQ_NUM_CH + 3]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch5 = g_daq_buffer_curr[sample_idx * DAQ_NUM_CH + 4]<<DAQ_SAMPLE_BITSIHFT;
  sample.ch6 = g_daq_buffer_curr[sample_idx * DAQ_NUM_CH + 5]<<DAQ_SAMPLE_BITSIHFT;
  sample.timestamp = daq_get_sampling_start_timestamp()+DAQ_SAMPLE_TIME_100KSPS*sample_idx;
  return sample;
}


/**
 * @brief convert raw sample to voltage [V]
 * @param raw raw sample structure
 * @return converted sample structure
 */
t_daq_sample_convd daq_raw_to_volt(t_daq_sample_raw raw){
  t_daq_sample_convd convd;
  convd.ch1 = (((float)raw.ch1 / DAQ_MAX_ADC_VAL) * DAQ_VREF)/DAQ_VOLT_AMP_GAIN;
  convd.ch2 = (((float)raw.ch2 / DAQ_MAX_ADC_VAL) * DAQ_VREF)/DAQ_VOLT_AMP_GAIN;
  convd.ch3 = (((float)raw.ch3 / DAQ_MAX_ADC_VAL) * DAQ_VREF)/DAQ_VOLT_AMP_GAIN;
  convd.ch4 = (((float)raw.ch4 / DAQ_MAX_ADC_VAL) * DAQ_VREF)/DAQ_VOLT_AMP_GAIN;
  convd.ch5 = (((float)raw.ch5 / DAQ_MAX_ADC_VAL) * DAQ_VREF)/DAQ_VOLT_AMP_GAIN;
  convd.ch6 = (((float)raw.ch6 / DAQ_MAX_ADC_VAL) * DAQ_VREF)/DAQ_VOLT_AMP_GAIN;
  convd.timestamp = raw.timestamp;

  return convd;
}

/**
 * @brief convert raw sample to current [mA]
 * @param raw raw sample structure
 * @return converted sample structure
 */
t_daq_sample_convd daq_raw_to_curr(t_daq_sample_raw raw){
  t_daq_sample_convd convd;
  float shunt_volt;

  //voltage measured by ADC in V
  shunt_volt = ((float)raw.ch1 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  //voltage measured by ADC in mV
  shunt_volt *= 1000;
  //shunt voltage in mV
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN;
  //current in mA
  convd.ch1 = shunt_volt / fec_get_shunt_resistance(1);

  //voltage measured by ADC in V
  shunt_volt = ((float)raw.ch2 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  //voltage measured by ADC in mV
  shunt_volt *= 1000;
  //shunt voltage in mV
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN;
  //current in mA
  convd.ch2 = shunt_volt / fec_get_shunt_resistance(2);

  //voltage measured by ADC in V
  shunt_volt = ((float)raw.ch3 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  //voltage measured by ADC in mV
  shunt_volt *= 1000;
  //shunt voltage in mV
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN;
  //current in mA
  convd.ch3 = shunt_volt / fec_get_shunt_resistance(3);

  //voltage measured by ADC in V
  shunt_volt = ((float)raw.ch4 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  //voltage measured by ADC in mV
  shunt_volt *= 1000;
  //shunt voltage in mV
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN;
  //current in mA
  convd.ch4 = shunt_volt / fec_get_shunt_resistance(4);

  //voltage measured by ADC in V
  shunt_volt = ((float)raw.ch5 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  //voltage measured by ADC in mV
  shunt_volt *= 1000;
  //shunt voltage in mV
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN;
  //current in mA
  convd.ch5 = shunt_volt / fec_get_shunt_resistance(5);

  //voltage measured by ADC in V
  shunt_volt = ((float)raw.ch6 / DAQ_MAX_ADC_VAL) * DAQ_VREF;
  //voltage measured by ADC in mV
  shunt_volt *= 1000;
  //shunt voltage in mV
  shunt_volt = shunt_volt / DAQ_SHUNT_AMP_GAIN;
  //current in mA
  convd.ch6 = shunt_volt / fec_get_shunt_resistance(6);

  convd.timestamp = raw.timestamp;

  return convd;
}

/**
 * @brief get average of raw voltage samples from buffer
 * timestamp is middle of first and last sample
 * !! this function reads stuff from buffer and can only be used after sampling finished !!
 * @param num_samples number of samples to average
 * @return average sample (for all channels)
 */
t_daq_sample_raw daq_volt_raw_get_average(uint32_t num_samples){
  //check if accumulator is big enough
  assert_param(num_samples < 65535);

  uint32_t t1, t2;

  //accumulator for each channel
  uint32_t sum_array[DAQ_NUM_CH] = {0};
  t_daq_sample_raw avg_sample;

  t1 = usec_get_timestamp();

  for(uint32_t n = 0 ; n<num_samples ; n++){
    //add to accumulator
    sum_array[0] += g_daq_buffer_volt[n * DAQ_NUM_CH + 0]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[1] += g_daq_buffer_volt[n * DAQ_NUM_CH + 1]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[2] += g_daq_buffer_volt[n * DAQ_NUM_CH + 2]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[3] += g_daq_buffer_volt[n * DAQ_NUM_CH + 3]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[4] += g_daq_buffer_volt[n * DAQ_NUM_CH + 4]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[5] += g_daq_buffer_volt[n * DAQ_NUM_CH + 5]<<DAQ_SAMPLE_BITSIHFT;
  }
  avg_sample.ch1 = sum_array[0] / num_samples;
  avg_sample.ch2 = sum_array[1] / num_samples;
  avg_sample.ch3 = sum_array[2] / num_samples;
  avg_sample.ch4 = sum_array[3] / num_samples;
  avg_sample.ch5 = sum_array[4] / num_samples;
  avg_sample.ch6 = sum_array[5] / num_samples;

  //calculate timestamp.
  avg_sample.timestamp = daq_get_sampling_start_timestamp() + (num_samples/2)*DAQ_SAMPLE_TIME_100KSPS;


  t2 = usec_get_timestamp();

  //print averaging time
//  dbg(Debug, "Averaging %d samples took %d usec\n", num_samples, t2-t1);

  return avg_sample;
}

/**
 * @brief get average of raw current samples from buffer
 * @param num_samples number of samples to average
 * timestamp is middle of first and last sample
 * @return average sample (for all channels)
 */
t_daq_sample_raw daq_curr_raw_get_average(uint32_t num_samples){
  //check if accumulator is big enough
  assert_param(num_samples < 65535);

  uint32_t t1, t2;

  //accumulator for each channel
  uint32_t sum_array[DAQ_NUM_CH] = {0};
  t_daq_sample_raw avg_sample;

  t1 = usec_get_timestamp();

  for(uint32_t n = 0 ; n<num_samples ; n++){
    //add to accumulator
    sum_array[0] += g_daq_buffer_curr[n * DAQ_NUM_CH + 0]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[1] += g_daq_buffer_curr[n * DAQ_NUM_CH + 1]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[2] += g_daq_buffer_curr[n * DAQ_NUM_CH + 2]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[3] += g_daq_buffer_curr[n * DAQ_NUM_CH + 3]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[4] += g_daq_buffer_curr[n * DAQ_NUM_CH + 4]<<DAQ_SAMPLE_BITSIHFT;
    sum_array[5] += g_daq_buffer_curr[n * DAQ_NUM_CH + 5]<<DAQ_SAMPLE_BITSIHFT;
  }
  avg_sample.ch1 = sum_array[0] / num_samples;
  avg_sample.ch2 = sum_array[1] / num_samples;
  avg_sample.ch3 = sum_array[2] / num_samples;
  avg_sample.ch4 = sum_array[3] / num_samples;
  avg_sample.ch5 = sum_array[4] / num_samples;
  avg_sample.ch6 = sum_array[5] / num_samples;

  //calculate timestamp.
  avg_sample.timestamp = daq_get_sampling_start_timestamp() + (num_samples/2)*DAQ_SAMPLE_TIME_100KSPS;


  t2 = usec_get_timestamp();

  //print averaging time
//  dbg(Debug, "Averaging %d samples took %d usec\n", num_samples, t2-t1);

  return avg_sample;

}

/**
 * @brief take a single shot measurement of voltages
 * averages num_samples samples
 * timestamp is middle of first and last sample
 * !! WARNING: blocking function !!
 * @param num_samples number of samples to take
 * @return averaged measurement, converted to V, all channels
 */
t_daq_sample_convd daq_single_shot_volt(uint32_t num_samples){
  daq_prepare_for_sampling(num_samples);
  daq_start_sampling();
  while(!daq_is_sampling_done());
  t_daq_sample_raw avg_raw = daq_volt_raw_get_average(num_samples);
  t_daq_sample_convd avg_convd = daq_raw_to_volt(avg_raw);
  return avg_convd;
}

/**
 * @brief take a single shot measurement of currents
 * averages num_samples samples
 * timestamp is middle of first and last sample
 * !! WARNING: blocking function !!
 * !! WARNING: uses currently set shunts, no autoranging!
 * @param num_samples number of samples to take
 * @return averaged measurement, converted to mA, all channels
 */
t_daq_sample_convd daq_single_shot_curr_no_autorng(uint32_t num_samples){
  daq_prepare_for_sampling(num_samples);
  daq_start_sampling();
  while(!daq_is_sampling_done());
  t_daq_sample_raw avg_raw = daq_curr_raw_get_average(num_samples);
  t_daq_sample_convd avg_convd = daq_raw_to_curr(avg_raw);
  return avg_convd;
}