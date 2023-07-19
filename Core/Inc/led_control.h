//
// Created by Matej Planinšek on 19/07/2023.
//

#ifndef LIGHTSOAKFW_STM_LED_CONTROL_H
#define LIGHTSOAKFW_STM_LED_CONTROL_H

#include "stm32g4xx_hal.h"
#include "dac.h"
#include "main.h"

#define LEDCTRL_DAC_HANDLE &hdac1
#define LEDCTRL_DAC_CH DAC_CHANNEL_1

void ledctrl_init(void);
void ledctrl_set_dac_raw(uint32_t dac_value);

#endif //LIGHTSOAKFW_STM_LED_CONTROL_H
