//
// Created by Matej Planinšek on 30/07/2023.
//

#include "global_callbacks.h"
#include "UserGPIO.h"


//timer period elapsed callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  //check if the interrupt is from ADC trigger timer
  if(htim->Instance == TIM20)
  {
    // Your code here
	//D1Tgl();
  }

  //check if the interrupt is from the usec timer
  else if(htim->Instance == TIM2){
    //call overflow callback
    prv_usec_overflow_callback();
  }
}

//adc conversion complete callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  if(hadc->Instance == DAQ_VOLT_ADC){
    daq_sampling_done_volt = 1;
    daq_sampling_volt_done_timestamp = usec_get_timestamp_64();
    //HAL_GPIO_WritePin(DBG_PAD_2_GPIO_Port, DBG_PAD_2_Pin, GPIO_PIN_RESET);
    D2Off();
    //turn off LED
    //HAL_GPIO_WritePin(DBG_LED_2_GPIO_Port, DBG_LED_2_Pin, GPIO_PIN_RESET);
    L2Off();
  }
  else if(hadc->Instance == DAQ_CURR_ADC){
    daq_sampling_done_curr = 1;
    daq_sampling_curr_done_timestamp = usec_get_timestamp_64();
  }
}
