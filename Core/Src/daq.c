//
// Created by Matej Planinšek on 30/07/2023.
//

#include "daq.h"

// voltage ADC buffer
volatile uint16_t daq_buffer_volt[DAQ_BUFF_SIZE] = {0};
volatile uint16_t daq_buffer_curr[DAQ_BUFF_SIZE] = {0};

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
  daq_sampling_done_volt = 0;
  daq_sampling_done_curr = 0;
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
  //check if sample number possible
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
                    (uint32_t*)daq_buffer_volt,
                    num_samples*DAQ_NUM_CH);
  HAL_ADC_Start_DMA(DAQ_CURR_ADC_HANDLE,
                    (uint32_t*)daq_buffer_curr,
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
  return daq_sampling_volt_done_timestamp - DAQ_SAMPLE_TIME_100KSPS*(prv_daq_num_samples-1);
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
  sample.ch1 = daq_buffer_volt[sample_idx*DAQ_NUM_CH + 0];
  sample.ch2 = daq_buffer_volt[sample_idx*DAQ_NUM_CH + 1];
  sample.ch3 = daq_buffer_volt[sample_idx*DAQ_NUM_CH + 2];
  sample.ch4 = daq_buffer_volt[sample_idx*DAQ_NUM_CH + 3];
  sample.ch5 = daq_buffer_volt[sample_idx*DAQ_NUM_CH + 4];
  sample.ch6 = daq_buffer_volt[sample_idx*DAQ_NUM_CH + 5];
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
  sample.ch1 = daq_buffer_curr[sample_idx*DAQ_NUM_CH + 0];
  sample.ch2 = daq_buffer_curr[sample_idx*DAQ_NUM_CH + 1];
  sample.ch3 = daq_buffer_curr[sample_idx*DAQ_NUM_CH + 2];
  sample.ch4 = daq_buffer_curr[sample_idx*DAQ_NUM_CH + 3];
  sample.ch5 = daq_buffer_curr[sample_idx*DAQ_NUM_CH + 4];
  sample.ch6 = daq_buffer_curr[sample_idx*DAQ_NUM_CH + 5];
  sample.timestamp = daq_get_sampling_start_timestamp()+DAQ_SAMPLE_TIME_100KSPS*sample_idx;
  return sample;
}