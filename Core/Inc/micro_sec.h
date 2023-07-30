//
// Created by Matej Planinšek on 30/07/2023.
//

/**
 * @brief a set of timing functions with microsecond resolution
 * such as: delay, get timestamp, schedule interrupt...
 * Uses a 32bit timer at 170Mhz clock with 169 divider -> 1 tick per 1us. Overflows in 1.2ish hours.
 * Some overflow-corrected functions are provided (to generate absoulute 64bit timestamps). Warning: use wisely.
 * 64bit timestamp is good for 584942 years of on-time. Good enough
 */

#ifndef LIGHTSOAKFW_STM_MICRO_SEC_H
#define LIGHTSOAKFW_STM_MICRO_SEC_H

#include "stm32g4xx_hal.h"
#include "tim.h"

#define MICRO_SEC_TIM_HANDLE &htim2

extern volatile uint32_t g_usec_overflow_count;

void usec_init(void);
void usec_delay(uint32_t delay_us);
uint32_t usec_get_timestamp(void);

uint64_t usec_get_timestamp_64(void);
uint32_t usec_get_overflow_count(void);

void prv_usec_overflow_callback(void);





#endif //LIGHTSOAKFW_STM_MICRO_SEC_H
