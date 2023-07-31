//
// Created by Matej Planinšek on 30/07/2023.
//

#include "micro_sec.h"


//overflow counter global variable
volatile uint32_t g_usec_overflow_count;


/**
 * @brief initializes usec timer
 */
void usec_init(void){
  //start timer
  g_usec_overflow_count = 0;
  //clear overflow interrupt flag (if not, callback called immediately)
  __HAL_TIM_CLEAR_IT(MICRO_SEC_TIM_HANDLE ,TIM_IT_UPDATE);
  HAL_TIM_Base_Start_IT(MICRO_SEC_TIM_HANDLE);
}



/**
 * @brief blocking delay function for microsecond delays
 */
void usec_delay(uint32_t delay_us){
  uint32_t start_time = __HAL_TIM_GET_COUNTER(MICRO_SEC_TIM_HANDLE);
  while(__HAL_TIM_GET_COUNTER(MICRO_SEC_TIM_HANDLE) - start_time < delay_us);
}

/**
 * @brief gets timer counter value
 */
uint32_t usec_get_timestamp(void){
  return (uint32_t)__HAL_TIM_GET_COUNTER(MICRO_SEC_TIM_HANDLE);
}

/**
 * @brief call this from period elapsed callback to cout timer overflows
 */
void prv_usec_overflow_callback(void){
  g_usec_overflow_count++;
}

/**
 * @brief returns the number of usec timer overflows
 */
uint32_t usec_get_overflow_count(void){
  return g_usec_overflow_count;
}


/**
 * @brief returns the current timestamp in 64bit format
 * @warning
 */
uint64_t usec_get_timestamp_64(void){
  //this section needs to happen without interrupts to make it thread safe
  __disable_irq();
  uint32_t timestamp_32 = __HAL_TIM_GET_COUNTER(MICRO_SEC_TIM_HANDLE);
  uint32_t ovf_cnt = g_usec_overflow_count;
  __enable_irq();

  uint64_t timestamp = (uint64_t)ovf_cnt << 32;
  timestamp |= (uint64_t)timestamp_32;
  return timestamp;
}



