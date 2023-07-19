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

void ledctrl_set_dac_raw(uint32_t dac_value){
  HAL_DAC_SetValue(LEDCTRL_DAC_HANDLE, LEDCTRL_DAC_CH, DAC_ALIGN_12B_R, dac_value);
}