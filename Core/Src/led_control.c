//
// Created by Matej Planinšek on 19/07/2023.
//
#include "led_control.h"

// coefficient to set current for requested illumination. Can be set by user through CLI as illlum - current point
float prv_ledctrl_illum_to_curr_coeff = 1.0f;

//currently set current
float prv_ledctrl_current_now_notempcomp = 0.0f;


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
  control_voltage /= LEDCTRL_PULLDOWN_FACTOR;

  // Calculate corresponding raw value
  uint32_t dac_raw_val = (uint32_t) ((control_voltage / LEDCTRL_VREF) * LEDCTRL_DAC_MAX_RAW);

  // Check if the value is within the DAC range
  if (dac_raw_val > LEDCTRL_DAC_MAX_RAW) {
    dac_raw_val = LEDCTRL_DAC_MAX_RAW;
  }

  return dac_raw_val;
}


/**
 * @brief Set LED current. This function will calculate raw DAC value and set it. Compensated for LED temperature.
 * Sets LED current, but is compensated for temperature, so that same current value produces same illumination at different temperatures.
 * Do not use if timing is critical.
 * @param current current in A
 */
void ledctrl_set_current_tempcomp(float current){
//  mainser_printf("SETLEDCURR:%f\r\n", current);
  //no need to calculate if 0 requested. Set to 0, not LEDCTRL_ZERO_CURRENT_CTRL to get 100% turn off
  if(current == 0){
    ledctrl_set_dac_raw(0);
    prv_ledctrl_current_now_notempcomp = 0;
    return;
  }
  //save current for temperature compensation handler (if led is on for long time)
  prv_ledctrl_current_now_notempcomp = current;
  current = ledctrl_compensate_current_for_temp(current);
  dbg(Debug, "ledctrl set current: %f\r\n", current);
  uint32_t dac_raw_val = ledctrl_get_raw_from_current(current);
  ledctrl_set_dac_raw(dac_raw_val);
}

/**
 * @brief Set LED illumination.
 * Do not use if timing is critical.
 * @param current current in A
 */
void ledctrl_set_illum(float illum){
  mainser_printf("SETLEDILLUM:\r\n");
  prv_meas_print_timestamp(usec_get_timestamp_64());
  mainser_printf("ILLUM:%f\r\n", illum);
  if(illum == 0){
    ledctrl_set_dac_raw(0);
    prv_ledctrl_current_now_notempcomp = 0;
    return;
  }
  // temperature compensation is done in ledctrl_set_current_tempcomp
  ledctrl_set_current_tempcomp(ledctrl_illumination_to_current(illum));
}

/**
 * @brief Converts illumination value to LED current Compensated for temperature. normalized to 0-1 suns. Compensated for LED temperature.
 * prv_ledctrl_illum_to_curr_coeff is used to set current for requested illumination. Can be set by user through CLI as illlum - current point
 * This parameter is not saved to flash and defaults to 1 at boot
 * Do not use if timing is critical.
 * @param current current in A
 */
float ledctrl_illumination_to_current(float illumination){
  //this is exact for 25degc, so needs to be compensated for temperature
  float curr = illumination * prv_ledctrl_illum_to_curr_coeff;
  return curr;
}

/**
 * @brief Returns current temperature of LED. Non-blocking: this just returns last known value. Sampling is done in handler.
 */
float ledctrl_get_temperature(void){
  return ds18b20_get_temp();
}

float ledctrl_compensate_current_for_temp(float current){
  float ledtemp = ledctrl_get_temperature();
  float tempdiff = ledtemp - LED_REF_TEMP;
  return current * (1.0f + (tempdiff * (-LED_PER_DEG_TEMP_COEFF)));
}



/**
 * @brief Prints led temperature to main serial
 */
void ledctrl_print_temperature_mainser(void){
  dbg(Debug, "ledctrl_print_temperature_mainser()\r\n");
  mainser_printf("LEDTEMP:\r\n");
  prv_meas_print_timestamp(usec_get_timestamp_64());
  mainser_printf("TEMP:%f\r\n", ds18b20_get_temp());
}

/**
 * @brief Takes a callibrate point (illumination[sun], current[A]) from user and calculates prv_ledctrl_illum_to_curr_coeff coefficient
 * @param illum illumination in suns
 * @param current current in A
 */
void ledctrl_calibrate_illum_curr(float illum, float current){
  prv_ledctrl_illum_to_curr_coeff = current / illum;
  mainser_printf("[A] per [sun]: %f\r\n", prv_ledctrl_illum_to_curr_coeff);
  dbg(Warning, "prv_ledctrl_illum_to_curr_coeff set to: %f\r\n", prv_ledctrl_illum_to_curr_coeff);
}

/**
 * @brief If LED is on, compensates the LED current for temperature. If LED is off, does nothing.
 * Call periodically.
 * Compensates only if LED is set by ledctrl_set_current_tempcomp or ledctrl_set_illum
 */
void ledctrl_handler(void){
  ledctrl_set_current_tempcomp(prv_ledctrl_current_now_notempcomp);
}
