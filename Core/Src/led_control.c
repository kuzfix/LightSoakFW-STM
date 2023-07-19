//
// Created by Matej Planinšek on 19/07/2023.
//
#include "led_control.h"


/**
 * @brief Initializes the LED control module
 */
void ledctrl_init(void){
  //set value to 0 first to be sure
  HAL_DAC_SetValue(LEDCTRL_DAC_HANDLE, LEDCTRL_DAC_CH, DAC_ALIGN_12B_R, 0);
  //start DAC
  HAL_DAC_Start(LEDCTRL_DAC_HANDLE, LEDCTRL_DAC_CH);
  //DAC is now outputing 0V

}

/**
 * @brief set raw value to DAC
 * @param dac_value raw DAC value
 */
void ledctrl_set_dac_raw(uint32_t dac_value){
  HAL_DAC_SetValue(LEDCTRL_DAC_HANDLE, LEDCTRL_DAC_CH, DAC_ALIGN_12B_R, dac_value);
}

/**
 * @brief Calculate raw DAC value from desired current.
 * Do this in advance and set dac with ledctrl_set_dac_raw() at the exact time.
 * @param current current in A
 * @return raw DAC value to get this LED current
 */
uint32_t ledctrl_get_raw_from_current(float current){
  // Check if current is within the range
  if (current > LED_CURRENT_MAX) {
    current = LED_CURRENT_MAX;
    dbg(Error, "Current request too high! Setting to max!\n\r");
  }
  else if (current < 0) {
    current = 0;
    dbg(Error, "Current request below 0! Setting to 0!\n\r");
  }

  //if 0 requested, force control voltage to 0, not LEDCTRL_ZERO_CURRENT_CTRL
  if(current == 0.0f){
    return 0;
  }

  // Calculate control voltage
  float control_voltage = (current * LEDCTRL_CURRENT_GAIN) + LEDCTRL_ZERO_CURRENT_CTRL;

  // Calculate corresponding raw value
  uint32_t dac_raw_val = (uint32_t) ((control_voltage / LEDCTRL_VREF) * LEDCTRL_DAC_MAX_RAW);

  // Check if the value is within the DAC range
  if (dac_raw_val > LEDCTRL_DAC_MAX_RAW) {
    dac_raw_val = LEDCTRL_DAC_MAX_RAW;
  }

  return dac_raw_val;
}


/**
 * @brief Set LED current. This function will calculate raw DAC value and set it.
 * Do not use if timing is critical.
 * @param current current in A
 */
void ledctrl_set_current(float current){
  //no need to calculate if 0 requested. Set to 0, not LEDCTRL_ZERO_CURRENT_CTRL to get 100% turn off
  if(current == 0){
    ledctrl_set_dac_raw(0);
    return;
  }
  uint32_t dac_raw_val = ledctrl_get_raw_from_current(current);
  ledctrl_set_dac_raw(dac_raw_val);
}


