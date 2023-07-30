//
// Created by Matej Planinšek on 30/07/2023.
//

#include "global_callbacks.h"


//timer period elapsed callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  //check if the interrupt is from ADC trigger timer
  if(htim->Instance == TIM20)
  {
    // Your code here
    HAL_GPIO_TogglePin(DBG_PAD_1_GPIO_Port, DBG_PAD_1_Pin);
  }

  //check if the interrupt is from the usec timer
  else if(htim->Instance == TIM2){
    //call overflow callback
    prv_usec_overflow_callback();
  }
}

//adc conversion complete callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  if(hadc->Instance == ADC1)
  {
    HAL_GPIO_WritePin(DBG_PAD_2_GPIO_Port, DBG_PAD_2_Pin, GPIO_PIN_RESET);
    //adcCpltFlag = 1;
  }
}